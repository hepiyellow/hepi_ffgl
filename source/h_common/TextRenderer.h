//
//  TextRenderer.h
//
//  Created by Erez Makavy on 31/08/2021.
//

#ifndef TextRenderer_h
#define TextRenderer_h

#include <stdio.h>
#include <map>
#include "stb_truetype.h"
#include <FFGLSDK.h>
#include "FontRasterizer.h"
#include "Utils.h"

using namespace utils;

namespace tr {

enum class AlignmentHorizontal {
    Left, Center, Right
};

enum class SpacingMode {
    Manual, AutoFit
};

enum class AlignmentVertical {
    Top, Center, Bottom
};

enum class Range {
    Characters,
    Words,
    Lines
};

enum class Layout {
    Rows,
    Circle,
};



struct TextParams {
    std::u16string text = utils::toUtf16("");
    float size = 1; // Size of 1 equals to 1/4 of the screen height
    float pos_x = 0;
    float pos_y = 0;
    AlignmentHorizontal align_h = AlignmentHorizontal::Center;
    AlignmentVertical align_v = AlignmentVertical::Center;
    float radius = 0.5;
    float rotation = 0;
    float charRotation = 0;
    float charRotationFan = 0;
    bool rotateTogether = true;
    float spacing_x = 1;
    SpacingMode spacing_x_mode = SpacingMode::Manual;
    Range range = Range::Characters;
    float length = 0;
    float offset = 0;
    Layout layout = Layout::Rows;
};

const int MAX_UNIQ_CHARACTERS = 100;

using namespace ffglex;

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
    // OpenGL
    ffglex::FFGLShader shader;
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

}
#endif /* TextRenderer_h */
