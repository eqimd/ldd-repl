#include "elf_file.h"

#include <unistd.h>
#include <cstring>
#include <iterator>
#include <iostream>


elf_file::elf_file(const std::string& fn) {
    std::ifstream file(fn, std::ios::binary | std::ios::in);
    const unsigned char expected_magic[] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};

    file.read(reinterpret_cast<char*>(&_elf_hdr), sizeof(_elf_hdr));

    read_dynsect_off_and_sz(file);
    read_dt_strtab_ofs(file);
    read_needed_libs_and_rpath(file);

    file.close();
}

elf_file::elf_file(char* fn) {
    elf_file(std::string(fn));
}

elf_file::~elf_file() {}

// size_t elf_file::get_dynsect_offset() const {
//     return _dynsect_off;
// }
// size_t elf_file::get_dynsect_size() const {
//     return _dynsect_sz;
// }
// size_t elf_file::get_dt_strtab_offset() const {
//     return _dt_strtab_ofs;
// }

// bool elf_file::is_elf_error() const {
//     return _elf_error;
// }

// std::string elf_file::get_error_message() const {
//     return _error_msg;
// }

std::list<std::string> elf_file::get_needed_libraries() const {
    return _needed_libs;
}

void elf_file::read_needed_libs_and_rpath(std::ifstream& file) {
    Elf64_Dyn dyn;
    for (size_t j = 0; j * sizeof(Elf64_Sym) < _dynsect_sz; j++) {
        size_t absoffset = _dynsect_off + j * sizeof(Elf64_Dyn);

        file.seekg(absoffset);
        file.read(reinterpret_cast<char*>(&dyn), sizeof(dyn));
        if (dyn.d_tag == DT_NEEDED) {
            file.seekg(_dt_strtab_ofs + dyn.d_un.d_val);
            std::string s;
            std::getline(file, s, '\0');
            _needed_libs.push_back(std::move(s));
        } else if (dyn.d_tag == DT_RUNPATH || dyn.d_tag == DT_RPATH) {
            file.seekg(_dt_strtab_ofs + dyn.d_un.d_val);
            std::getline(file, _rpath, '\0');
        }
    }
}


void elf_file::read_dynsect_off_and_sz(std::ifstream& file) {
    file.seekg(_elf_hdr.e_shoff);
    Elf64_Shdr shdr;
    for (uint16_t i = 0; i < _elf_hdr.e_shnum; i++) {
        file.read(reinterpret_cast<char*>(&shdr), sizeof(shdr));
        if (shdr.sh_type == SHT_DYNAMIC) {
            _dynsect_off = shdr.sh_offset;
            _dynsect_sz = shdr.sh_size;
            break;
        }
    }
}
void elf_file::read_dt_strtab_ofs(std::ifstream& file) {
    file.seekg(_dynsect_off);
    Elf64_Dyn dyn;
    for (size_t j = 0; j * sizeof(Elf64_Sym) < _dynsect_sz; j++) {
        file.read(reinterpret_cast<char*>(&dyn), sizeof(dyn));
        if (dyn.d_tag == DT_STRTAB) {
            _dt_strtab_ofs = dyn.d_un.d_val;
            break;
        }
    }
}

std::string elf_file::get_rpath() const {
    return _rpath;
}
