#include "util.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <iostream>
#include <string>
#include <map>
#include <vector>


int main(int argc, char** argv, char** envp) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <binary>\n";
        return 0;
    }
    Filename fn(argv[1]);

    std::vector<Path> paths;
    std::vector<Path> ld_lib_paths = read_ld_library_paths(envp);
    for (auto& path : ld_lib_paths) {
        paths.push_back(path);
    }
    paths.push_back("/lib");
    paths.push_back("/usr/lib");

    std::vector<Path> etc_conf_dir_paths = read_etc_conf_dir();
    paths.insert(paths.end(), etc_conf_dir_paths.begin(), etc_conf_dir_paths.end());

    std::map<Filename, Path> libs_and_paths;
    parse_needed_libs_and_paths(fn, paths, libs_and_paths);

    for (auto& it : libs_and_paths) {
        std::cout << "\t" << it.first << " => " << it.second << "\n";
    }

    return 0;
}
