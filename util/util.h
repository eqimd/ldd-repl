#pragma once

#include <string>
#include <list>
#include <map>
#include <vector>

using Filename = std::string;
using Path = std::string;


const std::string LD_LIB_PATH_VAR_NAME = "LD_LIBRARY_PATH";

std::vector<Path> read_ld_library_paths(char** envp);

void parse_needed_libs_and_paths(
    const Filename& lib_name,
    const std::vector<Path>& paths,
    std::map<Filename, Path>& libs_and_paths
);

std::vector<Path> read_etc_conf_dir();