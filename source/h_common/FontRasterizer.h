//
//  FontRasterizer.hpp
//  HTextChaser
//
//  Created by Erez Makavy on 06/09/2021.
//

#ifndef FontRasterizer_h
#define FontRasterizer_h

#include <stdio.h>
#include <string>
#include <FFGLSDK.h>
#include "stb_truetype.h"
#include "Utils.h"

using namespace std;
using namespace utils;

class FontRasterizer {
public:
    FontRasterizer();
    ~FontRasterizer();
    void InitGL();
    void DeInitGL();
    void setFont(string fontFilePath);
    void updateFontTexture(std::u16string text);
    void getCharacterQuad(char16_t characterValue, float *x, float *y, TexturedQuadPoints &tq);
    GLuint getTextureId();
    int getLetterAHeight();
private:
    const float pixel_height = 1024.0;
    const int textureWidth = 8192;
    const int textureHeight = 8192;
    
    int letterAHeight; // The height of the letter 'A'
    
    string fontFilePath;
    stbtt_fontinfo fontInfo;
    GLuint ftex;
    stbtt_bakedchar *cdata = NULL; // ASCII 32..126 is 95 glyphs
    std::vector<int> textCodepointToIndex;
    std::map<char16_t,int> codepointToIndex;
    
    unsigned char * readFile(std::string filePath);
    bool initializeFont();
    bool initializeBakedFont(std::string fontFilePath); // Read font file and initialize a texture with all font characters (ftex)
   
    void GetBakedQuad(char16_t characterValue, float *xpos, float *ypos, stbtt_aligned_quad *q);
    void createBakedTexture(unsigned char * ttf_buffer); // creates texture of all characters with texture id ftex
    void createGlyphTexture();
    int stbtt_BakeFontBitmap_internal_MY(int offset,  // font location (use offset=0 for plain .ttf)
                                    float pixel_height,                     // height of font in pixels
                                    unsigned char *pixels, int pw, int ph,  // bitmap to be filled in
                                    std::vector<int> characters,         // characters to bake
                                     stbtt_bakedchar *chardata);
};


#endif /* FontRasterizer_h */

