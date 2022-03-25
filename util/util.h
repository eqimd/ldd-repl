#pragma once

#include <string>
#include <list>
#include <map>
#include <vector>


const std::string LD_LIB_PATH_VAR_NAME = "LD_LIBRARY_PATH";

std::vector<std::string> read_ld_library_paths(char** envp);

void parse_needed_libs_and_paths(
    const std::string& lib_name,
    const std::vector<std::string>& paths,
    std::map<std::string, std::string>& libs_and_paths
);

std::vector<std::string> read_etc_conf_dir();