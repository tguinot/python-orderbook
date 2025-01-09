#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/sync/named_upgradable_mutex.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>
#include <array>
#include "skiplist.hpp"
#include <boost/rational.hpp>
#include <pybind11/eigen.h>
#include <boost/config.hpp>
#include <utility>
#include <vector>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h> // Everything needed for embedding
#include <pybind11/stl.h>

#define SIDEBOOK_SIZE       2000
#define ZEROVAL             number(0, 1)
#define MAXVAL              number(2147483645, 1)

using namespace boost::interprocess;
using boost::rational;
namespace py = pybind11;


enum shm_mode { read_shm, read_write_shm };

typedef int64_t base_number;
typedef managed_shared_memory::segment_manager 									 	                  segment_manager_t;
typedef allocator<void, segment_manager_t>                           				        void_allocator;

typedef rational<base_number>                                                       number;
typedef std::array<number, 2>                                                       orderbook_entry_type;
typedef std::pair<number, number>                                                   orderbook_entry_rep;
typedef std::vector<orderbook_entry_rep >                                           orderbook_extract;

typedef std::array< orderbook_entry_type, SIDEBOOK_SIZE>                            sidebook_content;
typedef sidebook_content::iterator                                                  sidebook_ascender;
typedef sidebook_content::reverse_iterator                                          sidebook_descender;

number quantity(sidebook_content::iterator loc);

number price(sidebook_content::iterator loc);

number quantity(sidebook_content::reverse_iterator loc);

number price(sidebook_content::reverse_iterator loc);

bool compare_s(orderbook_entry_type a, orderbook_entry_type b);

bool compare_b(orderbook_entry_type a, orderbook_entry_type b);


void set_quantity(sidebook_content::iterator, number);
void set_price(sidebook_content::iterator, number);
void set_quantity(sidebook_content::reverse_iterator, number);
void set_price(sidebook_content::reverse_iterator, number);

class SideBook {
    mapped_region *region;
    managed_shared_memory *segment;
    sidebook_content *data;
    py::object np_data;
    void_allocator *allocator;
    number default_value;
    shm_mode book_mode;
    std::string segment_path;
    std::string mutex_path;

    //void fill_with(number);
    void setup_segment (std::string, shm_mode);
    
	public:
        SideBook(std::string, shm_mode, number);
        void insert_at_place(orderbook_entry_type, sidebook_content::iterator);
        named_upgradable_mutex *mutex;
        long *update_number;

        number** snapshot_to_limit(int);
        number** extract_to_limit(int);
        number** side_up_to_volume(number);

        py::list py_snapshot_to_limit(int);
        py::list py_extract_to_limit(int);

        void clean_first_limit();

        number get_default_value() {
          return default_value;
        };

        sidebook_ascender begin();
        sidebook_ascender end();
};


