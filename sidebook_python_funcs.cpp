#include "sidebook.hpp"
#include <boost/interprocess/sync/lock_options.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

using namespace boost::interprocess;
using namespace boost::posix_time;

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
  scoped_lock<named_upgradable_mutex> lock(*mutex, defer_lock);
  ptime locktime(microsec_clock::local_time());
  locktime = locktime + milliseconds(75);
  
  bool acquired = lock.timed_lock(locktime);
  if (!acquired) {
    std::cout << "Unable to acquire memory in py_snapshot_to_limit" << std::endl;
  } else {
    lock.unlock();
  }
  
  return py_extract_to_limit(limit);
}


