#ifndef ARCHIVE_HANDLER_H
#define ARCHIVE_HANDLER_H
#include <string>
#include "archive.h"
#include "tbb/concurrent_unordered_map.h"
#include "tbb/pipeline.h"
#include <filesystem>
#include <unordered_map>
typedef std::unordered_map<std::string, size_t> word_map;


std::string directory_iteration(tbb::flow_control& fc,
                                std::filesystem::recursive_directory_iterator& directory,
                                const std::filesystem::recursive_directory_iterator& file_end);

std::string read_raw_file(const std::string& file_name);

std::string archive_handler(const std::string& file_name);

word_map words_counter(const std::string& text);

#endif // ARCHIVE_HANDLER_H
