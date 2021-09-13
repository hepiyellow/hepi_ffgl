#include "CircleLayout.h"
#include "circleShaders.h"

bool CircleLayout::InitGL() {
    if( !shader.Compile( _circleVertexShaderCode, _circleGeometryShaderCode, _circleFragmentShaderCode ) ) {
        return false;
    }
    return true;
}

