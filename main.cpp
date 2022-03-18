#include "util.h"

#include <iostream>
#include <string>
#include <map>


int main(int argc, char** argv, char** envp) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <binary>\n";
        return 0;
    }
    std::string fn(argv[1]);

    std::list<std::string> paths = {"/lib", "/usr/lib"};
    std::string ld_lib_path = std::move(read_ld_library_path(envp));
    if (ld_lib_path != "") {
        paths.push_front(ld_lib_path);
    }

    std::list<std::string> etc_conf_dir_paths = std::move(read_etc_conf_dir());
    paths.insert(paths.end(), etc_conf_dir_paths.begin(), etc_conf_dir_paths.end());

    std::map<std::string, std::string> libs_and_paths;
    parse_needed_libs_and_paths(fn, paths, libs_and_paths);

    for (auto& it : libs_and_paths) {
        std::cout << "\t" << it.first << " => " << it.second << "\n";
    }
}
