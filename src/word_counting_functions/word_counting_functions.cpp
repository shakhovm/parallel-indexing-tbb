#include "archive.h"
#include "archive_entry.h"
#include <fstream>
#include <sstream>
#include "../../include/word_counting_functions/word_counting_functions.h"
#include <boost/locale/boundary.hpp>
#include <boost/locale.hpp>

#define FILE_SIZE_BORDER 10000000

std::string directory_iteration(tbb::flow_control& fc,
                                std::filesystem::recursive_directory_iterator& directory,
                                const std::filesystem::recursive_directory_iterator& file_end) {
    if (file_end == directory)
    {
        fc.stop();
        return "";
    }

    auto entry = *directory++;

    std::string file_name_entry = entry.path().string();

    return file_name_entry;
}

std::string read_raw_file(const std::string& file_name) {
    std::ifstream raw_file(file_name, std::ios::binary);

    if (!raw_file.is_open()) return "";

    std::string buffer = dynamic_cast<std::ostringstream&>(
            std::ostringstream{} << raw_file.rdbuf()).str();
    
    return buffer;
}

void free_arch(archive* a) {
    archive_read_close(a);
    archive_read_free(a);
}

std::string return_thrash(archive* a) {
    free_arch(a);
    return "";
}

std::string archive_handler(const std::string& buffer) {

    int r;
    archive* a;
    a = archive_read_new();
    archive_entry* entry;
    r = archive_read_support_filter_all(a);
    if (r != ARCHIVE_OK) {
        return return_thrash(a);
    }

    r = archive_read_support_format_raw(a);
    if (r != ARCHIVE_OK) {
        return return_thrash(a);
    }

    r = archive_read_support_format_all(a);
    if (r != ARCHIVE_OK) {
        return return_thrash(a);
    }

    r = archive_read_open_memory(a, buffer.c_str(), buffer.size());

    if (r != ARCHIVE_OK) {
        return return_thrash(a);
    }

    size_t size;
    const void* buff;
    la_int64_t offset;


    r = archive_read_next_header(a, &entry);
    if (r != ARCHIVE_OK) {
        return return_thrash(a);
    }


    std::string txt_file_name = archive_entry_pathname(entry);

    if (txt_file_name == "data") {
        return buffer;
    }
    if (txt_file_name.size() < 4 ||
            txt_file_name.substr(txt_file_name.size() - 4, 4) != ".txt" ||
            archive_entry_size(entry) >= FILE_SIZE_BORDER) {
        return return_thrash(a);
    }

    std::string ss;
    while (true) {
        r = archive_read_data_block(a, &buff, &size, &offset);

        if (r == ARCHIVE_EOF) {
            break;
        }
        if (r != ARCHIVE_OK) {
            return return_thrash(a);
        }
        ss.append(std::string(static_cast<const char*>(buff), size));
        ss += " ";
    }
    free_arch(a);
    return ss;
}


word_map words_counter(const std::string& text) {
    if (text.empty()) {
        return word_map();
    }

    word_map wm;
    boost::locale::boundary::ssegment_index map(boost::locale::boundary::word,
                text.begin(), text.end());

    map.rule(boost::locale::boundary::word_letters);
    for (const auto& x: map) {
        wm[boost::locale::fold_case(boost::locale::normalize(std::string(x)))]++;
    }

    return wm;
}
