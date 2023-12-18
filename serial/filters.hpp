#ifndef FILTERS_HPP
#define FILTERS_HPP

#include "bmp.hpp"

BMP blur_filter(const BMP& input_pic);

BMP purple_filter(const BMP& input_pic);

BMP vertical_mirror_filter(BMP input_pic);

BMP draw_hashur(BMP input_pic);

#endif
