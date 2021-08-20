//
//  Utils.cpp
//  HCloner
//
//  Created by Erez Makavy on 04/09/2021.
//

#include "Utils.h"
#include <FFGLSDK.h>
#include <string>
#include <codecvt>

using namespace std;

namespace utils
{

void updateVerticesFromQuads(vector<TexturedQuadPoints> &quads, vector<ffglex::GlVertexTextured> &vertices) {
    const int quadVertexCount = 6;
    const std::string::size_type vertexCount = quadVertexCount * quads.size();
    
    if (vertices.size() != vertexCount) {
        vertices.resize(vertexCount);
    }
    
    int vertexIndex = 0;
    
    for(auto const& quad: quads) {
        utils::addQuadVertices(quad, vertices, &vertexIndex);
    }
}
/**
 Converts Quad Points to 2 Triangle 6 vertices.
 Assumes quad is not rotated  !
 Increaments the vertice index.
 */
void addQuadVertices(TexturedQuadPoints q, vector<ffglex::GlVertexTextured> &vertexArray, int *index) {
    int i = *index;
    vertexArray[i]     = { q.s0, q.t0, q.x0, q.y0, 0.0f }; //Bottom-left
    vertexArray[i + 1] = { q.s1, q.t0, q.x1, q.y0, 0.0f }; //Bottom-right
    vertexArray[i + 2] = { q.s0, q.t1, q.x0, q.y1, 0.0f }; //Top-left

    vertexArray[i + 3] = { q.s0, q.t1, q.x0, q.y1, 0.0f }; //Top-left
    vertexArray[i + 4] = { q.s1, q.t0, q.x1, q.y0, 0.0f }; //Bottom-right.
    vertexArray[i + 5] = { q.s1, q.t1, q.x1, q.y1, 0.0f }; //Top-right
     
    *index = i+6;
}

void log(std::stringstream &msg) {
    FFGLLog::LogToHost(msg.str().c_str());
}

void log(std::string msg) {
    FFGLLog::LogToHost(msg.c_str());
}

void logError(std::string msg) {
    FFGLLog::LogToHost(("ERROR: " + msg).c_str());
}

/**
  Takes in  a Quad and transforms it.
 */
//void transformQuad(FTQuad &sourceQuad, Point sourceAnchor, Point targetAnchor, float rotation) {
//    // Angle - angle of 0 points north. and positive foes clockwise
//    float angleRad = charIndex * angleStepRad;
//    Point bc = {radius * sin(angleRad), radius * cos(angleRad)}; // Bottom-Center
//    Point tc = {(radius + height) * sin(angleRad), (radius + height) * cos(angleRad)}; // Top-Center
//    float dx = (width / 2)  * cos(angleRad);
//    float dy = (height / 2) * sin(angleRad);
//    q.x0 = bc.x - dx;
//    q.y0 = bc.y + dy;
//    q.x1 = tc.x + dx;
//    q.y1 = tc.y - dy;
//    
//}

std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;

std::u16string toUtf16(std::string text) {
    std::u16string text_u16 = std::wstring_convert<
        std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(text);
    
    return text_u16;
}

std::string toUtf8(std::u16string text) {
    
    return convert.to_bytes(text);
}

}
