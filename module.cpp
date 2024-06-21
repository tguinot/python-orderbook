#include <pybind11/pybind11.h>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <iostream>
#include "orderbook.hpp"

PYBIND11_MODULE(orderbook, m) {
    m.doc() = "pybind11 orderbook plugin";

    py::class_<SideBook>(m, "SideBook")
        .def(py::init<std::string, shm_mode, number>());

    py::class_<OrderbookReader>(m, "OrderbookReader")
        .def("init_shm", &OrderbookReader::init_shm)
        .def("bids_up_to_volume", &OrderbookReader::py_bids_up_to_volume)
        .def("asks_up_to_volume", &OrderbookReader::py_asks_up_to_volume)
        .def("snapshot_bids", &OrderbookReader::py_snapshot_bids)
        .def("snapshot_asks", &OrderbookReader::py_snapshot_asks)
        .def("snapshot_whole", &OrderbookReader::py_snapshot_whole)
        .def("bids_nonce", &OrderbookReader::py_bids_nonce)
        .def("asks_nonce", &OrderbookReader::py_asks_nonce)
        .def("first_price", &OrderbookReader::py_first_price);

    py::class_<OrderbookWriter>(m, "OrderbookWriter")
        .def(py::init<>())
        .def("init_shm", &OrderbookWriter::init_shm)
        .def("reset_content", &OrderbookWriter::reset_content)
        .def("clean_top_ask", &OrderbookWriter::clean_top_ask)
        .def("clean_top_bid", &OrderbookWriter::clean_top_bid)
        .def("bids_up_to_volume", &OrderbookWriter::py_bids_up_to_volume)
        .def("asks_up_to_volume", &OrderbookWriter::py_asks_up_to_volume)
        .def("snapshot_bids", &OrderbookWriter::py_snapshot_bids)
        .def("snapshot_asks", &OrderbookWriter::py_snapshot_asks)
        .def("snapshot_whole", &OrderbookWriter::py_snapshot_whole)
        .def("bids_nonce", &OrderbookWriter::py_bids_nonce)
        .def("asks_nonce", &OrderbookWriter::py_asks_nonce)
        .def("first_price", &OrderbookWriter::py_first_price)
        .def("set_quantity_at", &OrderbookWriter::py_set_quantity_at)
        .def("set_quantities_at", &OrderbookWriter::py_set_quantities_at)
        .def("set_entry", &OrderbookWriter::py_set_entry)
        .def("set_entries", &OrderbookWriter::py_set_entries);
}


