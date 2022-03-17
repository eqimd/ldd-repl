cmake_minimum_required(VERSION 3.10)

project(ldd-repl)
set(CMAKE_CXX_STANDARD 17)

include_directories("elf_file", "util")

add_executable(ldd-repl
        main.cpp elf_file/elf_file.cpp elf_file/elf_file.h util/util.cpp util/util.h)
