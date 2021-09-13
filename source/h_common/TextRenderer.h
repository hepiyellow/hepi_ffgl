#pragma once

#include <stdio.h>
#include <map>
#include "stb_truetype.h"
#include <FFGLSDK.h>
#include "FontRasterizer.h"
#include "Utils.h"
#include "RowLayout.h"
#include "CircleLayout.h"
#include "TextRendererTypes.h"

using namespace utils;
using namespace ffglex;
using namespace tr;

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();
    
    bool InitGL(const FFGLViewportStruct* vp);
    bool DeInitGL();
    
    /**
        This should be called before calling the draw() method. If the font or text had NOT changes since the last call, then no new Texture would be created.
     */
    void updateFontTextureIfNeeded(string fontPath, u16string text);
    /**
        Draw the given text. Assumes that allcharacters in the text exist in the current Font-Texture (which was created by updateFontTexture() ).
     */
    void draw(TextParams &params);
    
private:
    RowLayout rowLayout;
    CircleLayout circleLayout;
    
    // OpenGL
    
    ffglex::FFGLShader circleShader;
    GLuint vaoID;
    GLuint vboID;
    
    GLint viewportAspectLocation;

    // Viewport
    float viewportAspect = 1; // For 2D rotations
    
    // Text Manipulation
    u16string getTextToDraw(TextParams &params);
        
    // Text CPU Rendering
    FontRasterizer fontRasterizer;
    std::u16string lastText;
    std::string lastFont = "";
    
    bool InitializeResources(std::string fontPath);
    
    // Vertex Stuff
    std::vector<GlVertexTextured> vertices;
    
    void updateVerticesRowsLayout(TextParams &params, u16string text);
    void updateVerticesCircleLayout(TextParams &params, u16string text);
    void updateVerticesWithAllCharacters();
    
    TexturedQuadPoints getCharacterQuad(char16_t characterValue, float *x, float *y);
};

