#pragma once

#include <optional>
#include <string>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// File

/// Read file contents to a string
auto read_file_to_string(const std::string& filename) -> std::optional<std::string>;

