#include "RowLayout.h"

static const char _vertexShaderCode[] = R"(#version 410 core
layout( location = 0 ) in vec4 vPosition;
layout( location = 1 ) in vec2 vUV;

out vec2 uv;

void main()
{
    gl_Position = vPosition;
    uv = vUV;

}
)";

// Geometry shader goes here !

static const char _fragmentShaderCode[] = R"(#version 410 core
uniform sampler2D InputTexture;

in vec2 uv;
out vec4 fragColor;

void main()
{
    vec4 color = texture( InputTexture, uv );
    fragColor = vec4(1.0, 1.0, 1.0, color.r);
}
)";

bool RowLayout::InitGL(const FFGLViewportStruct *vp) {
    if( !shader.Compile( _vertexShaderCode, _fragmentShaderCode ) )
    {
        return false;
    }
    return BaseLayout::InitGL(vp);
}

void RowLayout::setUniforms(TextParams params, u16string textToDraw) {
    shader.Set( "inputTexture", 0 );    
}

void RowLayout::updateVertices(TextParams params, u16string text, FontRasterizer &fr, vector<GlVertexTextured> &vertices) {
    
    // I want the default to be that the letter 'a' takes 1/4 of the screen height;
    // When params.size == 1 then the letter A's height would take 1/2 a screen height.
    float appliedScale = params.size * (1.0f / fr.getLetterAHeight());
    
    vector<TexturedQuadPoints> quads;
    float x = 0;
    float y = 0;
    float max_x = 0; // width
    float max_y = 0; // height
    for (std::string::size_type charIndex = 0; charIndex < text.length(); charIndex++) {
        const char16_t characterValue = text[charIndex];
        
        
        // 01 - Get Char Quad
        TexturedQuadPoints tq;
        fr.getCharacterQuad(characterValue, &x, &y, tq);
        
        // 02 Apply Scale
        tq.x0 *= appliedScale;
        tq.y0 *= appliedScale;
        tq.x1 *= appliedScale;
        tq.y1 *= appliedScale;
        
        // 02 - Apply Spacing
        if (charIndex != 0) {
            const float offset = tq.x0 * (params.spacing_x - 1);
            tq.x0 += offset;
            tq.x1 += offset;
        }
        
        // 03 - Measure total width & height for alignment
        if (tq.x1 > max_x) {
            max_x = tq.x1;
        }
        
        if (tq.y1 > max_y) {
            max_y = tq.y1;
        }
        
        quads.push_back(tq);
    }
    
    // Global transforms
    float align_offset_x = 0;
    switch (params.align_h) {
        case AlignmentHorizontal::Left:
            align_offset_x = 0;
            break;
        case AlignmentHorizontal::Center:
            align_offset_x = -max_x / 2;
            break;
        case AlignmentHorizontal::Right:
            align_offset_x = -max_x;
            break;
    }
    
    float align_offset_y = 0;
    switch (params.align_v) {
        case AlignmentVertical::Top:
            align_offset_y = 0;
            break;
        case AlignmentVertical::Center:
            align_offset_y = -max_y / 2;
            break;
        case AlignmentVertical::Bottom:
            align_offset_y = -max_y;
            break;
    }
    
    for(TexturedQuadPoints &quad: quads) {
        float offset_x = align_offset_x + params.pos_x;
        quad.x0 += offset_x;
        quad.x1 += offset_x;
        float offset_y = align_offset_y + params.pos_y;
        quad.y0 += offset_y;
        quad.y1 += offset_y;
    }
    
    // Update vertices
    updateVerticesFromQuads(quads, vertices);

}


