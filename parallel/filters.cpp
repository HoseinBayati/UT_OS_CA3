#include "filters.hpp"

#include <algorithm>
#include <cmath>

#include "bmp.hpp"

static const float blur_kernel[3][3] = {
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

void *partial_convolution(void *args)
{
    SegProcArgs *args_struct =
        static_cast<SegProcArgs *>(args);

    auto &row_begin = args_struct->col_begin;
    auto &row_end = args_struct->col_end;
    auto &bmp = args_struct->bmp;
    auto &original_bmp = args_struct->original_bmp;

    for (int i = row_begin; i < row_end; ++i)
    {
        if (i == original_bmp.get_height() - 1 || i == 0)
            continue;
        for (int j = 1; j < bmp.get_width() - 1; ++j)
        {
            bmp(i, j) =
                convolution(
                    original_bmp,
                    blur_kernel,
                    i,
                    j);
        }
    }
    return NULL;
}

BMP blur_filter(const BMP &input_pic)
{
    BMP pic_cpy(input_pic);

    auto segment_process = [&pic_cpy, &input_pic](int row_begin, int row_end)
    {
        SegProcArgs *args = new SegProcArgs(
            pic_cpy,
            input_pic,
            row_begin,
            row_end);
        pthread_t t_id;
        pthread_create(&t_id, NULL, partial_convolution, (void *)args);
        return t_id;
    };

    std::vector<pthread_t> t_ids;
    int N_PARTS = 8;
    int height = input_pic.get_height() - 1;
    for (int i = 0; i < N_PARTS; ++i)
    {
        int row_begin = i * (height / N_PARTS);
        int row_end = (i + 1) * (height / N_PARTS);
        t_ids.push_back(segment_process(row_begin, row_end));
    }

    if (height % N_PARTS)
    {
        int row_begin = (height - (height % N_PARTS));
        int row_end = height;
        t_ids.push_back(segment_process(row_begin, row_end));
    }

    for (auto t_id : t_ids)
        pthread_join(t_id, NULL);

    return pic_cpy;
}

void *partial_purple_filter(void *args)
{
    SegProcArgs *args_struct =
        static_cast<SegProcArgs *>(args);

    auto &row_begin = args_struct->col_begin;
    auto &row_end = args_struct->col_end;
    auto &bmp = args_struct->bmp;
    auto &original_bmp = args_struct->original_bmp;

    for (int i = row_begin; i < row_end; ++i)
    {
        for (int j = 0; j < bmp.get_width(); ++j)
        {
            auto &pixel = bmp(i, j);
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

    return NULL;
}

BMP purple_filter(BMP input_pic)
{
    BMP pic_cpy(input_pic);

    auto segment_process = [&pic_cpy, &input_pic](int row_begin, int row_end)
    {
        SegProcArgs *args = new SegProcArgs(
            pic_cpy,
            input_pic,
            row_begin,
            row_end);
        pthread_t t_id;
        pthread_create(&t_id, NULL, partial_purple_filter, (void *)args);
        return t_id;
    };

    std::vector<pthread_t> t_ids;
    int N_PARTS = 8;
    int height = input_pic.get_height() - 1;
    for (int i = 0; i < N_PARTS; ++i)
    {
        int row_begin = i * (height / N_PARTS);
        int row_end = (i + 1) * (height / N_PARTS);
        t_ids.push_back(segment_process(row_begin, row_end));
    }

    if (height % N_PARTS)
    {
        int row_begin = (height - (height % N_PARTS));
        int row_end = height;
        t_ids.push_back(segment_process(row_begin, row_end));
    }

    for (auto t_id : t_ids)
        pthread_join(t_id, NULL);

    return pic_cpy;
}

void *partial_vertical_mirror(void *args)
{
    SegProcArgs *args_struct =
        static_cast<SegProcArgs *>(args);

    auto &col_begin = args_struct->col_begin;
    auto &col_end = args_struct->col_end;
    auto &bmp = args_struct->bmp;
    auto &original_bmp = args_struct->original_bmp;

    for (int i = col_begin; i < col_end; ++i)
    {
        for (int j = 0; j < std::floor(bmp.get_height() / 2); ++j)
        {
            BMP::Pixel temp = bmp(j, i);
            bmp(j, i) = bmp(bmp.get_height() - j - 1, i);
            bmp(bmp.get_height() - j - 1, i) = temp;
        }
    }
    return NULL;
}

BMP vertical_mirror_filter(BMP input_pic)
{
    auto segment_process = [&input_pic](int col_begin, int col_end)
    {
        SegProcArgs *args = new SegProcArgs(
            input_pic,
            input_pic,
            col_begin,
            col_end);
        pthread_t t_id;
        pthread_create(&t_id, NULL, partial_vertical_mirror, (void *)args);
        return t_id;
    };

    std::vector<pthread_t> t_ids;
    int N_PARTS = 8;
    int width = input_pic.get_width();
    for (int i = 0; i < N_PARTS; ++i)
    {
        int col_begin = i * (width / N_PARTS);
        int col_end = (i + 1) * (width / N_PARTS);
        t_ids.push_back(segment_process(col_begin, col_end));
    }

    if (width % N_PARTS)
    {
        int col_begin = (width - (width % N_PARTS));
        int col_end = width;
        t_ids.push_back(segment_process(col_begin, col_end));
    }

    for (auto t_id : t_ids)
        pthread_join(t_id, NULL);

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
