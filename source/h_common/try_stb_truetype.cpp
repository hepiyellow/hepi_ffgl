//
//  try_stb_truetype.cpp
//  FFGLPlugins
//
//  Created by Erez Makavy on 05/09/2021.
//

#include <string>
#include <iostream>
#include <math.h>
#include "try_stb_truetype.h"

using namespace std;

unsigned char * readFile(std::string filePath) {
    FILE *file;
    file = fopen(filePath.c_str(), "rb");
    fseek(file, 0, SEEK_END);
    const long size = ftell(file); /* how long is the file ? */
    fseek(file, 0, SEEK_SET); /* reset */
    unsigned char * ttf_buffer = new unsigned char[size];
    size_t bytesRead = fread(ttf_buffer, 1, size, file);
    cout << "BytesRead: " << bytesRead << "\n" ;
    return ttf_buffer;
}

void stbtt_GetBakedQuad2(const stbtt_bakedchar *chardata, int pw, int ph, int char_index, float *xpos, float *ypos, stbtt_aligned_quad *q, int opengl_fillrule)
{
   float d3d_bias = opengl_fillrule ? 0 : -0.5f;
   float ipw = 1.0f / pw, iph = 1.0f / ph;
   const stbtt_bakedchar *b = chardata + char_index;
   int round_x = ((int) floor((*xpos + b->xoff) + 0.5f));
   int round_y = ((int) floor((*ypos + b->yoff) + 0.5f));


    cout << "char(" + std::to_string(char_index) << ") b:";
    cout << "yoff=" <<  b->yoff << "\n";
   
   q->x0 = round_x + d3d_bias;
   q->y0 = round_y + d3d_bias;
   q->x1 = round_x + b->x1 - b->x0 + d3d_bias;
   q->y1 = round_y + b->y1 - b->y0 + d3d_bias;

   q->s0 = b->x0 * ipw;
   q->t0 = b->y0 * iph;
   q->s1 = b->x1 * ipw;
   q->t1 = b->y1 * iph;

   *xpos += b->xadvance;
}

void bakedTextureFont() {
//    const int textureWidth = 4096;
//    const int textureHeight = 4096;
//    const float pixel_height = 512;
    
    const int textureWidth = 4096;
    const int textureHeight = 4096;
    const float pixel_height = 512;

    unsigned char * ttf_buffer = readFile("/Users/erez/dev/ffgl/ffgl/resources/ariblk.ttf");
//    unsigned char * ttf_buffer = readFile("/Users/erez/dev/ffgl/ffgl/binaries/debug/fonts/NewYork.ttf");
    
    
    stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
    
    unsigned char * temp_bitmap = new unsigned char[textureWidth * textureHeight];
    const int bottomY = stbtt_BakeFontBitmap(ttf_buffer,0, pixel_height, temp_bitmap, textureWidth, textureHeight, 32,96, cdata);
    
//    for (int i=0; i < textureHeight; i++) {
//        for (int j=0; j < textureWidth; j++) {
//            int v = temp_bitmap[i*textureWidth + j];
//            wcout << (v > 0 ? "X" : " ");
//        }
//        wcout << "\n";
//    }
    cout << "BottomY: " << bottomY << "\n";
    delete [] ttf_buffer;
    delete [] temp_bitmap;
    std::string text = "A1B2";
    float x = 0;
    float y = 0;
    for (std::string::size_type i = 0; i < text.size(); i++) {
        int characterValue = text[i];
        if (characterValue >= 32 && characterValue < 128) {
           stbtt_aligned_quad q;
            
           stbtt_GetBakedQuad2(cdata, textureWidth,textureHeight, characterValue - 32, &x,&y,&q,1);//1=opengl & d3d10+,0=d3d9
//            float scale = 1;
            float s0 = q.s0, s1 = q.s1;
            float t0 = q.t0, t1 = q.t1;
            float x0 = q.x0, x1 = q.x1;
            float y0 = q.y0, y1 = q.y1;
            
            cout << text[i] << ":\n";
            cout << "s:" << "[" << s0 << "," << s1 << "], ";
            cout << "t:" << "[" << t0 << "," << t1 << "], ";
            cout << "xy0:" << "[" << x0 << "," << y0 << "], ";
            cout << "xy1:" << "[" << x1 << "," << y1 << "]";
            cout << "\n";
            cout << "height: " << y1 - y0 << "\n";
        }
    }


}

void renderCodePoint() {
    // Read
    unsigned char * ttf_buffer = readFile("/Users/erez/dev/ffgl/ffgl/resources/NewYork.ttf");
    // Init
    stbtt_fontinfo fontInfo;
    stbtt_InitFont(&fontInfo, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer,0));
    delete [] ttf_buffer;
    
    // Render
    
    int c = 195; // 'Ã';
    
//    const charHeight = 1080;
    const int charHeight = 50;
    const float scale = stbtt_ScaleForPixelHeight(&fontInfo, charHeight / 0.41);
    cout << "Scale: " << scale << "\n";
//    scale = stbtt_ScaleForPixelHeight(&fontInfo, 15);

    
    
    // Get dimensions
    int g = stbtt_FindGlyphIndex(&fontInfo, c);
    int  x0,y0,x1,y1;
    stbtt_GetGlyphBitmapBox(&fontInfo, g, scale,scale, &x0,&y0,&x1,&y1);
    int textureWidth = x1 - x0;
    int textureHeight = y1 - y0;
    
//    unsigned char *bitmap;
//    unsigned char bitmap[textureWidth * textureHeight];
    unsigned char *bitmap = stbtt_GetCodepointBitmap(&fontInfo, 0,scale, c, &textureWidth, &textureHeight, 0,0);
    
    
    
    
    cout << "TextureSize: " << textureWidth << "x" << textureHeight << "\n";
    for (int i=0; i < textureHeight; i++) {
        for (int j=0; j < textureWidth; j++) {
            int v = bitmap[i*textureWidth + j];
            wcout << (v > 0 ? "X" : " ");
        }
        wcout << "\n";
    }
}

int main() {
//    renderCodePoint();
    bakedTextureFont();
}
