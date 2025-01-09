#include "sidebook.hpp"
#include "with_timed_lock.hpp"

using namespace boost::interprocess;

py::object fractions_module = py::module_::import("fractions");
py::object fraction_class = fractions_module.attr("Fraction");

py::object namedtuple = py::module_::import("collections").attr("namedtuple");
py::object Entry = namedtuple("Entry", "price quantity");

py::list SideBook::py_extract_to_limit(int limit){
  py::list result;
  int i = 0;
  for (sidebook_ascender it=data->begin(); it!=data->end(); it++){
    if (i >= limit || price(it) == default_value)
      break;
    py::object frac_qty = fraction_class(quantity(it).numerator(), quantity(it).denominator());
    py::object frac_price = fraction_class(price(it).numerator(), price(it).denominator());

    result.append(Entry(frac_price, frac_qty));

    i++;
  }
  return result;
}

py::list SideBook::py_snapshot_to_limit(int limit){
  pybind11::list result;
  with_timed_lock(mutex, [&]() {
    result = py_extract_to_limit(limit);
  });
  return result;
}


