
#include "Sandpile.h"
#include "BMPhandler.h"

#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <unordered_set>
#include <random>

namespace sp {
    const std::regex kCommand("\\s*(-l|--length)\\s+\\d+\\s+(-w|--width)\\s+\\d+\\s+(-i|--input)\\s+\\w+(\\.tsv)?\\s+(-o|--output)\\s+([A-Z]:|\\.\\.(\\\\|\\/))(.+)(\\\\|\\/)(([^(\\\\|\\/)]+)(\\/|\\\\))*\\s+(-m|--max-iter)\\s+\\d+\\s+(-f|--freq)\\s+\\d+\\s+(-e|--extend)\\s+[1|0]\\s*");
    const std::regex kSplit("(\\S+)");

    sp::Sandpile::Sandpile(uint16_t length, uint16_t width, uint64_t limit, uint64_t freq) :
       length_(length != 0 ? length : 1), width_(width != 0 ? width : 1),
       iteration_limit_(limit), save_frequency_(freq) {
        grid_.assign(width_, std::vector<uint64_t>(length_, 0));
    }

    void sp::Sandpile::read_input_file_table(const std::string& input_file) {
        std::ifstream input("../input/" + input_file);
        if (!input.is_open()) {
            input.open("../input/" + input_file + ".tsv");
            if (!input.is_open())
                throw std::runtime_error{"Input file does not exist"};
        }
        std::string line;
        uint16_t row_count = 0;
        uint16_t col_count = 0;
        while (std::getline(input, line, '\n')) {
            ++row_count;
            if (row_count > width_)
                throw std::runtime_error{"Incorrect input file: too many rows"};
            std::istringstream iss(line);
            col_count = 0;
            for (auto it = std::istream_iterator<std::string>(iss); it != std::istream_iterator<std::string>(); ++it) {
                ++col_count;
                if (col_count > length_)
                    throw std::runtime_error{"Incorrect input file: too many columns"};
                grid_[row_count - 1][col_count - 1] = std::stoull(*it);
            }
            if (col_count < length_)
                throw std::runtime_error{"Incorrect input file: not enough columns"};
        }
        if (row_count < width_)
            throw std::runtime_error{"Incorrect input file: not enough rows"};

        input.close();
    }

    void sp::Sandpile::read_input_file_coords(const std::string& input_file) {
        std::ifstream input("../input/" + input_file);
        if (!input.is_open()) {
            input.open("../input/" + input_file + ".tsv");
            if (!input.is_open())
                throw std::runtime_error{"Input file does not exist"};
        }
        std::string line;
        uint16_t x_coord;
        uint16_t y_coord;
        while (std::getline(input, line, '\n')) {
            std::istringstream iss(line);
            iss >> x_coord >> y_coord;
            if (x_coord > width_ || y_coord > length_)
                throw std::runtime_error{"Input file incorrect: index out of range"};
            iss >> grid_[x_coord - 1][y_coord - 1];
            if (grid_[x_coord - 1][y_coord - 1] > 3)
                critical_points_.emplace(x_coord - 1, y_coord - 1);
        }

        input.close();
    }

    void sp::Sandpile::print(std::ostream& os) const {
        for (auto& row : grid_) {
            for (uint32_t grain : row)
                os << grain << ' ';
            os << std::endl;
        }
        os << "===================\n";
    }

    bool sp::Sandpile::new_iteration() {
        for (uint16_t i = 0; i < length_; ++i) {
            for (uint16_t j = 0; j < width_; ++j) {
                if (grid_[i][j] >= 4) {
                    grid_[i][j] -= 4;
                    if (i != 0)
                        grid_[i - 1][j]++;
                    if (i != length_ - 1)
                        grid_[i + 1][j]++;
                    if (j != 0)
                        grid_[i][j - 1]++;
                    if (j != width_ - 1)
                        grid_[i][j + 1]++;
                    return true;
                }
            }
        }

        return false;
    }

    bool sp::Sandpile::new_iteration_set() {
        if (critical_points_.empty())
            return false;
        auto p = critical_points_.begin();
        uint64_t i = p->first;
        uint64_t j = p->second;
        grid_[i][j] -= 4;
        if (grid_[i][j] < 4)
            critical_points_.erase(p);
        if (i != 0) {
            if (++grid_[i - 1][j] > 3) critical_points_.emplace(i - 1, j);
        }
        if (i != width_ - 1) {
            if (++grid_[i + 1][j] > 3) critical_points_.emplace(i + 1, j);
        }
        if (j != 0) {
            if (++grid_[i][j - 1] > 3 ) critical_points_.emplace(i, j - 1);
        }
        if (j != length_ - 1) {
            if (++grid_[i][j + 1] > 3) critical_points_.emplace(i, j + 1);
        }

        return true;
    }

    bool sp::Sandpile::new_iteration_extend() {
        for (uint16_t i = 0; i < width_; ++i) {
            for (uint16_t j = 0; j < length_; ++j) {
                if (grid_[i][j] >= 4) {
                    grid_[i][j] -= 4;
                    if (i == 0) {
                        grid_.insert(grid_.begin(), std::vector<uint64_t>(length_, 0));
                        width_++;
                        i++;
                    } else if (i == width_ - 1) {
                        grid_.insert(grid_.end(), std::vector<uint64_t>(length_, 0));
                    }
                    if (j == 0) {
                        for (int k = 0; k < width_; grid_[k].insert(grid_[k].begin(), 0), ++k);
                        length_++;
                        j++;
                    } else if (j == length_ - 1) {
                        for (int k = 0; k < width_; grid_[k].insert(grid_[k].end(), 0), ++k);
                        length_++;
                    }
                    ++grid_[i + 1][j];
                    ++grid_[i - 1][j];
                    ++grid_[i][j + 1];
                    ++grid_[i][j - 1];

                    return true;
                }
            }
        }

        return false;
    }

    bool sp::Sandpile::new_iteration_extend_set() {
        if (critical_points_.empty())
            return false;
        auto p = critical_points_.begin();
        std::advance(p, std::rand() % critical_points_.size());
        uint64_t i = p->first;
        uint64_t j = p->second;
        grid_[i][j] -= 4;
        if (grid_[i][j] < 4) critical_points_.erase(critical_points_.find(std::make_pair(i, j)));
        std::cout << "Coords: " << i << ' ' << j << std::endl;
        if (i == 0) {
            grid_.insert(grid_.begin(), std::vector<uint64_t>(length_, 0));
            width_++;
            i = 1;
        } else if (i == width_ - 1) {
            grid_.insert(grid_.end(), std::vector<uint64_t>(length_, 0));
        }
        if (j == 0) {
            for (int k = 0; k < width_; grid_[k].insert(grid_[k].begin(), 0), std::cout << k << ' ', ++k);
            length_++;
            j = 1;
        } else if (j == length_ - 1) {
            for (int k = 0; k < width_; grid_[k].insert(grid_[k].end(), 0), std::cout << k << ' ', ++k);
            length_++;
        }
        if (++grid_[i - 1][j] > 3) critical_points_.emplace(i - 1, j);
        if (++grid_[i + 1][j] > 3) critical_points_.emplace(i + 1, j);
        if (++grid_[i][j - 1] > 3) critical_points_.emplace(i, j - 1);
        if (++grid_[i][j + 1] > 3) critical_points_.emplace(i, j + 1);

        return true;
    }

    void sp::Sandpile::save(const std::string& path, uint32_t id) const {
        std::ofstream output(path + "state" + std::to_string(id) + ".bmp", std::ios::binary);
//        if (!output.is_open())
//            std::cout << "not open\n" << path + "state" + std::to_string(id) + ".bmp" << std::endl;
        BMPhandler handler;
        handler.fill_header_info(output, length_, width_);
        handler.fill_pixels(output, length_, width_, grid_);
        output.close();
    }

    void sp::Sandpile::begin_iterations(const std::string& save_path, bool extend = false) {
        uint64_t iteration_counter = 0;
        uint64_t save_counter = 0;
        if (save_frequency_ == 0)
            save_frequency_ = -1;
        uint64_t save_id = 0;
        while ((iteration_counter < iteration_limit_ || iteration_limit_ == 0) && (extend ? new_iteration_extend() : new_iteration())) {
            ++iteration_counter;
            ++save_counter;
            if (save_counter == save_frequency_) {
                save(save_path, save_id);
                save_counter = 0;
                ++save_id;
            }
        }
        save(save_path, save_id);
    }


    void start() {
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line == "stop")
                break;
            if (std::regex_match(line, kCommand)) {
                std::vector<std::string> tokens{std::sregex_token_iterator(std::cbegin(line), std::cend(line), kSplit, 1),
                                                std::sregex_token_iterator()};

                std::unique_ptr<sp::Sandpile> sandpile(new sp::Sandpile(std::stoull(tokens[sp::Sandpile::KeyWordId::LengthID]),
                                                                        std::stoull(tokens[sp::Sandpile::KeyWordId::WidthID]),
                                                                        std::stoull(tokens[sp::Sandpile::KeyWordId::MaxIterID]),
                                                                        std::stoull(tokens[sp::Sandpile::KeyWordId::FreqID])));
                try {
                    sandpile->read_input_file_coords(tokens[sp::Sandpile::KeyWordId::InputFileID]);
                } catch (const std::exception& e) {
                    std::cout << e.what() << std::endl;
                    continue;
                }
                if (tokens[sp::Sandpile::KeyWordId::ExtendID] == "1")
                    sandpile->begin_iterations(tokens[sp::Sandpile::KeyWordId::OutputFileID], true);
                else
                    sandpile->begin_iterations(tokens[sp::Sandpile::KeyWordId::OutputFileID]);
                std::cout << "Completed!" << std::endl;
            } else {
                std::cout << "Wrong syntax" << std::endl;
            }
        }
    }
}