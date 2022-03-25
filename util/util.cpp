#include "util.h"
#include "elf_file.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <string>
#include <map>
#include <utility>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <vector>


void parse_needed_libs_and_paths(
    const std::string& lib_name,
    const std::vector<std::string>& paths,
    std::map<std::string, std::string>& libs_and_paths
) {

    elf_file ef(lib_name);
    const std::vector<std::string>& ef_rpaths = ef.get_rpaths();
    for (auto& lib : ef.get_needed_libraries()) {
        if (libs_and_paths.find(lib) != libs_and_paths.end()) {
            continue;
        }
        
        bool found = false;
        for(auto& rpath : ef_rpaths) {
            if (std::filesystem::exists(rpath + "/" + lib)) {
                libs_and_paths[lib] = rpath;
                found = true;
                parse_needed_libs_and_paths(rpath + "/" + lib, paths, libs_and_paths);
            }
        }
        if (!found) {
            for (auto& path : paths) {
                if (std::filesystem::exists(path + "/" + lib)) {
                    libs_and_paths[lib] = path;
                    found = true;
                    parse_needed_libs_and_paths(path + "/" + lib, paths, libs_and_paths);
                    break;
                }
            }
        }
        if (!found) {
            libs_and_paths[lib] = "not found";
        }
    }
}

std::vector<std::string> read_ld_library_paths(char** envp) {
    std::vector<std::string> paths;
    for (char** env = envp; *env != 0; env++) {
        std::string s = *env;
        if (s.find(LD_LIB_PATH_VAR_NAME) != std::string::npos) {
            std::string ld_lib_path = std::string(*env).substr(LD_LIB_PATH_VAR_NAME.length() + 1); // +1 for symbol '=' because *env is LD_LIBRARY_PATH=.....
            boost::split(paths, ld_lib_path, boost::is_any_of(":"));
            return paths;
        }
    }

    return paths;
}

std::vector<std::string> read_etc_conf_dir() {
    std::vector<std::string> paths;
    std::string etc_conf_path = "/etc/ld.so.conf.d";
    for (auto& entry : std::filesystem::directory_iterator(etc_conf_path)) {
        std::ifstream file(entry.path());
        std::string path;
        while (file >> path) {
            paths.push_back(path);
        }
    }

    return paths;
}
