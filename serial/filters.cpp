#include "filters.hpp"

#include <algorithm>
#include <cmath>

#include "bmp.hpp"

static const float emboss_kernel[3][3] = {
    {1, 2, 1},
    {2, 4, 2},
    {1, 2, 1}};

static BMP::Pixel convolution(
    const BMP &pic,
    const float kernel[3][3],
    const int row,
    const int col)
{
    float red = 0, blue = 0, green = 0;

    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            int cur_row = row + i;
            int cur_col = col + j;
            const auto &temp = pic(cur_row, cur_col);
            red += (temp.R_) * kernel[i + 1][j + 1];
            blue += (temp.B_) * kernel[i + 1][j + 1];
            green += (temp.G_) * kernel[i + 1][j + 1];
        }
    }

    red /= 16;
    blue /= 16;
    green /= 16;

    BMP::Pixel result;
    result.R_ = (unsigned char)std::min(std::max<int>(red, 0), 255);
    result.B_ = (unsigned char)std::min(std::max<int>(blue, 0), 255);
    result.G_ = (unsigned char)std::min(std::max<int>(green, 0), 255);
    return result;
}

BMP blur_filter(const BMP &input_pic)
{
    BMP pic_cpy(input_pic);
    for (int i = 1; i < input_pic.get_height() - 1; ++i)
    {
        for (int j = 1; j < input_pic.get_width() - 1; ++j)
        {
            pic_cpy(i, j) =
                convolution(
                    input_pic,
                    emboss_kernel,
                    i,
                    j);
        }
    }
    return pic_cpy;
}

BMP purple_filter(const BMP &input_pic)
{
    BMP pic_cpy(input_pic);
    for (int i = 1; i < input_pic.get_height() - 1; ++i)
    {
        for (int j = 1; j < input_pic.get_width() - 1; ++j)
        {
            auto &pixel = pic_cpy(i, j);
            pixel.R_ = (unsigned char)std::min(
                std::max<int>((pixel.R_ * 0.5 + pixel.G_ * 0.3 + pixel.B_ * 0.5), 0),
                255);
            pixel.G_ = (unsigned char)std::min(
                std::max<int>((pixel.R_ * 0.16 + pixel.G_ * 0.5 + pixel.B_ * 0.16), 0),
                255);
            pixel.B_ = (unsigned char)std::min(
                std::max<int>((pixel.R_ * 0.6 + pixel.G_ * 0.2 + pixel.B_ * 0.8), 0),
                255);
        }
    }
    return pic_cpy;
}

BMP vertical_mirror_filter(BMP input_pic)
{
    for (int i = 0; i < input_pic.get_height(); ++i)
    {
        for (int j = 0; j < std::floor(input_pic.get_height() / 2); ++j)
        {
            BMP::Pixel temp = input_pic(j, i);
            input_pic(j, i) = input_pic(input_pic.get_height() - j - 1, i);
            input_pic(input_pic.get_height() - j - 1, i) = temp;
        }
    }
    return input_pic;
}

BMP draw_hashur(BMP input_pic)
{
    float slope = (float)input_pic.get_height() / (float)input_pic.get_width();
    for (int i = 0; i < input_pic.get_width(); ++i)
    {
        input_pic((input_pic.get_height() - 1) - i * slope, i) =
            WHITE;
    }

    for (int i = 0; i < input_pic.get_width() / 2; ++i)
    {
        input_pic(input_pic.get_height() / 2 - i * slope, i) =
            WHITE;
    }

    for (int i = 0; i < input_pic.get_width() / 2; ++i)
    {
        input_pic(input_pic.get_height() - 1 - i * slope, input_pic.get_width() / 2 + i) =
            WHITE;
    }

    return input_pic;
}
