#pragma once

#include <string>
#include <vector>
#include <FFGLSDK.h>
#include "TextRendererTypes.h"
#include "FontRasterizer.h"

using namespace ffglex;
using namespace tr;

class BaseLayout {
public:
    virtual bool InitGL(const FFGLViewportStruct* vp) = 0;
    virtual void DeInitGL();
    virtual void setUniforms(TextParams params, u16string textToDraw) = 0;
    virtual void updateVertices(TextParams params, u16string textToDraw, FontRasterizer &fr, vector<GlVertexTextured> &vertices) = 0;

    // Members
    float viewportAspect;
    
    
    // GL Resources
    FFGLShader shader;
    
};
