#pragma once

#include <elf.h>

#include <fstream>
#include <string>
#include <vector>

using Filename = std::string;
using Path = std::string;


class elf_file {
public:
    explicit elf_file(const Filename& fn);

    const std::vector<Filename>& get_needed_libraries() const;
    const std::vector<Path>& get_rpaths() const;
private:
    void read_needed_libs_and_rpath(std::ifstream& file);
    void read_dynsect_off_and_sz(std::ifstream& file);
    void read_dt_strtab_ofs(std::ifstream& file);
    void read_load_addr(std::ifstream& file);

    Elf64_Ehdr _elf_hdr;

    Elf64_Off _dynsect_off = 0;
    Elf64_Xword _dynsect_sz = 0;
    Elf64_Xword _dt_strtab_ofs = 0;
    Elf64_Xword _load_addr = 0;
    std::vector<Path> _rpaths;

    std::vector<Filename> _needed_libs;
};