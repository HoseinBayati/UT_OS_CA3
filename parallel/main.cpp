#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>

#include "bmp.hpp"
#include "filters.hpp"

int main(int argc, char const *argv[])
{
    auto start_clk = std::chrono::high_resolution_clock::now();

    std::string file_name(argv[1]);
    std::ifstream input_file(file_name);

    BMP bmp;

    auto read_start_clk = std::chrono::high_resolution_clock::now();
    input_file >> bmp;
    input_file.close();
    auto read_end_clk = std::chrono::high_resolution_clock::now();

    auto mirror_start_clk = std::chrono::high_resolution_clock::now();
    bmp = vertical_mirror_filter(bmp);
    auto mirror_end_clk = std::chrono::high_resolution_clock::now();

    auto blur_start_clk = std::chrono::high_resolution_clock::now();
    bmp = blur_filter(bmp);
    auto blur_end_clk = std::chrono::high_resolution_clock::now();
   
    auto purple_start_clk = std::chrono::high_resolution_clock::now();
    bmp = purple_filter(bmp);
    auto purple_end_clk = std::chrono::high_resolution_clock::now();

    auto hashur_start_clk = std::chrono::high_resolution_clock::now();
    bmp = draw_hashur(bmp);
    auto hashur_end_clk = std::chrono::high_resolution_clock::now();

    std::ofstream output_file("result.bmp");
    output_file << bmp;
    output_file.close();

    auto end_clk = std::chrono::high_resolution_clock::now();

    auto total_time_diff = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(
        end_clk - start_clk);
    auto read_time_diff = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(
        read_end_clk - read_start_clk);
    auto flip_time_diff = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(
        blur_end_clk - blur_start_clk);
    auto blur_time_diff = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(
        mirror_end_clk - mirror_start_clk);
    auto purple_time_diff = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(
        purple_end_clk - purple_start_clk);
    auto lines_time_diff = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(
        hashur_end_clk - hashur_start_clk);

    std::cout << "Read: " << read_time_diff.count() << '\n';
    std::cout << "Flip: " << flip_time_diff.count() << '\n';
    std::cout << "Blur: " << blur_time_diff.count() << '\n';
    std::cout << "Purple: " << purple_time_diff.count() << '\n';
    std::cout << "Lines: " << lines_time_diff.count() << '\n';
    std::cout << "Execution: " << total_time_diff.count() << '\n';
}
