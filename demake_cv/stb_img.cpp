#include "stb_img.h"
#include "../stb/stb_image_write.h"
#include "../TinyTIFF-4.0.1.0/src/tinytiffreader.hxx"
#include "../utilities/error_handler.h"
#include "../utilities/code_tester.h"

#include <filesystem>

STB_Img &STB_Img::operator-() {
    for (int i = 0; i < get_pixel_num() * get_channel_num(); ++i) {
        image[i] = 255 - image[i];
    }
    return *this;
}

STB_Img& STB_Img::operator=(const STB_Img& p_img) {
    name = p_img.name;
    width = p_img.width;
    height = p_img.height;
    channel_num = p_img.channel_num;
    image = static_cast<stbi_uc *>(memcpy(image, p_img.image, width * height * channel_num * sizeof(stbi_uc)));
    return *this;
}

// clamp to 255 if result pixel value > 255
STB_Img &STB_Img::operator+=(const STB_Img& p_img) {
    ErrorHandler_FATAL_IF(channel_num != p_img.get_channel_num(), __FILE__, __LINE__) // only allow 1 frame
    for (int i = 0; i < get_pixel_num() * get_channel_num(); ++i) {
        int result = static_cast<int>(image[i]) + static_cast<int>(p_img.image[i]); 
        if (result > 255) {
            result = 255;
        }
        image[i] = static_cast<stbi_uc>(result);
    }
    return *this;
}

// clamp to 0 if result pixel value < 0
STB_Img &STB_Img::operator-=(const STB_Img& p_img) {
    ErrorHandler_FATAL_IF(channel_num != p_img.get_channel_num(), __FILE__, __LINE__) // only allow 1 frame
    for (int i = 0; i < get_pixel_num() * get_channel_num(); ++i) {
        int result = static_cast<int>(image[i]) - static_cast<int>(p_img.image[i]); 
        if (result < 0) {
            result = 0;
        }
        image[i] = static_cast<stbi_uc>(result);
    }
    return *this;
}

// clamp to 255 if result pixel value > 255
STB_Img &STB_Img::operator*=(float scalar) {
    stbi_uc *bound = get_image() + get_pixel_num() * get_channel_num();
    for (stbi_uc *now_ptr = get_image(); now_ptr < bound; ++now_ptr) {
        float result = static_cast<float>(*now_ptr) * scalar; 
        if (result > 255) {
            result = 255;
        }
        *now_ptr = static_cast<stbi_uc>(result);
    }
    return *this;
}


STB_Img STB_Img::operator-(const STB_Img& p_img) const {
    STB_Img result(*this);
    result -= p_img;
    return result;
}

STB_Img STB_Img::operator*(float scalar) const{
    STB_Img result(*this);
    result *= scalar;
    return result;
}

STB_Img STB_Img::operator+(const STB_Img& p_img) {
    STB_Img result(*this);
    result += p_img;
    return result;
}


STB_Img::STB_Img(string p_path): 
name(p_path),
image(stbi_load(p_path.c_str(), &width, &height, &channel_num, 3)) {
    if (image == NULL) { // try read tif instead
        TinyTIFFReaderFile* tiff_reader = TinyTIFFReader_open(p_path.c_str());
        if (tiff_reader) {
            ErrorHandler_FATAL_IF(TinyTIFFReader_countFrames(tiff_reader) != 1, __FILE__, __LINE__) // only allow 1 frame
            ErrorHandler_FATAL_IF(TinyTIFFReader_getBitsPerSample(tiff_reader, 0) != 8, __FILE__, __LINE__) // only allow 8 bits / one chaneel pixel
            width = TinyTIFFReader_getWidth(tiff_reader);
            height = TinyTIFFReader_getHeight(tiff_reader);
            channel_num = TinyTIFFReader_getSamplesPerPixel(tiff_reader);
            image = (stbi_uc *)malloc(sizeof(stbi_uc) * width * height * channel_num);
            if (!TinyTIFFReader_getSampleData(tiff_reader, image, 0)) {
                ErrorHandler::fatal_error(TinyTIFFReader_getLastError(tiff_reader), __FILE__, __LINE__);
            } 
        }
    }
    ErrorHandler_FATAL_IF(image == NULL, __FILE__, __LINE__); 
}

STB_Img::STB_Img(const STB_Img &p_img):
name(p_img.name),
width(p_img.width),
height(p_img.height),
channel_num(p_img.channel_num),
image( static_cast<stbi_uc *>(malloc(sizeof(stbi_uc) * width * height * channel_num)) ) {
    image = static_cast<stbi_uc *>(memcpy(image, p_img.image, width * height * channel_num * sizeof(stbi_uc)));
}

STB_Img::STB_Img(stbi_uc *p_image, int p_width, int p_height, int p_channel_num, string p_name):
name(p_name),
width(p_width),
height(p_height),
channel_num(p_channel_num),
image(p_image) {}

STB_Img::~STB_Img() {
    stbi_image_free(image);
}

void STB_Img::store(string p_file, string p_folder) {
    // ensure output dir exists (when exists: return false, but no error_ocde)
    error_code err_code;
    filesystem::create_directories(p_folder, err_code);
    if (err_code.value() != 0) {
        ErrorHandler::fatal_error(err_code.message(), __FILE__, __LINE__);
    }
    // store image
    string file_path = p_folder + ("/" + p_file);
    if (stbi_write_png(file_path.c_str(), width, height, channel_num, image, width * channel_num) == 0) {  // save RGB
        ErrorHandler::fatal_error("fail to save " + file_path, __FILE__, __LINE__);
    }
    ErrorHandler::success_notice("Save " + p_file + " in " + p_folder);
}


STB_Img operator*(float scalar, const STB_Img &image) {
    STB_Img result(image);
    result *= scalar;
    return result;
}
