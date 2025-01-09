#include <tuple>
#include "orderbook.hpp"

py::list OrderbookBase::_py_side_up_to_volume_(SideBook *sb, number target_volume) {
  py::list result;
  sharable_lock<named_upgradable_mutex> rlock(*(sb->mutex));
  for (sidebook_ascender it=sb->begin(); it!=sb->end(); ++it){
     if (price(it) == sb->get_default_value())
       break;
     target_volume -= quantity(it);
     if (target_volume <= ZEROVAL) {
       number actual_quantity = quantity(it) + target_volume;
       result.append(py::make_tuple(py::make_tuple(price(it).numerator(), price(it).denominator()), py::make_tuple(actual_quantity.numerator(), actual_quantity.denominator())));
   
       break;
     }
    result.append(py::make_tuple(py::make_tuple(price(it).numerator(), price(it).denominator()), py::make_tuple(quantity(it).numerator(), quantity(it).denominator())));
   }
   return result;
}

number number_from_fraction(py::object fraction) {
  int64_t new_number_n = fraction.attr("numerator").cast<int64_t>();
  int64_t new_number_d = fraction.attr("denominator").cast<int64_t>();

  return number(new_number_n, new_number_d);
}

void OrderbookWriter::py_set_ask(const py::kwargs& kwargs)
{
    if (!kwargs.contains("price") || !kwargs.contains("quantity")) {
        throw py::value_error("Required keyword arguments: price and quantity");
    }
    
    py::object new_price = kwargs["price"];
    py::object new_qty = kwargs["quantity"];
    
    set_quantity_at(false, number_from_fraction(new_qty), number_from_fraction(new_price));
}

void OrderbookWriter::py_set_bid(const py::kwargs& kwargs)
{
    if (!kwargs.contains("price") || !kwargs.contains("quantity")) {
        throw py::value_error("Required keyword arguments: price and quantity");
    }
    
    py::object new_price = kwargs["price"];
    py::object new_qty = kwargs["quantity"];
    set_quantity_at(true, number_from_fraction(new_qty), number_from_fraction(new_price));
}

void OrderbookWriter::py_set_asks(py::list new_qties, py::list new_prices)
{    
    with_timed_lock(asks->mutex, [&]() {
        for (int i = 0; i < len(new_qties); ++i) {
            py::object new_qty = new_qties[i];
            py::object new_price = new_prices[i];
            set_quantity_at_no_lock(false, number_from_fraction(new_qty), number_from_fraction(new_price));
        }
    });
}

void OrderbookWriter::py_set_bids(py::list new_qties, py::list new_prices)
{    
    with_timed_lock(bids->mutex, [&]() {
        for (int i = 0; i < len(new_qties); ++i) {
            py::object new_qty = new_qties[i];
            py::object new_price = new_prices[i];
            set_quantity_at_no_lock(true, number_from_fraction(new_qty), number_from_fraction(new_price));
        }
    });
}

py::list OrderbookBase::py_bids_up_to_volume(base_number n, base_number d) {
  return _py_side_up_to_volume_(bids, number(n, d));
}

py::list OrderbookBase::py_asks_up_to_volume(base_number n, base_number d) {
  return _py_side_up_to_volume_(asks, number(n, d));
}

py::list OrderbookBase::py_snapshot_bids(int limit) {
  return bids->py_snapshot_to_limit(limit);
}

py::list OrderbookBase::py_snapshot_asks(int limit) {
  return asks->py_snapshot_to_limit(limit);
}

void OrderbookWriter::clean_top_ask() {
  return asks->clean_first_limit();
}

void OrderbookWriter::clean_top_bid() {
  return bids->clean_first_limit();
}

long OrderbookBase::py_bids_nonce() {
  long result;
  
  with_timed_lock(bids->mutex, [&]() {
      result = *(bids->update_number);
  });

  return result;
}

long OrderbookBase::py_asks_nonce() {
    long result;

    with_timed_lock(asks->mutex, [&]() {
      result = *(asks->update_number);
    });

    return result;
}

py::tuple OrderbookBase::py_snapshot_whole(int limit) {
  py::list snapped_bids; 
  py::list snapped_asks;

  with_timed_lock(bids->mutex, [&]() {
    snapped_bids = bids->py_extract_to_limit(limit);
  });

  with_timed_lock(asks->mutex, [&]() {
    snapped_asks = asks->py_extract_to_limit(limit);
  });

  return py::make_tuple(snapped_bids, snapped_asks);
}

py::tuple OrderbookBase::py_first_price (bool side) {
    number top_price = first_price(side);
    return py::make_tuple(top_price.numerator(), top_price.denominator());
}
