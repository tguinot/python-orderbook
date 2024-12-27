#include "orderbook.hpp"
#include <unistd.h>
#include <iostream>
#include <boost/interprocess/sync/lock_options.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
#include <iomanip>

using namespace boost::interprocess;
using namespace boost::posix_time;

void OrderbookReader::init_shm(std::string path){
  bids = new SideBook(path + BID_PATH_SUFFIX, read_shm, ZEROVAL);
  asks = new SideBook(path + ASK_PATH_SUFFIX, read_shm, MAXVAL);
}

std::pair<number**, int> OrderbookReader::_side_up_to_volume_(SideBook *sb, number target_volume) {
  number** result = new number*[100];
  int i = 0;
  scoped_lock<named_upgradable_mutex> lock(*(sb->mutex), defer_lock);
  ptime locktime(microsec_clock::local_time());
  locktime = locktime + milliseconds(75);
  
  bool acquired = lock.timed_lock(locktime);
  for (sidebook_ascender it=sb->begin(); it!=sb->end(); ++it){
      if (price(it) == ZEROVAL)
        break;
      target_volume -= quantity(it);
      result[i] = new number[2];
      if (target_volume <= ZEROVAL) {
        result[i][0] = price(it);
        result[i][1] = (quantity(it) + target_volume);
        break;
      }

      result[i][0] = price(it);
      result[i][1] = quantity(it);
      i++;
  }
  if (!acquired) {
    std::cout << "Unable to acquire memory in _side_up_to_volume_" << std::endl;
  }else {
    lock.unlock();
  }

  return std::pair<number**, int>(result, i);
}

std::pair<number**, int> OrderbookReader::asks_up_to_volume(number target_volume) {
  return _side_up_to_volume_(asks, target_volume);
}

std::pair<number**, int> OrderbookReader::bids_up_to_volume(number target_volume) {
  return _side_up_to_volume_(bids, target_volume);
}

void OrderbookReader::display_side (order_side side) {
  if (side == ASK) {
    for (sidebook_ascender it=asks->begin(); it!=asks->end() && price(it)!= 0; ++it)
      std::cout << "ASK: " << std::setprecision(15) << price(it) << " => " << std::setprecision(20) << quantity(it) << '\n';
  } else if (side == BID) {
    for (sidebook_ascender it=bids->begin(); it!=bids->end() && price(it)!= 0; ++it)
      std::cout << "BID: " << std::setprecision(15) << price(it) << " => " << std::setprecision(20) << quantity(it) << '\n';
  }
}

void OrderbookWriter::reset_content(){
    bids->reset_content();
    asks->reset_content();
}

void OrderbookWriter::set_quantity_at (order_side side, number new_quantity, number new_price) {  
  if (side == ASK)
    asks->insert_ask(new_price, new_quantity);
  else if (side == BID)
    bids->insert_bid(new_price, new_quantity);
}

void OrderbookWriter::set_quantity_at_no_lock(order_side side, number new_quantity, number new_price)
{
  if (side == ASK)
    asks->insert_ask_no_lock(new_price, new_quantity);
  else if (side == BID)
    bids->insert_bid_no_lock(new_price, new_quantity);
}

void OrderbookWriter::init_shm(std::string path) {
  bids = new SideBook(path + BID_PATH_SUFFIX, read_write_shm, ZEROVAL);
  asks = new SideBook(path + ASK_PATH_SUFFIX, read_write_shm, MAXVAL);
}
