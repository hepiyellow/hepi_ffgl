#pragma once
static const char _circleVertexShaderCode[] = R"(#version 410 core
layout( location = 0 ) in vec4 vPosition;
layout( location = 1 ) in vec2 vUV;

//in int gl_VertexID ;

out VS_OUT
{
    vec2 v_TexCoord;
    uint cIndex;
} vs_out;

void main()
{
    gl_Position = vPosition;
    vs_out.v_TexCoord = vUV;
    vs_out.cIndex = gl_VertexID / 6;

}
)";

static const char _circleGeometryShaderCode[] = R"(#version 410 core
#line 5
#define M_PI 3.1415926535897932384626433832795
#define M_2PI 6.283185307179586
layout( triangles_adjacency ) in;
layout( triangle_strip, max_vertices = 4 ) out;

uniform float viewportAspect;
uniform float radius;
uniform float rotation;
uniform float charRotation;
uniform float charRotationFan;
uniform float rotateTogether;
uniform int textLength;
uniform vec2 globalPosOffset;

in VS_OUT
{
    vec2 v_TexCoord;
    uint cIndex;
} gs_in[];

out GS_OUT
{
    vec2 v_TexCoord;
} gs_out;

mat2 rotateMat(float a) {
    // See this: https://computergraphics.stackexchange.com/questions/9496/what-kind-of-transformation-when-the-aspect-is-changed-for-a-rotated-shape

    float s = sin(a);
    float c = cos(a);
    return mat2(c, -s/viewportAspect, s*viewportAspect, c);
}

vec4 transform(vec4 pos, float posAngle, float charAngle) {

    // Take only x and y
    vec2 nPos = vec2(pos.x, pos.y); // New Pos

    // Apply Character Rotation anchor is (0,0)
    nPos = rotateMat(charAngle + rotateTogether * posAngle) * nPos;

    // Translate to circle radius.
    nPos = nPos + rotateMat(posAngle) * vec2(0, radius);

    // Global translation
    nPos = nPos + globalPosOffset;

    return vec4(nPos.x, nPos.y, pos.z, 1);
}


void main()
{
    float cIndex = float(gs_in[0].cIndex); // character index
    float charAngle = M_2PI * (charRotation + charRotationFan * cIndex / textLength);
    float posAngle = M_2PI * (rotation  + cIndex / textLength);

    const int vIndice[4] = int[4](
        0, // Bottom-Left
        1, // Bottom-Right
        2, // Top-Left
        5  // Top-Right
    );

    for (int i=0; i < 4; i++) {
        int vi = vIndice[i];
        gl_Position = transform(gl_in[vi].gl_Position, posAngle, charAngle);
        gs_out.v_TexCoord = gs_in[vi].v_TexCoord;
        EmitVertex();
    }

    EndPrimitive();
}
)";

static const char _circleFragmentShaderCode[] = R"(#version 410 core
uniform sampler2D InputTexture;

in GS_OUT
{
    vec2 v_TexCoord;
};

out vec4 fragColor;

void main()
{
    vec4 color = texture( InputTexture, v_TexCoord );
    fragColor = vec4(1.0, 1.0, 1.0, color.r);
}
)";
