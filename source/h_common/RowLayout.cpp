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

bool RowLayout::InitGL() {
    if( !shader.Compile( _vertexShaderCode, _fragmentShaderCode ) )
    {
        return false;
    }
    return true;
}

