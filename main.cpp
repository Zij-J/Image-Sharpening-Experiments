#include "utilities/error_handler.h"
#include "utilities/code_tester.h"
#include "demake_cv/stb_img.h"
#include "demake_cv/demake_cv.h"

#include <iostream>
#include <filesystem>
#include <string>
using namespace std;

#define INPUT_FOLDER "./test_images"
#define OUTPUT_FOLDER "./outputs"


int main(void) {

    // read all .bmp & .tif in INPUT_FOLDER. Process them one by one.
    try {
        for (const filesystem::directory_entry &entry : filesystem::directory_iterator(INPUT_FOLDER)) {
            // open image & preprocess file name
            STB_Img image(entry.path().string());
            filesystem::path origin_path = entry.path();
            string origin_file_name = origin_path.replace_extension().filename().string();
            
            // calculate
            // float kernel[9] = {0, 1, 0, 1, -4, 1, 0, 1, 0}; // origin Laplacian, extract changes "away" from pixel 
            float kernel[9] = {0, -1, 0, -1, 5, -1, 0, -1, 0}; // reverse sign to enhance changes "toward" pixel
            // int kernel[9] = {1, 1, 1, 1, -8, 1, 1, 1, }; 
            cout << "\nSharpening by Laplacian operator (clamp)..." << endl;
            STB_Img result(DemakeCV::convolution_range_clamp<float>(image, kernel, 3));
            ErrorHandler::success_notice("Laplacian operator (clamp)");
            // result += image;
            result.store("laplacian_clamp_" + origin_file_name + ".png", OUTPUT_FOLDER);

            result += 0.5f * image;
            result.store("laplacian_clamp_boost_" + origin_file_name + ".png", OUTPUT_FOLDER);


            cout << "\nnSharpening by Laplacian operator (scale)..." << endl;
            result = STB_Img(DemakeCV::convolution_range_scale<float>(image, kernel, 3));
            ErrorHandler::success_notice("Laplacian operator (scale)");
            // result += image;
            result.store("laplacian_scale_" + origin_file_name + ".png", OUTPUT_FOLDER);


            float val = -1.f / 5.f;
            // float average_kernel[9] = {val, val, val, val, val, val, val, val, val}; 
            float average_kernel[9] = {0, val, 0, val, 2.f + val, val, 0, val, 0}; 
            cout << "\nSharpening by Deblurring (clamp)..." << endl;
            result = DemakeCV::convolution_range_clamp<float>(image, average_kernel, 3);
            ErrorHandler::success_notice("Deblurring (clamp)");
            result.store("deblurr_clamp_" + origin_file_name + ".png", OUTPUT_FOLDER);

            result += 0.5f * image;
            result.store("deblurr_clamp_boost_" + origin_file_name + ".png", OUTPUT_FOLDER);


            cout << "\nSharpening by Deblurring (scale)..." << endl;
            result = DemakeCV::convolution_range_scale<float>(image, average_kernel, 3);
            ErrorHandler::success_notice("Deblurring (scale)");
            result.store("deblurr_scale_" + origin_file_name + ".png", OUTPUT_FOLDER);


            // close image by destructor
        }
    } catch (const filesystem::filesystem_error& err) {
        ErrorHandler::fatal_error(err.what(), __FILE__, __LINE__);
    }
    

    return 0;
}


//TODO:
//  [V] read .tif file: need TinyTiff https://jkriege2.github.io/TinyTIFF/page_useinstructions.html
//  [V] convolution
//      [V] hard clamp
//      [V] clamp range by scaling 
//  [V] Laplacian operator sharpening
//  [V] image addition
//  [V] blur  
//  [V] high-boost filtering
//  [?] FFT
//      [] DFT
//      [] mask
//      [] inverse DFFT
//  [?] to GLSL (faster only in creating new pic)
