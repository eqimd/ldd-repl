#pragma once

#include <string>
#include <list>

const std::string LD_LIB_PATH_VAR_NAME = "LD_LIBRARY_PATH";

std::string read_ld_library_path(char** envp);
void parse_needed_libs_and_paths(
    std::string& libname,
    const std::list<std::string>& paths,
    std::list<std::pair<std::string, std::string>>& libs_and_paths
);