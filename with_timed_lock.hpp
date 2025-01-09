#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;

template<typename F>
void with_timed_lock(named_upgradable_mutex* mutex, F operation) {
    scoped_lock<named_upgradable_mutex> lock(*mutex, defer_lock);
    ptime locktime(microsec_clock::local_time());
    locktime = locktime + milliseconds(2);

    bool acquired = lock.timed_lock(locktime);

    if (!acquired) {
        std::cout << "Unable to acquire memory in insert_ask" << std::endl;
        return;
    }

    operation();
    lock.unlock();
}