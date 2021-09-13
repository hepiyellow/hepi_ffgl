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
using namespace std;


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
    
    // Text Manipulation
    u16string getTextToDraw(TextParams &params);
        
    // Text CPU Rendering
    FontRasterizer fontRasterizer;
    u16string lastText;
    string lastFont = "";
    
    vector<GlVertexTextured> vertices;
    
    bool InitializeResources(std::string fontPath);
    void updateVerticesWithAllCharacters();
};

