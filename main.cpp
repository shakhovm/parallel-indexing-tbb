#include <iostream>
#include "tbb/parallel_reduce.h"
#include "tbb/pipeline.h"
#include <filesystem>
#include <boost/locale.hpp>
#include <unordered_map>
#include <chrono>
#include "tbb/concurrent_unordered_map.h"
#include "include/word_counting_functions/word_counting_functions.h"
#include "include/conf_handler/confhandler.h"
#define MAX_SIZE 1000

using namespace tbb;
typedef concurrent_unordered_map<std::string, size_t> main_map_t;

void map_write(const main_map_t& main_map, const std::string& out_by_n, const std::string& out_by_a) {
    std::vector<std::pair<std::string, size_t>> v(main_map.begin(), main_map.end());
    std::sort(v.begin(), v.end(),
              [](const std::pair<std::string, size_t>& a,
                 const std::pair<std::string, size_t>& b){return a.second > b.second; });
    std::ofstream file_out(out_by_n);

    for (const auto& x: v) {
        file_out << x.first << " : " << x.second << std::endl;
    }
    std::ofstream file_out_by_a(out_by_a);
    std::sort(v.begin(), v.end(),
              [](const std::pair<std::string, size_t>& a,
                 const std::pair<std::string, size_t>& b){return a.first < b.first; });
    for (const auto& x: v) {
        file_out_by_a << x.first << " : " << x.second << std::endl;
    }

}

int main(int argc, char* argv[]) {
    std::string conf_file_name;
    if (argc > 1) {
        conf_file_name = argv[1];
    } else {
        conf_file_name = "../conf.dat";
    }

    ConfHandler conf_handler;
    try {
        conf_handler = ConfHandler(conf_file_name);
    } catch (std::runtime_error& e ) {
        std::cout << "Cannot Handle Configuration file!" << std::endl;
        return 1;
    }
    auto params = conf_handler.getConfParams();
    boost::locale::generator gen;
    std::locale loc = gen("en_US.UTF-8");
    std::locale::global(loc);
    std::filesystem::recursive_directory_iterator  directory;
    try {
        directory = std::filesystem::recursive_directory_iterator(params.input_directory);
    }

    catch (std::runtime_error& e) {
        std::cout << "The Directoty Doesn't exist" << std::endl;
        return -1;
    }

    auto file_end = std::filesystem::end(directory);

    auto start = std::chrono::high_resolution_clock::now();
    main_map_t main_map;
    parallel_pipeline(MAX_SIZE, make_filter<void, std::string>(
                          filter::serial_out_of_order,
                          [&directory, &file_end](flow_control& fc)->std::string{
                              return directory_iteration(fc, directory, file_end);}) &
        make_filter<std::string, std::string>(filter::serial_out_of_order,
                [](const std::string& file){return read_raw_file(file);}) &
        make_filter<std::string, std::string>(filter::parallel,
                [](const std::string& raw_file){return archive_handler(raw_file);}) &
        make_filter<std::string, word_map>(filter::parallel,
                [](const std::string& text){return words_counter(text); }) &
        make_filter<word_map, void>(filter::serial_out_of_order,
                                      [&main_map](const word_map& wm){
            for (const auto& x: wm) {
                main_map[x.first] += x.second;
            }
    }));

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Indexing, reading, searching(seconds) :" <<
        static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()) / 1000
              << std::endl;
    map_write(main_map, params.out_by_n, params.out_by_a);
    return 0;
}
