#include "CircleLayout.h"
#include "circleShaders.h"

bool CircleLayout::InitGL(const FFGLViewportStruct *vp) {
    if( !shader.Compile( _circleVertexShaderCode, _circleGeometryShaderCode, _circleFragmentShaderCode ) ) {
        return false;
    }
    return BaseLayout::InitGL(vp);
}

void CircleLayout::setUniforms(TextParams params, u16string textToDraw) {
    shader.Set( "inputTexture", 0 );
    shader.Set( "viewportAspect", viewportAspect );
    shader.Set( "textLength", (int) textToDraw.length() );
    shader.Set( "radius", params.radius );
    shader.Set( "rotation", params.rotation );
    shader.Set( "charRotation", params.charRotation );
    shader.Set( "charRotationFan", params.charRotationFan );
    shader.Set( "rotateTogether", params.rotateTogether ? (float) 1.0 : (float) 0.0 );
    shader.Set( "globalPosOffset", params.pos_x, params.pos_y );
}

void CircleLayout::updateVertices(TextParams params, u16string text, FontRasterizer &fr, vector<GlVertexTextured> &vertices) {
        
    // I want the default to be that the letter 'a' takes 1/4 of the screen height;
    // When params.size == 1 then the letter A's height would take 1/2 a screen height.
    float appliedScale = params.size * (1.0f / fr.getLetterAHeight());
    
    vector<TexturedQuadPoints> quads;
    float x = 0;
    float y = 0;
   
    for (std::string::size_type charIndex = 0; charIndex < text.length(); charIndex++) {
        const char16_t characterValue = text[charIndex];
        
        
        // 01 - Get Char Quad
        x = 0;
        y = 0;
        TexturedQuadPoints q;
        fr.getCharacterQuad(characterValue, &x, &y, q);
        
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
