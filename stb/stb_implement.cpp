// stbL single-file public domain libraries for C/C++
// https://github.com/nothings/stb?tab=readme-ov-file

// CHANGE `stb_image_write.h` line 773:
//  `#ifdef  __STDC_LIB_EXT1__` to `#if defined(__STDC_LIB_EXT1__) || defined(_MSC_VER)` 
//  to fix `warning C4996: 'sprintf': This function or variable may be unsafe.`


// only bmp, jpeg, png needed (.tif not supported), reduce binary size
#define STBI_ONLY_BMP
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"       
#include "stb_image_write.h"