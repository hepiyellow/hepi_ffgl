//
//  ShapeRenderer.cpp
//  HCloner
//
//  Created by Erez Makavy on 04/09/2021.
//

#include "ShapeRenderer.h"

using namespace ffglex;
using namespace ffglqs;

#include "../../h_common/Utils.h"

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

static const char _fragmentShaderCode[] = R"(#version 410 core

in vec2 uv;
out vec4 fragColor;

void main()
{
    fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
)";

bool ShapeRenderer::InitGL() {
    return BaseRenderer::InitGL(_vertexShaderCode, _fragmentShaderCode);
}

void ShapeRenderer::updateVertices() {
    const int quadVertexCount = 6;
    const std::string::size_type vertexCount = quadVertexCount;
    
    if (vertices.size() != vertexCount) {
        vertices.resize(vertexCount);
    }
    
    int index = 0;
    utils::addQuadVertices({
        -0.5,0.5,0,1,
        0.5,-0.5,1,0
    }, vertices, &index);
}
