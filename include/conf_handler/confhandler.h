#ifndef CONFHANDLER_H
#define CONFHANDLER_H

#include <fstream>
class ConfHandler {
    std::string content;
    struct ConfParams {
        std::string input_directory;
        std::string out_by_a;
        std::string out_by_n;
        size_t indexing_threads;
        size_t merging_threads;
    } conf_params;
public:
    ConfHandler() = default;
    explicit ConfHandler(const std::string& filename);
    ~ConfHandler() = default;
    std::string file_pattern(const std::string& pattern);
    void conf_file_handler();
    inline ConfParams& getConfParams() { return conf_params; }
};
#endif // CONFHANDLER_H
