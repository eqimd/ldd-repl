#include <elf.h>
#include <unistd.h>
 
#include <cassert>
#include <cstdio>
#include <cstring>
 
#include <iostream>
#include <fstream>
#include <iterator>
#include <list>
#include <string>
#include <vector>


const std::string LD_LIB_PATH_VAR_NAME = "LD_LIBRARY_PATH";

std::string get_ld_library_path(char** envp);
std::list<std::string> get_needed_libraries(const char* fn);


int main(int argc, char** argv, char** envp) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <binary>\n";
    }

    for (auto& s : get_needed_libraries(argv[0])) {
        std::cout << s << "\n";
    }
}


std::string get_ld_library_path(char** envp) {
    for (char** env = envp; *env != 0; env++) {
        std::string s = *env;
        if (s.find(LD_LIB_PATH_VAR_NAME) != std::string::npos) {
            return std::string(*env).substr(LD_LIB_PATH_VAR_NAME.length() + 1); // +1 for symbol '=' because *env is LD_LIBRARY_PATH=.....
        }
    }

    return "";
}

std::list<std::string> get_needed_libraries(const char* fn) {
    std::vector<char> file_data = [fname = fn] {
        std::ifstream file(fname, std::ios::binary | std::ios::in);
        return std::vector<char>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>{});
    }();

    std::list<std::string> needed_libs;

    char *pybytes = file_data.data();

    const unsigned char expected_magic[] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};
    Elf64_Ehdr elf_hdr;

    memmove(&elf_hdr, pybytes, sizeof(elf_hdr));

    if (memcmp(elf_hdr.e_ident, expected_magic, sizeof(expected_magic)) != 0) {
        std::cerr << "Target is not an ELF executable\n";
        exit(1);
    }
    if (elf_hdr.e_ident[EI_CLASS] != ELFCLASS64) {
        std::cerr << "Sorry, only ELF-64 is supported.\n";
        exit(1);
    }
    if (elf_hdr.e_machine != EM_X86_64) {
        std::cerr << "Sorry, only x86-64 is supported.\n";
        exit(1);
    }

    char *cbytes = (char *)pybytes;

    size_t dynsect_off = 0;
    size_t dynsect_sz = 0;

    for (uint16_t i = 0; i < elf_hdr.e_shnum; i++) {
        size_t offset = elf_hdr.e_shoff + i * elf_hdr.e_shentsize;
        Elf64_Shdr shdr;

        memmove(&shdr, pybytes + offset, sizeof(shdr));

        if (shdr.sh_type == SHT_DYNAMIC) {
            dynsect_off = shdr.sh_offset;
            dynsect_sz = shdr.sh_size;
        }
    }

    if (!dynsect_off) {
        exit(1);
    }

    size_t dt_strtab_ofs = 0;

    for (size_t j = 0; j * sizeof(Elf64_Sym) < dynsect_sz; j++) {
        Elf64_Dyn dyn;
        size_t absoffset = dynsect_off + j * sizeof(Elf64_Dyn);

        memmove(&dyn, cbytes + absoffset, sizeof(dyn));

        if (dyn.d_tag == DT_STRTAB) {
            dt_strtab_ofs = dyn.d_un.d_val;
        }
    }
    for (size_t j = 0; j * sizeof(Elf64_Sym) < dynsect_sz; j++) {
        Elf64_Dyn dyn;
        size_t absoffset = dynsect_off + j * sizeof(Elf64_Dyn);

        memmove(&dyn, cbytes + absoffset, sizeof(dyn));
        if (dyn.d_tag == DT_NEEDED) {
            needed_libs.push_back(std::move(std::string(cbytes + dt_strtab_ofs + dyn.d_un.d_val)));
        }
    }

    return needed_libs;
}