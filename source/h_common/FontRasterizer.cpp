//
//  FontRasterizer.cpp
//  HTextChaser
//
//  Created by Erez Makavy on 06/09/2021.
//


#include <string>

#include "FontRasterizer.h"
#include "Utils.h"

using namespace std;
using namespace utils;

FontRasterizer::FontRasterizer() :
    ftex(0)
{
    log("FontRasterizer()");
};

FontRasterizer::~FontRasterizer() {
    log("~FontRasterizer()");
    assert( ftex == 0 );
    delete cdata;
}

int FontRasterizer::getLetterAHeight() {
    return letterAHeight;
}
void FontRasterizer::InitGL() {
    log("FontRasterizer::InitGL()");
    glGenTextures(1, &ftex);
}

void FontRasterizer::DeInitGL() {
    log("FontRasterizer::DeInitGL()");
    glDeleteTextures(1, &ftex);
    ftex = 0;
}

GLuint FontRasterizer::getTextureId() {
    return ftex;
}

void FontRasterizer::setFont(string fontPath) {
    fontFilePath = fontPath;
}
                                
bool FontRasterizer::initializeFont() {
    unsigned char * ttf_buffer = readFile(fontFilePath);
    fontInfo = {0};
    fontInfo.userdata = NULL;
    if (!stbtt_InitFont(&fontInfo, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer,0))) {
        return false;
    }
    delete [] ttf_buffer;
    return true;
}

void FontRasterizer::GetBakedQuad(char16_t characterValue, float *xpos, float *ypos, stbtt_aligned_quad *q) {
    const int characterIndex = codepointToIndex[characterValue];
    stbtt_GetBakedQuad(cdata, textureWidth, textureHeight, characterIndex, xpos, ypos, q, 1);
}

unsigned char * FontRasterizer::readFile(string filePath) {
    FILE *file;
    file = fopen(filePath.c_str(), "rb");
    fseek(file, 0, SEEK_END);
    const long size = ftell(file); /* how long is the file ? */
    fseek(file, 0, SEEK_SET); /* reset */
    unsigned char * ttf_buffer = new unsigned char[size];
    size_t bytesRead = fread(ttf_buffer, 1, size, file);
    log( "readFile(): bytesRead=" + to_string( bytesRead) );
    return ttf_buffer;
}

bool FontRasterizer::initializeBakedFont(std::string fontFilePath) {
    unsigned char * ttf_buffer = readFile(fontFilePath);
    
    createBakedTexture(ttf_buffer); // deletes ttf_buffer
    
    return true;
}

void FontRasterizer::updateFontTexture(std::u16string text) {
    if (fontFilePath.length() == 0) {
        logError("FontRasterizer::updateFontTexture(): setFont needs to be called prior to calling this function");
    }
    FFGLLog::LogToHost( "TextRenderer::updateFontTexture()");
        
    std::vector<int> uniqCharacters;
    codepointToIndex.clear();
    
    // Always add /the 'A' character as a Line height representator.
    int codeA = toUtf16("A")[0];
    uniqCharacters.push_back(codeA);
    codepointToIndex[codeA] = 0;
    
    int j=1;
    for (int i=0; i < text.size() ; i++ ) {
        int code = text[i];
        FFGLLog::LogToHost( ("TextRenderer::updateFontTexture() code=" + std::to_string(code)).c_str());
        if (codepointToIndex.find(code) == codepointToIndex.end()) {
            codepointToIndex[code] = j;
            uniqCharacters.push_back(code);
            j++;
        }
    }
    
    log( "TextRasterizer::updateFontTexture() uniqCharacters.size() = " + to_string(uniqCharacters.size()));
    
    
    // TODO: fontInfo seems to be corrupted after a second call to updateFontTexture(), this seems to fix it.
    initializeFont();
    
    if (cdata != NULL) {
        delete cdata;
    }
    cdata = new stbtt_bakedchar[uniqCharacters.size()];
    unsigned char * temp_bitmap = new unsigned char[textureWidth * textureHeight];
    FFGLLog::LogToHost( "new temp_bitmap");
    const int bottomY = stbtt_BakeFontBitmap_internal_MY(0, pixel_height, temp_bitmap, textureWidth, textureHeight, uniqCharacters, cdata);
    FFGLLog::LogToHost( ("bottomY=" + std::to_string(bottomY)).c_str() );
    
    
    float x = 0;
    float y = 0;
    stbtt_aligned_quad q;
    GetBakedQuad(codeA, &x, &y, &q);
    letterAHeight = q.y1 - q.y0;
    
    
    
    glDeleteTextures(1, &ftex);
    ftex = 0;
    glGenTextures(1, &ftex);
    
    glBindTexture(GL_TEXTURE_2D, ftex);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureWidth,textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);

    delete [] temp_bitmap;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    FFGLLog::LogToHost( ("TextRenderer::createBakedTexture() - Texture Created with id=" + std::to_string(ftex)).c_str());
}

void FontRasterizer::createBakedTexture(unsigned char * ttf_buffer) {
    unsigned char * temp_bitmap = new unsigned char[textureWidth * textureHeight];
    const int bottomY = stbtt_BakeFontBitmap(ttf_buffer,0, pixel_height, temp_bitmap, textureWidth, textureHeight, 32,96, cdata);
    FFGLLog::LogToHost( ("bottomY=" + std::to_string(bottomY)).c_str() );
    delete [] ttf_buffer;
    
    glGenTextures(1, &ftex);
    glBindTexture(GL_TEXTURE_2D, ftex);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureWidth,textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);

    delete [] temp_bitmap;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    FFGLLog::LogToHost( ("TextRenderer::createBakedTexture() - Texture Created with id=" + std::to_string(ftex)).c_str());
}

void FontRasterizer::getCharacterQuad(char16_t characterValue, float *x, float *y, TexturedQuadPoints &tq) {
    stbtt_aligned_quad q;
    
    GetBakedQuad( characterValue, x,y,&q);
    
    tq.s0 = q.s0;
    tq.s1 = q.s1;
    tq.t0 = q.t1; tq.t1 = q.t0; // Flip vertically
    tq.x0 = q.x0;
    tq.y0 = q.y1;// flip
    tq.x1 = q.x1;
    tq.y1 = -q.y0; // positive y should be towards the top
}

void FontRasterizer::createGlyphTexture() {
    const int charHeight = 20;
    
    const float scale = stbtt_ScaleForPixelHeight(&fontInfo, charHeight / 0.41);
//    int textureWidth,textureHeight;
    int c = 195;
//    unsigned char *bitmap;
//    bitmap = stbtt_GetCodepointBitmap(&fontInfo, 0,stbtt_ScaleForPixelHeight(&fontInfo, charHeight / 0.41), c, &textureWidth, &textureHeight, 0,0);
    
    // Get dimensions
    int g = stbtt_FindGlyphIndex(&fontInfo, c);
    int x0,y0,x1,y1;
    stbtt_GetGlyphBitmapBox(&fontInfo, g, scale,scale, &x0,&y0,&x1,&y1);
    int textureWidth = x1 - x0;
    int textureHeight = y1 - y0;
    
    
    unsigned char *bitmap = new unsigned char[textureWidth * textureHeight];
    stbtt_MakeGlyphBitmap(&fontInfo, bitmap, textureWidth,textureHeight,textureWidth, scale,scale, g);
    
    
    std::stringstream ss;
    ss << "TextureSize: " << textureWidth << "x" << textureHeight << "\n";
    FFGLLog::LogToHost( ss.str().c_str());
    
    for (int i=0; i < textureHeight; i++) {
        std::stringstream line;

        for (int j=0; j < textureWidth; j++) {
            int v = bitmap[i*textureWidth + j];
            line << (v > 0 ? "X" : " ");
        }
        FFGLLog::LogToHost( line.str().c_str());

    }
//    GLuint texID;
    glGenTextures(1, &ftex);
    glBindTexture(GL_TEXTURE_2D, ftex);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureWidth,textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);

    delete [] bitmap;

    
    
    // Cleanup
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
//    return texID;
}

//////////////////////////////////////////////////////////////////////////////
//
// bitmap baking
//
// This is SUPER-CRAPPY packing to keep source code small

int FontRasterizer::stbtt_BakeFontBitmap_internal_MY(int offset,  // font location (use offset=0 for plain .ttf)
                                float pixel_height,                     // height of font in pixels
                                unsigned char *pixels, int pw, int ph,  // bitmap to be filled in
                                std::vector<int> characters,         // characters to bake
                                stbtt_bakedchar *chardata)
{
   log( "stbtt_BakeFontBitmap_internal_MY");
   float scale;
   int x,y,bottom_y, i;
   memset(pixels, 0, pw*ph); // background of 0 around pixels
   x=y=1;
   bottom_y = 1;

   scale = stbtt_ScaleForPixelHeight(&fontInfo, pixel_height);

   for (i=0; i < characters.size(); ++i) {
       FFGLLog::LogToHost( ("BakeFontBitmap : index: " + std::to_string(i) + ", code: " + std::to_string( characters[i])).c_str() );
      int advance, lsb, x0,y0,x1,y1,gw,gh;
      int g = stbtt_FindGlyphIndex(&fontInfo, characters[i]);
      FFGLLog::LogToHost( ("BakeFontBitmap : g: " + std::to_string(g)).c_str());
      stbtt_GetGlyphHMetrics(&fontInfo, g, &advance, &lsb);
      stbtt_GetGlyphBitmapBox(&fontInfo, g, scale,scale, &x0,&y0,&x1,&y1);
      gw = x1-x0;
      gh = y1-y0;
      if (x + gw + 1 >= pw)
          static_cast<void>(y = bottom_y), x = 1; // advance to next row
      if (y + gh + 1 >= ph) // check if it fits vertically AFTER potentially moving to next row
         return -i;
      assert(x+gw < pw);
      assert(y+gh < ph);
      stbtt_MakeGlyphBitmap(&fontInfo, pixels+x+y*pw, gw,gh,pw, scale,scale, g);
      chardata[i].x0 = (signed   short) x;
      chardata[i].y0 = (signed   short) y;
      chardata[i].x1 = (signed   short) (x + gw);
      chardata[i].y1 = (signed   short) (y + gh);
      chardata[i].xadvance = scale * advance;
      chardata[i].xoff     = (float) x0;
      chardata[i].yoff     = (float) y0;
      x = x + gw + 1;
       FFGLLog::LogToHost( ("BakeFontBitmap : y+gh+1 : " + std::to_string(y+gh+1)).c_str() );
      if (y+gh+1 > bottom_y)
         bottom_y = y+gh+1;
   }
   return bottom_y;
}


