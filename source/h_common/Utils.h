//
//  Utils.hpp
//  HCloner
//
//  Created by Erez Makavy on 04/09/2021.
//

#ifndef Utils_h
#define Utils_h

#include <stdio.h>
#include <FFGLSDK.h>

using namespace std;

namespace utils {
    
/**
  A Textured Quad.
  This is the same as stbtt_aligned_quad but with different semantics.
  xy0 is for bottom-left, instead of top-left.
  xy1 is for top-right, instead of bottom-right.
 */
struct TexturedQuadPoints
{
   float x0,y0,s0,t0; // bottom-left
   float x1,y1,s1,t1; // top-right
};

struct Point {
    float x,y;
};

/**
 Free Transformable Quad.
 A Quad that can be transformed in any way.
 
 */
struct FTQuad
{
   Point bl,tl,tr,br; // bottom-left, top-left, top-right, bottom-right
};

void transformQuad(FTQuad &sourceQuad, Point sourceAnchor, Point targetAnchor, float rotation);

void addQuadVertices(TexturedQuadPoints q, std::vector<ffglex::GlVertexTextured> &vertexArray, int *index);
void updateVerticesFromQuads(vector<TexturedQuadPoints> &quads, vector<ffglex::GlVertexTextured> &vertices);


void log(std::string msg);
void log(std::stringstream &msg);
void logError(std::string msg);

std::u16string toUtf16(std::string text);
std::string toUtf8(std::u16string text);

}
#endif /* Utils_h */
