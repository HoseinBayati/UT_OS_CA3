#ifndef FILTERS_HPP
#define FILTERS_HPP

#include "bmp.hpp"

struct SegProcArgs {
    SegProcArgs(BMP& bmp_, const BMP& original_bmp_, int col_begin_, int col_end_)
        : bmp(bmp_),
          original_bmp(original_bmp_),
          col_begin(col_begin_),
          col_end(col_end_) {}
    BMP& bmp;
    const BMP& original_bmp;
    int col_begin;
    int col_end;
};

BMP blur_filter(const BMP& input_pic);

BMP vertical_mirror_filter(BMP input_pic);

BMP purple_filter(BMP input_pic);

BMP draw_hashur(BMP input_pic);

#endif
