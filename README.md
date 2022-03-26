## A replica of `ldd` library
---
### Usage
```bash
./ldd-repl <binary>
```
### Build
You need to have [Boost](https://www.boost.org/) library installed,
usually for Debian-based systems it is enough to run 
```bash
sudo apt-get install libboost-all-dev
```
For building with `cmake`, run
```bash
cmake -B build && cmake --build build && rm -r build
```
Now you have an executable `ldd-repl`.
