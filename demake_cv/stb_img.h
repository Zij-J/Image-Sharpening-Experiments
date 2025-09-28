#ifndef STB_IMG_H
#define STB_IMG_H

#include "../stb/stb_image.h"
#include <string>
#include <functional>   // for `std::function`
using namespace std;


class STB_Img {
public:
    STB_Img &operator-();
    STB_Img &operator=(const STB_Img& p_img);
    STB_Img &operator+=(const STB_Img& p_img);
    STB_Img &operator-=(const STB_Img& p_img);
    STB_Img &operator*=(float scalar);
    STB_Img operator-(const STB_Img& p_img) const;
    STB_Img operator*(float scalar) const;
    STB_Img operator+(const STB_Img& p_img);

    const string &get_name() const {
        return name;
    }
    int get_pixel_num() const {
        return width * height;
    }
    int get_width() const {
        return width;
    }
    int get_height() const {
        return height;
    }
    int get_channel_num() const {
        return channel_num;
    }
    stbi_uc *get_image() const {
        return image;
    }

    STB_Img(string p_path);
    STB_Img(const STB_Img &p_img);
    STB_Img(stbi_uc *p_image, int p_width, int p_height, int p_channel_num = 3, string p_name = "");
    ~STB_Img();

    void store(string p_full_name, string p_folder = "./");

private:
    string name;        // follow convention below
    int width;          // need to be modified inside the class, can't `const`
    int height;
    int channel_num; 
    stbi_uc *image;     
};

STB_Img operator*(float scalar, const STB_Img &image); 

#endif