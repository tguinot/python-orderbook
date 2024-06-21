#include <iostream>
#include <string>
#include "sidebook.hpp"
#include <tuple>

#define BID_PATH_SUFFIX "_bids"
#define ASK_PATH_SUFFIX "_asks"

#define BID true
#define ASK false

typedef bool order_side;


class OrderbookReader {
  protected:
    SideBook *bids, *asks;
    std::pair<number**, int> _side_up_to_volume_(SideBook*, number);
    py::list _py_side_up_to_volume_(SideBook*, number);


  public:
    virtual void init_shm (std::string);

    std::pair<number**, int> bids_up_to_volume (number);
    std::pair<number**, int> asks_up_to_volume (number);

    py::list py_asks_up_to_volume(base_number n, base_number d);
    py::list py_bids_up_to_volume(base_number n, base_number d);

    py::list py_snapshot_bids(int);
    py::list py_snapshot_asks(int);
    py::tuple py_snapshot_whole(int);

    long py_bids_nonce();
    long py_asks_nonce();

    py::tuple py_first_price(bool);

    number first_price (bool side) {
      return side == BID ? price(bids->begin()) : price(asks->begin());
    }

    void display_side (order_side);
};


class OrderbookWriter: public OrderbookReader {
  public:
  	
    void init_shm (std::string);
    void reset_content();
    void clean_top_ask();
    void clean_top_bid();
    void set_quantity_at (order_side, number, number);
    void set_quantity_at_no_lock (order_side, number, number);
    void py_set_quantity_at (order_side, base_number, base_number, base_number, base_number);
    void py_set_quantities_at(order_side, py::list, py::list);
    void py_set_entry(order_side, py::object, py::object);    
    void py_set_entries(order_side, py::list, py::list);
 };

