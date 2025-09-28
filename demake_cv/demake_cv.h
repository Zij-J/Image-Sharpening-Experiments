#ifndef DEMAKE_CV_H
#define DEMAKE_CV_H

#include "stb_img.h"


namespace DemakeCV {

void image_histogram(int p_hist_buffer[256], const STB_Img &p_image, int p_channel_index);
void print_histogram(const int p_histogram[256], string p_title);
void print_histogram(const STB_Img &p_image, int p_channel_index, string p_title);
STB_Img histogram_eq(const STB_Img &p_image);

template <typename type>
STB_Img convolution_range_clamp(const STB_Img &p_image, type *p_kernel, int p_kernel_size);
template <typename type>
STB_Img convolution_range_scale(const STB_Img &p_image, type *p_kernel, int p_kernel_size);

}


#endif