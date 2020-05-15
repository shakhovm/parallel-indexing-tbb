#include <iostream>
#include "tbb/parallel_reduce.h"
#include "tbb/pipeline.h"
#include "archive.h"
#include "archive_entry.h"
#include <fstream>
#include <filesystem>
#include <boost/locale.hpp>
#include <map>
#include <boost/locale/boundary.hpp>
typedef std::map<std::string, size_t> word_map;
using namespace tbb;
int main() {
    boost::locale::generator gen;
    std::locale loc = gen("en_US.UTF-8");
    std::locale::global(loc);
    std::string directory_path = "../files";
    auto directory = std::filesystem::recursive_directory_iterator(directory_path);
    auto file_end = std::filesystem::end(directory);
    parallel_pipeline(10, make_filter<void, std::string>(
                          filter::serial,
                          [&](flow_control& fc)->std::string{

                          //    size_t i = 0;

                                  if (file_end == directory)
                                  {
                                      fc.stop();
                                      return "stop";
                                  }
//                                  std::cout << (directory == file_end) << std::endl;
                                  auto entry = *directory++;

                                  std::string file_name_entry = entry.path().string();

                                  return file_name_entry;
                              }
                      ) &
                      make_filter<std::string, std::string>(
                                                filter::parallel,
                                                [](const std::string& file_name){
        std::ifstream raw_file(file_name, std::ios::binary);

//        if (!raw_file.is_open()) continue;

        std::string buffer = dynamic_cast<std::ostringstream&>(
                std::ostringstream{} << raw_file.rdbuf()).str();


//        if (raw_data.empty()) {
//            continue;
//        }

        int r;
        archive* a;
        a = archive_read_new();
        archive_entry* entry;
        r = archive_read_support_filter_all(a);
//        if (r != ARCHIVE_OK) {
//            archive_read_close(a);
//            archive_read_free(a);
//            return;
//        }

        r = archive_read_support_format_raw(a);
//        if (r != ARCHIVE_OK) {
//            archive_read_close(a);
//            archive_read_free(a);
//            return;
//        }

        r = archive_read_support_format_all(a);
//        if (r != ARCHIVE_OK) {
//            archive_read_close(a);
//            archive_read_free(a);
//            return;
//        }

        r = archive_read_open_memory(a, buffer.c_str(), buffer.size());

//        if (r != ARCHIVE_OK) {

//            archive_read_close(a);
//            archive_read_free(a);
//            return;
//        }

        size_t size;
        const void* buff;
        la_int64_t offset;


        r = archive_read_next_header(a, &entry);



        std::string txt_file_name = archive_entry_pathname(entry);




        std::string ss;
        while (true) {
            r = archive_read_data_block(a, &buff, &size, &offset);

            if (r == ARCHIVE_EOF) {
                break;
            }
//            if (r != ARCHIVE_OK) {
//                archive_read_close(a);
//                archive_read_free(a);
//                return;
//            }
            ss.append(std::string(static_cast<const char*>(buff), size));
            ss += " ";
        }
        archive_read_close(a);
        archive_read_free(a);
        return ss;

        }) &
        make_filter<std::string, word_map>(
                                  filter::parallel,
            [](const std::string& text) {

//    if (text.empty()) {
//        break;
//    }

    word_map wm;
    boost::locale::boundary::ssegment_index map(boost::locale::boundary::word,
                text.begin(), text.end());

    map.rule(boost::locale::boundary::word_letters);
    for (const auto& x: map) {
        wm[boost::locale::fold_case(boost::locale::normalize(std::string(x)))]++;
    }
    return wm;

            }) & make_filter<word_map, void>(filter::serial, [](const word_map& wm){
                for (auto& x: wm) {
                    std::cout << x.first << " : " << x.second << std::endl;
                }
            }));
    std::cout << "End of the Program!" << std::endl;

    return 0;
}
