#include "util.h"

#include <iostream>
#include <string>
#include <map>
#include <vector>


int main(int argc, char** argv, char** envp) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <binary>\n";
        return 0;
    }
    std::string fn(argv[1]);

    std::vector<std::string> paths;
    // std::string ld_lib_path = read_ld_library_path(envp);
    // if (ld_lib_path != "") {
    //     paths.push_back(ld_lib_path);
    // }
    paths.push_back("/lib");
    paths.push_back("/usr/lib");

    std::vector<std::string> etc_conf_dir_paths = read_etc_conf_dir();
    paths.insert(paths.end(), etc_conf_dir_paths.begin(), etc_conf_dir_paths.end());

    std::map<std::string, std::string> libs_and_paths;
    parse_needed_libs_and_paths(fn, paths, libs_and_paths);

    for (auto& it : libs_and_paths) {
        std::cout << "\t" << it.first << " => " << it.second << "\n";
    }

    return 0;
}
