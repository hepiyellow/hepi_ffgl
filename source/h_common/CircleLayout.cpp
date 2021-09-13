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

void CircleLayout::updateVertices(TextParams params, u16string textToDraw) {
   
}
