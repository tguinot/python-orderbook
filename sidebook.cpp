#include "sidebook.hpp"
#include <iostream>
#include "with_timed_lock.hpp"
#include <algorithm>

number quantity(sidebook_content::iterator loc) {
    return (*loc)[1];
}

number price(sidebook_content::iterator loc) {
    return (*loc)[0];
}

number quantity(sidebook_content::reverse_iterator loc) {
    return (*loc)[1];
}

number price(sidebook_content::reverse_iterator loc) {
    return (*loc)[0];
}

void set_quantity(sidebook_content::iterator loc, number qty) {
    (*loc)[1] = qty;
}

void set_price(sidebook_content::iterator loc, number price) {
    (*loc)[0] = price;
}

void set_quantity(sidebook_content::reverse_iterator loc, number qty) {
    (*loc)[1] = qty;
}

void set_price(sidebook_content::reverse_iterator loc, number price) {
    (*loc)[0] = price;
}

bool compare_s(orderbook_entry_type a, orderbook_entry_type b){
    return (a[0] < b[0]);
}

bool compare_b(orderbook_entry_type a, orderbook_entry_type b){
    return (a[0] > b[0]);
}

void SideBook::setup_segment(std::string path, shm_mode mode){
    try {
        if (mode == read_write_shm) {
            std::cout << "Creating/opening shared memory at path: " << path << std::endl;
            boost::interprocess::shared_memory_object::remove(path.c_str());
            segment = new managed_shared_memory(open_or_create, path.c_str(), 90000);
        }
        else if (mode == read_shm) {
            std::cout << "Opening shared memory at path: " << path << std::endl;
            segment = new managed_shared_memory(open_only, path.c_str());
        }
        std::cout << "Successfully setup shared memory segment at: " << path << std::endl;
    } catch (const boost::interprocess::interprocess_exception& ex) {
        std::cerr << "Failed to setup shared memory segment: " << ex.what() << std::endl;
        std::cerr << "Error code: " << ex.get_error_code() << std::endl;
        std::cerr << "Path attempted: " << path << std::endl;
        throw;
    }
}
SideBook::SideBook(std::string path, shm_mode mode, number fill_value): segment(nullptr), mutex(nullptr) {
    segment_path = path;
    mutex_path = path + "_mutex";
    mutex = new named_upgradable_mutex(open_or_create, mutex_path.c_str());
    setup_segment(path, mode);
    data = segment->find_or_construct< sidebook_content > ("unique")();
    update_number = segment->find_or_construct< long > ("nonce")();
    default_value = fill_value;
    book_mode = mode;
}


number** SideBook::extract_to_limit(int limit){
    number** result = new number*[limit];
    int i = 0;
    for (sidebook_ascender it=data->begin(); it!=data->end(); i++){
        if (i >= limit || price(it) == default_value)
            break;

        result[i] = new number[2];
        result[i][0] = price(it);
        result[i][1] = quantity(it);
    }
    return result;
}

sidebook_ascender SideBook::begin() {
    return data->begin();
}

sidebook_ascender SideBook::end() {
    return data->end();
}

void SideBook::insert_at_place(orderbook_entry_type to_insert, sidebook_content::iterator loc){
    if (loc == data->end())
        return;
    if ((*loc)[0] != to_insert[0] && to_insert[1].numerator() != 0){
        std::rotate(loc, data->end()-1, data->end());

        (*loc)[0] = to_insert[0];
        (*loc)[1] = to_insert[1];
    } else if ((*loc)[0] == to_insert[0] && to_insert[1].numerator() == 0) {
        std::copy(loc+1,data->end(), loc);
        data->back()[0] = default_value;
        data->back()[1] = default_value;
    } else if (to_insert[1].numerator() != 0){
        (*loc)[1] = to_insert[1];
    }
    (*update_number)++;
}

void SideBook::clean_first_limit() {

    with_timed_lock(mutex, [&]() {
        std::copy(data->begin()+1, data->end(), data->begin());
        data->back()[0] = default_value;
        data->back()[1] = default_value;
    });

}

