#pragma once

#include <iostream>
#include <vector>
#include <unordered_set>


namespace sp {

    class Sandpile {
    private:
        enum KeyWordId {
            LengthID = 1, WidthID = 3, InputFileID = 5, OutputFileID = 7, MaxIterID = 9, FreqID = 11, ExtendID = 13
        };

        struct hash_pair {
            inline std::size_t operator()(const std::pair<uint64_t, uint64_t>& p) const {
                return (std::hash<uint64_t>()(p.first * 31) + std::hash<uint64_t>()(p.second));
            }
        };

        uint16_t length_;
        uint16_t width_;
        uint64_t iteration_limit_;
        uint64_t save_frequency_;

        std::vector<std::vector<uint64_t>> grid_;
        std::unordered_set<std::pair<uint64_t, uint64_t>, hash_pair> critical_points_;

        Sandpile(uint16_t length, uint16_t width, uint64_t limit, uint64_t freq);
        void read_input_file_table(const std::string& input_file);
        void read_input_file_coords(const std::string& input_file);
        bool new_iteration();
        bool new_iteration_extend();
        bool new_iteration_set();
        bool new_iteration_extend_set();

        void begin_iterations(const std::string& save_path, bool extend);
        void print(std::ostream& os = std::cout) const;

        void save(const std::string& path, uint32_t id) const;

        friend void start();
    };

    void start();
}
