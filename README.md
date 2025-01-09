____  _   _ __  __    ___          _           _                 _    
/ ___|| | | |  \/  |  / _ \ _ __ __| | ___ _ __| |__   ___   ___| | __
\___ \| |_| | |\/| | | | | | '__/ _` |/ _ \ '__| '_ \ / _ \ / _ \ |/ /
 ___) |  _  | |  | | | |_| | | | (_| |  __/ |  | |_) | (_) |  __/   < 
|____/|_| |_|_|  |_|  \___/|_|  \__,_|\___|_|  |_.__/ \___/ \___|_|\_\


# SHM Orderbook

Shared memory orderbook implementation in C++ with Python bindings

## What is this?

This is a fast orderbook implementation that:
- uses shared memory for low latency
- uses rational numbers for precise price representation
- exposes a clean Python API via pybind11

## Requirements

* C++11
* CMake >= 3.12
* Python 3.x with development headers
* Boost (for interprocess)
* pybind11
* Eigen 3.4.0 (auto-fetched by CMake)

## Build it

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Usage

```python
from fractions import Fraction
from orderbook import OrderbookWriter, OrderbookReader

# create a new orderbook
writer = OrderbookWriter("test_book")

# set some orders
writer.set_bid(price=Fraction(100, 1), quantity=Fraction(10, 1))  # bid 10 @ $100
writer.set_ask(price=Fraction(101, 1), quantity=Fraction(5, 1))   # offer 5 @ $101

# read from another process
reader = OrderbookReader("test_book")
bids = reader.snapshot_bids(10)  # get top 10 bids
asks = reader.snapshot_asks(10)  # get top 10 asks
```

## Features

### Shared memory
- uses boost::interprocess for shared memory communication
- perfect for HFT systems where you need multiple processes accessing the same orderbook
- reader process does not blocks the writer

### Precise number representation  
- all prices and quantities are rational numbers
- no floating point

### Python bindings
- clean pythonic interface
- automatic conversion between C++ rational numbers and Python Fraction objects
- supports bulk updates for efficiency

## Performance

- O(log n) updates and reads
- negligible overhead from Python bindings

## Limitations

- fixed-size shared memory segment
- single writer, multiple reader design
