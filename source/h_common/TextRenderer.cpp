//
//  TextRenderer.cpp
//
//  Created by Erez Makavy on 31/08/2021.
//

#include "TextRenderer.h"


#include "Utils.h"
#include <iostream>
#include <string>
#include <numbers>
//#include <locale>
#include <codecvt>
#include "circleShaders.h"

using namespace ffglex;
using namespace ffglqs;
using namespace tr;
using namespace utils;



TextRenderer::TextRenderer():
    vaoID( 0 ),
    vboID( 0 )
{
    log("TextRenderer()");
};

TextRenderer::~TextRenderer() {
    log("~TextRenderer()");
    //If any of these assertions hit you forgot to release gl resources.
    assert( vaoID == 0 );
    assert( vboID == 0 );
    
}


bool TextRenderer::InitGL(const FFGLViewportStruct* vp) {
    log("TextRenderer.InitGL()");

    if (!rowLayout.InitGL(vp)) {
        return false;
    }
    
    if( !circleLayout.InitGL(vp)) {
        return false;
    }
    FFGLLog::LogToHost("TextRenderer.InitGL() shaders compiled");
    
    
    glGenVertexArrays( 1, &vaoID );
    glGenBuffers( 1, &vboID );
    if( vaoID == 0 || vboID == 0 )
    {
        return false;
    }
    
    
    fontRasterizer.InitGL();
            
    viewportAspect =  (float)vp->height / (float)vp->width;
    log("TextRenderer::InitGL(): vp->width=" + to_string(vp->width) + ", vp->height=" + to_string(vp->height));
    
    return true;
}




bool TextRenderer::DeInitGL() {
    rowLayout.DeInitGL();
    circleLayout.DeInitGL();
    
    glDeleteBuffers( 1, &vboID );
    vboID = 0;
    glDeleteVertexArrays( 1, &vaoID );
    vaoID = 0;
    
    fontRasterizer.DeInitGL();
    
    return true;
}

void TextRenderer::updateFontTextureIfNeeded(string fontPath, u16string text) {
    if (lastFont != fontPath || lastText != text) {
        log("TextRenderer::updateFontTexture(): create new Texture (fontPath= " + fontPath + ", text=" + toUtf8(text)+")");
        lastFont = fontPath;
        lastText = text;
        fontRasterizer.setFont(fontPath);
        fontRasterizer.updateFontTexture(lastText);
    } else {
//        log("TextRenderer::updateFontTexture(): Keeping existing texture. (fontPath= " + fontPath + ", text=" + toUtf8(text)+")");
    }
}

void TextRenderer::draw(TextParams &params) {

    BaseLayout *layout;
    if (params.layout == Layout::Rows) {
        layout = &rowLayout;
    } else {
        layout = &circleLayout;
    }
//    auto *layout = params.layout == Layout::Rows ?  &rowLayout : &circleLayout;
    // Prepare OpenGL
    glUseProgram( layout->shader.GetGLID() );
    glActiveTexture( GL_TEXTURE0 ); // TODO: is this correct?
    glBindTexture(GL_TEXTURE_2D, fontRasterizer.getTextureId());
    
    glBindVertexArray(vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    
    //
    u16string textToDraw = getTextToDraw(params);
    layout->setUniforms(params, textToDraw);
    if (params.layout == Layout::Rows) {
        layout->updateVertices(params, textToDraw, fontRasterizer , vertices);
    } else {
        updateVerticesCircleLayout(params, textToDraw);
    }

    //    updateVerticesWithAllCharacters(); // FOR DEBUG
    
    glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), static_cast<GLvoid*>(& vertices[0]), GL_STATIC_DRAW );
    
    // Configure Vertesx Array Attributes
    glEnableVertexAttribArray( 0 );
    // xyz as 3rd, 4th and 5th floats
    glVertexAttribPointer( 0, 3, GL_FLOAT, false, sizeof( TEXTURED_QUAD_VERTICES[ 0 ] ), (char*)NULL + 2 * sizeof( float ) );
    // Texture coordinates (u,v) as 1st and 2nd floats
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 1, 2, GL_FLOAT, false, sizeof( TEXTURED_QUAD_VERTICES[ 0 ] ), (char*)NULL + 0 * sizeof( float ) );

    
    // Draw
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
    glDrawArrays( params.layout == Layout::Rows ? GL_TRIANGLES : GL_TRIANGLES_ADJACENCY, 0, static_cast<int>(vertices.size()) );
    glBlendFunc(GL_ONE, GL_ZERO);
    glDisable(GL_BLEND);
    
    // Unbind Vertex Data
    glBindVertexArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    
    // Unbind Texture Data
    glActiveTexture( GL_TEXTURE0 ); // Is this redundant? (it's what the ScopedSamplerActivation does)
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Unbind Shader
    glUseProgram( 0 );

}

u16string TextRenderer::getTextToDraw(TextParams &params) {
    const u16string originalText = params.text;
    const Range mode = params.range;
    
    //TODO: Optimization: assuming that phase and length may change fast, it is better to implement it in the TextRenderer thus giving it an opportunity to optimize and notbuild a new font bitmap to be rasterized (which is what happens when we change the text).
    const float offsetNorm = params.offset;
    const float lengthNorm = params.length;
    
    
    //TODO: Optimizaiton: use someking of stream instead of concating u16string
    u16string textToDraw;
    switch (mode) {
        case Range::Characters: {
            const int length = lengthNorm == 1 ? (int) originalText.length() : floor((originalText.length()+1) * lengthNorm);
            const int offsetIndex = (int) (offsetNorm == 1 ? originalText.length() - 1 : floor(originalText.length() * offsetNorm));
            for (int i=0; i < length; i++ ) {
                textToDraw += originalText[(offsetIndex + i) % originalText.length()];
            }
            break;
        }
        case Range::Words: {
            vector<u16string> words;
            std::u16string delimiter = toUtf16(" ");
            int start = 0;
            int end = (int) originalText.find(delimiter);
            while (end != std::string::npos)
            {
                words.push_back(originalText.substr(start, end - start));
                start = (int)(end + delimiter.length());
                end = (int) originalText.find(delimiter, start);
            }

            words.push_back(originalText.substr(start, end));
            
            const int length = lengthNorm == 1 ? (int) words.size() : floor((words.size() + 1) * lengthNorm);
            const int offsetIndex = (int) (offsetNorm == 1 ? originalText.length() - 1 : floor(originalText.length() * offsetNorm));
            for (int i=0; i < length; i++ ) {
                const u16string space = toUtf16((i < length - 1) ? " " : "");
                textToDraw += words[(offsetIndex + i) % words.size()] + space;
            }
            break;
        }
        case Range::Lines: {
            vector<u16string> lines;
            std::u16string delimiter = toUtf16("\n");
            int start = 0;
            int end = (int) originalText.find(delimiter);
            while (end != std::string::npos)
            {
                lines.push_back(originalText.substr(start, end - start));
                start = (int)(end + delimiter.length());
                end = (int) originalText.find(delimiter, start);
            }

            lines.push_back(originalText.substr(start, end));
            
            const int length = lengthNorm == 1 ? (int) lines.size() : floor((lines.size() + 1) * lengthNorm);
            const int offsetIndex = (int) (offsetNorm == 1 ? originalText.length() - 1 : floor(originalText.length() * offsetNorm));
            for (int i=0; i < length; i++ ) {
                const u16string newLine = toUtf16((i < length - 1) ? "\n" : "");
                textToDraw += lines[(offsetIndex + i) % lines.size()] + newLine;
            }
            break;
        }
            
    }
    return textToDraw;
}

void TextRenderer::updateVerticesCircleLayout(TextParams &params, u16string text)
{
    
    // I want the default to be that the letter 'a' takes 1/4 of the screen height;
    // When params.size == 1 then the letter A's height would take 1/2 a screen height.
    float appliedScale = params.size * (1.0f / fontRasterizer.getLetterAHeight());
    
    vector<TexturedQuadPoints> quads;
    float x = 0;
    float y = 0;
//    float max_x = 0; // width
//    float max_y = 0; // height

//    float angleStepRad = PI_x2 / text.size();
//    float radius = 0.25;
    for (std::string::size_type charIndex = 0; charIndex < text.length(); charIndex++) {
        const char16_t characterValue = text[charIndex];
        
        
        // 01 - Get Char Quad
        x = 0;
        y = 0;
        TexturedQuadPoints q;
        fontRasterizer.getCharacterQuad(characterValue, &x, &y, q);
        
        // 02 Apply Scale
        q.x0 *= appliedScale;
        q.y0 *= appliedScale;
        q.x1 *= appliedScale;
        q.y1 *= appliedScale;
        
        // Put character centered around X=0. (anchor at (0,0))
        float width = (q.x1 - q.x0)/2;
        float height = (q.y1 - q.y0)/2;
        
        q.x0 =-width;
        q.x1 =width;
        q.y0 =-height;
        q.y1 =height;
        
        quads.push_back(q);
    }
    
    
    
    // Update vertices
    updateVerticesFromQuads(quads, vertices);
    
}

void TextRenderer::updateVerticesWithAllCharacters()
{
    const int quadVertexCount = 6;
    const std::string::size_type vertexCount = quadVertexCount;
    
    if (vertices.size() != vertexCount) {
        vertices.resize(vertexCount);
    }
    
    float s0 = 0, s1 = 1;
    float t0 = 0, t1 = 1;
    // top-left
    float x0 = -1;
    float y0 = 0.8;
    // bottom-right
    float x1 = 1;
    float y1 = -0.8;
    
    int vertexIndex = 0;
    utils::addQuadVertices({x0, y0, s0, t0, x1, y1, s1, t1}, vertices, &vertexIndex);
}

