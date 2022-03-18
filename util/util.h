#pragma once

#include <string>
#include <list>
#include <map>

const std::string LD_LIB_PATH_VAR_NAME = "LD_LIBRARY_PATH";

std::string read_ld_library_path(char** envp);

void parse_needed_libs_and_paths(
    const std::string& lib_name,
    const std::list<std::string>& paths,
    std::map<std::string, std::string>& libs_and_paths
);

std::list<std::string> read_etc_conf_dir();