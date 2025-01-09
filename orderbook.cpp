#include "orderbook.hpp"
#include <unistd.h>
#include <iostream>
#include <iomanip>

using namespace boost::interprocess;

OrderbookReader::OrderbookReader(const std::string& path) : OrderbookBase(path) {
  bids = new SideBook(path_ + BID_PATH_SUFFIX, read_shm, ZEROVAL);
  asks = new SideBook(path_ + ASK_PATH_SUFFIX, read_shm, MAXVAL);
}

OrderbookBase::~OrderbookBase() {
  if (bids) {
    std::cout << "Deleting bids" << std::endl;
    delete bids;
    bids = nullptr;
  }
  if (asks) {
    std::cout << "Deleting asks" << std::endl;
    delete asks;
    asks = nullptr;
  }
}

OrderbookWriter::OrderbookWriter(const std::string& path) : OrderbookBase(path) {
  bids = new SideBook(path_ + BID_PATH_SUFFIX, read_write_shm, ZEROVAL);
  asks = new SideBook(path_ + ASK_PATH_SUFFIX, read_write_shm, MAXVAL);
  reset_content();
}


std::pair<number**, int> OrderbookBase::_side_up_to_volume_(SideBook *sb, number target_volume) {
  number** result = new number*[100];
  int i = 0;
  sidebook_ascender it;

  with_timed_lock(sb->mutex, [&]() {
    for (it=sb->begin(); it!=sb->end(); ++it){
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
  });

  return std::pair<number**, int>(result, i);
}

std::pair<number**, int> OrderbookBase::asks_up_to_volume(number target_volume) {
  return _side_up_to_volume_(asks, target_volume);
}

std::pair<number**, int> OrderbookBase::bids_up_to_volume(number target_volume) {
  return _side_up_to_volume_(bids, target_volume);
}

void OrderbookBase::display_side (order_side side) {
  if (side == ASK) {
    for (sidebook_ascender it=asks->begin(); it!=asks->end() && price(it)!= 0; ++it)
      std::cout << "ASK: " << std::setprecision(15) << price(it) << " => " << std::setprecision(20) << quantity(it) << '\n';
  } else if (side == BID) {
    for (sidebook_ascender it=bids->begin(); it!=bids->end() && price(it)!= 0; ++it)
      std::cout << "BID: " << std::setprecision(15) << price(it) << " => " << std::setprecision(20) << quantity(it) << '\n';
  }
}

void OrderbookWriter::reset_content(){
    fill_side_with(bids, ZEROVAL);
    fill_side_with(asks, MAXVAL);
}

void OrderbookWriter::set_quantity_at (order_side side, number new_quantity, number new_price) {  
  if (side == ASK){
    insert_ask(new_price, new_quantity);
  }
  else if (side == BID){
    insert_bid(new_price, new_quantity);
  }
}

void OrderbookWriter::set_quantity_at_no_lock(order_side side, number new_quantity, number new_price)
{
  if (side == ASK)
    insert_ask_no_lock(new_price, new_quantity);
  else if (side == BID)
    insert_bid_no_lock(new_price, new_quantity);
}

void OrderbookWriter::fill_side_with(SideBook *sb, number fillNumber){
    sidebook_ascender it;
  
    with_timed_lock(sb->mutex, [&]() {
      for (it= sb->begin(); it!=sb->end(); it++){
        set_price(it, fillNumber);
        set_quantity(it, fillNumber);
      }
      (*(sb->update_number))++;
    });
}

void OrderbookWriter::fill_asks_with(number fillNumber){
    fill_side_with(asks, fillNumber);
}

void OrderbookWriter::fill_bids_with(number fillNumber){
    fill_side_with(bids, fillNumber);
}

void OrderbookWriter::insert_ask(number new_price, number new_quantity) {
    orderbook_entry_type to_insert = {new_price, new_quantity};

    sidebook_content::iterator loc;
    with_timed_lock(asks->mutex, [&]() {
      loc = std::lower_bound<sidebook_content::iterator, orderbook_entry_type>(asks->begin(), asks->end(), to_insert, compare_s);
      asks->insert_at_place(to_insert, loc);
    });
}

void OrderbookWriter::insert_ask_no_lock(number new_price, number new_quantity) {
    orderbook_entry_type to_insert = {new_price, new_quantity};

    sidebook_content::iterator loc = std::lower_bound<sidebook_content::iterator, orderbook_entry_type>(asks->begin(), asks->end(), to_insert, compare_s);
    asks->insert_at_place(to_insert, loc);
}

void OrderbookWriter::insert_bid(number new_price, number new_quantity) {
    orderbook_entry_type to_insert = {new_price, new_quantity};

    sidebook_content::iterator loc;
    with_timed_lock(bids->mutex, [&]() {
      loc = std::lower_bound<sidebook_content::iterator, orderbook_entry_type>(bids->begin(), bids->end(), to_insert, compare_b);
      bids->insert_at_place(to_insert, loc);
    });
}

void OrderbookWriter::insert_bid_no_lock(number new_price, number new_quantity) {
    orderbook_entry_type to_insert = {new_price, new_quantity};

    sidebook_content::iterator loc = std::lower_bound<sidebook_content::iterator, orderbook_entry_type>(bids->begin(), asks->end(), to_insert, compare_b);
    bids->insert_at_place(to_insert, loc);
}

number** OrderbookWriter::snapshot_to_limit(SideBook *sb, int limit){
    number** result;
    with_timed_lock(sb->mutex, [&]() {
      result = sb->extract_to_limit(limit);
    });
    return result;
}