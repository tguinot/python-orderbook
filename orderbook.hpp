#include <iostream>
#include <string>
#include <tuple>
#include "sidebook.hpp"
#include "with_timed_lock.hpp"

#define BID_PATH_SUFFIX "_bids"
#define ASK_PATH_SUFFIX "_asks"

#define BID true
#define ASK false

typedef bool order_side;

class OrderbookBase {
  protected:
    std::string path_;
    SideBook *bids, *asks;
    std::pair<number**, int> _side_up_to_volume_(SideBook*, number);
    py::list _py_side_up_to_volume_(SideBook*, number);

  public:
    OrderbookBase(const std::string& path) : path_(path), bids(nullptr), asks(nullptr) {}
    virtual ~OrderbookBase();

    std::pair<number **, int> bids_up_to_volume(number);
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

class OrderbookReader : public OrderbookBase {
public:
    OrderbookReader(const std::string& path);
    ~OrderbookReader(){};
};


class OrderbookWriter: public OrderbookBase {
  public:
  	
    OrderbookWriter(const std::string& path);
    ~OrderbookWriter(){};
    void reset_content();
    void clean_top_ask();
    void clean_top_bid();
    void set_quantity_at (order_side, number, number);
    void set_quantity_at_no_lock (order_side, number, number);
    void fill_side_with(SideBook *sb, number fillNumber);
    void fill_asks_with(number fillNumber);
    void fill_bids_with(number fillNumber);
    void py_set_ask(const py::kwargs&); 
    void py_set_bid(const py::kwargs&);
    void py_set_bids(py::list, py::list);
    void py_set_asks(py::list, py::list);
    void insert_ask(number, number);
    void insert_ask_no_lock(number, number);
    void insert_bid(number, number);
    void insert_bid_no_lock(number, number);
    number** snapshot_to_limit(SideBook*, int);
 };

