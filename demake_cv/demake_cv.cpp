#include "demake_cv.h"
#include "../utilities/error_handler.h"
#include "../utilities/code_tester.h"
#include <iostream>
#include <cmath>
#include <limits>

// accumlate histogram result to `hist_buffer`
void DemakeCV::image_histogram(int p_hist_buffer[256], const STB_Img &p_image, int p_channel_index) {

    ErrorHandler_FATAL_IF(p_channel_index >= p_image.get_channel_num(), __FILE__, __LINE__);

    stbi_uc *img_ptr =  p_image.get_image() + p_channel_index;
    for (int i = 0; i < p_image.get_pixel_num(); ++i) {
        ++p_hist_buffer[*(img_ptr + i * p_image.get_channel_num())];
    }
}

void DemakeCV::print_histogram(const int p_histogram[256], string p_title) {
    // calculate pre-request
    string str_hist[256];
    int max_ten = 10;
    for (int i = 0; i < 256; ++i) {
        if (p_histogram[i] > 0) { // don't show "0" terns
            str_hist[i] = to_string(p_histogram[i]);
        }
        while (p_histogram[i] >= max_ten) {
            max_ten *= 10;
        }
    }
    unsigned int hist_height = 0;
    while ((max_ten /= 10) != 0) {
        ++hist_height;
    }

    // show
    cout << p_title << ":\n";
    for (unsigned int h = hist_height - 1; ; --h) {
        for (int i = 0; i < 256; ++i) {
            if (str_hist[i].length() <= h) {
                cout <<  ' ';
            } else {
                cout << str_hist[i][str_hist[i].length() - 1 - h]; // store big number first
            }
        }
        cout << '\n';
        if (h == 0) {
            break;
        }
    }
    for (int i = 0; i < 256; ++i) {
        cout << "-";
    }
    cout << '\n';
}

void DemakeCV::print_histogram(const STB_Img &p_image, int p_channel_index, string p_title) {
    int histogram[256] = { 0 }; // all init 0
    DemakeCV::image_histogram(histogram, p_image, p_channel_index);
    DemakeCV::print_histogram(histogram, p_title);
}


STB_Img DemakeCV::histogram_eq(const STB_Img &p_image) {

    STB_Img result(p_image);
    for (int channel = 0; channel < p_image.get_channel_num(); ++channel) {

        int histogram[256] = { 0 }; // all init 0
        DemakeCV::image_histogram(histogram, p_image, channel); 

        float cdf_histogram[256] = { 0.0f }; // all init 0
        float total_pixel_num = static_cast<float>(p_image.get_pixel_num());
        cdf_histogram[0] = static_cast<float>(histogram[0] * 255) / total_pixel_num;
        for (int i = 1; i < 256; ++i) {
            cdf_histogram[i] = cdf_histogram[i-1] + static_cast<float>(histogram[i] * 255) / total_pixel_num;
        }

        stbi_uc *result_ptr = result.get_image() + channel;
        for (int i = 0; i < result.get_pixel_num(); ++i) {
            stbi_uc *now_ptr = result_ptr + i * result.get_channel_num(); 
            *(now_ptr) = static_cast<stbi_uc>(roundf(cdf_histogram[*now_ptr]));
            // *(now_ptr) = static_cast<stbi_uc>(histogram[ static_cast<int>(roundf(cdf_histogram[*now_ptr])) ]);
        }

        // Algorithm:
        // 1. calculate p(i) = hist[i]  / total_pixel_num * 255; (range: [0,255])
        // 2. accumlate p(i) to c(i) = p(0)+p(1)+...+p(i)  (range: [0,255])
        //      origin gray level i -> new gray level c(i) 
        // 3. replace image gray level i pixel -> gray level c(i)
    }

    return result; 
}


// create result_image in `type` array (size: p_image.get_pixel_num() * p_image.get_channel_num())
template <typename type>
static type *inner_convolution(const STB_Img &p_image, type *p_kernel, int p_kernel_size) {
    // type conversion (reduce redunent convert in convolution)
    stbi_uc *img_ptr = p_image.get_image(); 
    type *int_image = new type [p_image.get_pixel_num() * p_image.get_channel_num()];
    for (int i = 0; i < p_image.get_pixel_num() * p_image.get_channel_num(); ++i) {
        int_image[i] = static_cast<type>(img_ptr[i]);
    }
    type *int_result = new type [p_image.get_pixel_num() * p_image.get_channel_num()];

    // for every channels
    for (int channel = 0; channel < p_image.get_channel_num(); ++channel) {

        // for every pixel
        int in_width = 0;
        int in_height = 0;
        int kernel_bound = p_kernel_size / 2;
        type *in_ptr = int_image + channel;
        type *out_bound = int_result + p_image.get_pixel_num() * p_image.get_channel_num(); 
        for (type *out_ptr = int_result + channel; out_ptr < out_bound; out_ptr += p_image.get_channel_num()) {

            // one n*n convolution (zero padding)
            type sum = 0; 
            type *now_kernel = p_kernel - p_kernel_size - 1; // add back first substract
            type *now_in_ptr = in_ptr - (kernel_bound+1) * p_image.get_width() * p_image.get_channel_num() - (kernel_bound+1) * p_image.get_channel_num(); 
            for (int conv_height = -kernel_bound; conv_height <= kernel_bound; ++conv_height) {
                // next row
                int now_in_height = in_height + conv_height;
                now_kernel += p_kernel_size;
                // zero padding
                if (now_in_height >= p_image.get_height()) {
                    break;
                }
                now_in_ptr += p_image.get_width() * p_image.get_channel_num();
                if (now_in_height < 0) {
                    continue;
                }
                // if (now_in_height_index < 0) { // reflection padding
                //     now_in_height_index = -now_in_height_index;
                // }
                // if (now_in_height_index > feature_height_as_index) { // reflection padding
                //     now_in_height_index = feature_height_as_index - (now_in_height_index - feature_height_as_index);
                // }

                type *inner_now_kernel = now_kernel;
                type *inner_now_in_ptr = now_in_ptr;
                for (int conv_width = -kernel_bound; conv_width <= kernel_bound; ++conv_width) {
                    // next column
                    int now_in_width = in_width + conv_width;
                    inner_now_kernel += 1;
                    // zero padding
                    if (now_in_width >= p_image.get_width()) {
                        continue;
                    }
                    inner_now_in_ptr += p_image.get_channel_num();
                    if (now_in_width < 0) { 
                        continue;
                    }
                    // if (now_in_width_index < 0) { // reflection padding
                    //     now_in_width_index = -now_in_width_index;
                    // }
                    // if (now_in_width_index > feature_width_as_index) { // reflection padding
                    //     now_in_width_index = feature_width_as_index - (now_in_width_index - feature_width_as_index);
                    // }
                    
                    sum += *inner_now_in_ptr * *inner_now_kernel;
                }
            }
            *out_ptr = sum;

            // update width & height index
            in_width += 1;
            if (in_width == p_image.get_width())  {   // can't go left, next row
                in_width = 0;
                in_height += 1;
                // if (in_height == p_image.get_height()) { // can't go down, convolution finished 
                //     break;
                // }
            }
            // to next pixel
            in_ptr += p_image.get_channel_num();
        } 
    }

    delete [] int_image;
    return int_result;
}

// native p_kernel_size * p_kernel_size convolution, modified from my handcraft CNN's convolution
template <typename type>
STB_Img DemakeCV::convolution_range_clamp(const STB_Img &p_image, type *p_kernel, int p_kernel_size) {
    // do it!
    type *int_result = inner_convolution<type>(p_image, p_kernel, p_kernel_size);

    // convert back
    stbi_uc *result_ptr = static_cast<stbi_uc *>(malloc(sizeof(stbi_uc) * p_image.get_pixel_num() * p_image.get_channel_num()));
    for (int i = 0; i < p_image.get_pixel_num() * p_image.get_channel_num(); ++i) {
        result_ptr[i] = static_cast<stbi_uc>(std::max(static_cast<type>(0), std::min(static_cast<type>(255), int_result[i])));
    }
    STB_Img result(result_ptr, p_image.get_width(), p_image.get_height(), p_image.get_channel_num(), p_image.get_name());

    delete [] int_result;
    return result;
}
// implements
template STB_Img DemakeCV::convolution_range_clamp<int>(const STB_Img &p_image, int *p_kernel, int p_kernel_size); 
template STB_Img DemakeCV::convolution_range_clamp<float>(const STB_Img &p_image, float *p_kernel, int p_kernel_size);


// p_kernel_size * p_kernel_size convolution, but result range is scaled to 0-255 
template <typename type>
STB_Img DemakeCV::convolution_range_scale(const STB_Img &p_image, type *p_kernel, int p_kernel_size) {
    // do it!
    type *int_result = inner_convolution<type>(p_image, p_kernel, p_kernel_size);

    // scale back
    stbi_uc *result_ptr = static_cast<stbi_uc *>(malloc(sizeof(stbi_uc) * p_image.get_pixel_num() * p_image.get_channel_num()));
    // for (int channel = 0; channel < p_image.get_channel_num(); ++channel) { // scale channel-wise to preserve color relation
        // get min/max
        type min = std::numeric_limits<type>::max(), max = std::numeric_limits<type>::min();
        type *bound = int_result + p_image.get_pixel_num() * p_image.get_channel_num();
        // for (type *img_ptr = int_result + channel; img_ptr < bound; img_ptr += p_image.get_channel_num()) {
        for (type *img_ptr = int_result; img_ptr < bound; ++img_ptr) {
            if (*img_ptr < min) {
                min = *img_ptr;
            }
            if (*img_ptr > max) {
                max = *img_ptr;     
            }
        }
        float scale = 255.0f / static_cast<float>(max - min);
        DEBUG_COUT(min);
        DEBUG_COUT(max);
        DEBUG_COUT(scale);


        // convert back (shift and scale)
        // for (int i = channel; i < p_image.get_pixel_num() * p_image.get_channel_num(); i += p_image.get_channel_num()) {

        for (int i = 0; i < p_image.get_pixel_num() * p_image.get_channel_num(); ++i) {
            result_ptr[i] = static_cast<stbi_uc>(static_cast<float>(int_result[i] - min) * scale);
        }
    // }
    STB_Img result(result_ptr, p_image.get_width(), p_image.get_height(), p_image.get_channel_num(), p_image.get_name());

    delete [] int_result;
    return result;
}
// implements
template STB_Img DemakeCV::convolution_range_scale<int>(const STB_Img &p_image, int *p_kernel, int p_kernel_size); 
template STB_Img DemakeCV::convolution_range_scale<float>(const STB_Img &p_image, float *p_kernel, int p_kernel_size);

