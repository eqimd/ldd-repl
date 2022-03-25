#pragma once

#include <elf.h>

#include <fstream>
#include <string>
#include <vector>


class elf_file {
public:
    explicit elf_file(const std::string& fn);

    const std::vector<std::string>& get_needed_libraries() const;
    const std::vector<std::string>& get_rpaths() const;
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
    std::vector<std::string> _rpaths;

    std::vector<std::string> _needed_libs;
};