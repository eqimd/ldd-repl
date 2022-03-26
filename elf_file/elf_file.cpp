#include "elf_file.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <unistd.h>
#include <cstring>
#include <iterator>
#include <iostream>
#include <vector>



elf_file::elf_file(const Filename& fn) {
    std::ifstream file(fn, std::ios::binary | std::ios::in);

    file.read(reinterpret_cast<char*>(&_elf_hdr), sizeof(_elf_hdr));

    read_dynsect_off_and_sz(file);
    read_load_addr(file);
    read_dt_strtab_ofs(file);
    read_needed_libs_and_rpath(file);

    file.close();
}

const std::vector<Filename>& elf_file::get_needed_libraries() const {
    return _needed_libs;
}

void elf_file::read_needed_libs_and_rpath(std::ifstream& file) {
    Elf64_Dyn dyn;
    for (Elf64_Xword j = 0; j * sizeof(dyn) < _dynsect_sz; j++) {
        size_t absoffset = _dynsect_off + j * sizeof(dyn);

        file.seekg(absoffset);
        file.read(reinterpret_cast<char*>(&dyn), sizeof(dyn));
        if (dyn.d_tag == DT_NEEDED) {
            file.seekg(_dt_strtab_ofs + dyn.d_un.d_ptr);
            Filename s;
            std::getline(file, s, '\0');
            _needed_libs.push_back(std::move(s));
        } else if (dyn.d_tag == DT_RUNPATH || dyn.d_tag == DT_RPATH) {
            file.seekg(_dt_strtab_ofs + dyn.d_un.d_val);
            Path rpath;
            std::getline(file, rpath, '\0');
            boost::split(_rpaths, rpath, boost::is_any_of(":"));
        }
    }
}

void elf_file::read_load_addr(std::ifstream& file) {
    file.seekg(_elf_hdr.e_phoff);
    Elf64_Phdr phdr;
    for (Elf64_Half i = 0; i < _elf_hdr.e_phnum; i++) {
        file.read(reinterpret_cast<char*>(&phdr), sizeof(phdr));
        if (phdr.p_type == PT_LOAD) {
            _load_addr = phdr.p_paddr - phdr.p_offset;
            break;
        }
    }
}

void elf_file::read_dynsect_off_and_sz(std::ifstream& file) {
    file.seekg(_elf_hdr.e_phoff);
    Elf64_Phdr phdr;
    for (Elf64_Half i = 0; i < _elf_hdr.e_phnum; i++) {
        file.read(reinterpret_cast<char*>(&phdr), sizeof(phdr));
        if (phdr.p_type == PT_DYNAMIC) {
            _dynsect_off = phdr.p_offset;
            _dynsect_sz = phdr.p_filesz;
            break;
        }
    }
}
void elf_file::read_dt_strtab_ofs(std::ifstream& file) {
    file.seekg(_dynsect_off);
    Elf64_Dyn dyn;
    for (Elf64_Xword j = 0; j * sizeof(dyn) < _dynsect_sz; j++) {
        file.read(reinterpret_cast<char*>(&dyn), sizeof(dyn));
        if (dyn.d_tag == DT_STRTAB) {
            _dt_strtab_ofs = dyn.d_un.d_ptr - _load_addr;
            break;
        }
    }
}

const std::vector<Path>& elf_file::get_rpaths() const {
    return _rpaths;
}
