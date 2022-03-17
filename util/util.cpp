#include "util.h"
#include "elf_file.h"

#include <string>
#include <list>
#include <utility>
#include <algorithm>
#include <filesystem>
#include <iostream>


std::string read_ld_library_path(char** envp) {
    for (char** env = envp; *env != 0; env++) {
        std::string s = *env;
        if (s.find(LD_LIB_PATH_VAR_NAME) != std::string::npos) {
            return std::string(*env).substr(LD_LIB_PATH_VAR_NAME.length() + 1); // +1 for symbol '=' because *env is LD_LIBRARY_PATH=.....
        }
    }

    return "";
}

void parse_needed_libs_and_paths(
    std::string& libname,
    const std::list<std::string>& paths,
    std::list<std::pair<std::string, std::string>>& libs_and_paths
) {

    elf_file ef(libname);
    std::string ef_rpath = ef.get_rpath();
    for (auto& lib : ef.get_needed_libraries()) {
        auto lib_pos = std::find_if(libs_and_paths.begin(), libs_and_paths.end(),
                            [libname](std::pair<std::string, std::string> const &b) { 
                                return b.first == libname; 
                            });
        if (lib_pos != libs_and_paths.end()) {
            continue;
        }

        bool found = false;
        if (ef_rpath != "" && std::filesystem::exists(ef_rpath + "/" + lib)) {
            libs_and_paths.push_back({lib, ef_rpath});
        } else {
            bool found = false;
            for (auto& path : paths) {
                if (std::filesystem::exists(path + "/" + lib)) {
                    libs_and_paths.push_back({lib, path});
                    found = true;
                    break;
                }
            }
            if (!found) {
                libs_and_paths.push_back({lib, "<not found>"});
            }
        }

        parse_needed_libs_and_paths(lib, paths, libs_and_paths);
    }
}

void read_etc_conf(std::list<std::string>& paths) {
    
}

void read_etc_conf_dir(std::list<std::string>& paths) {

}
