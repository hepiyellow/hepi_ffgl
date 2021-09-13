#pragma once

#include <string>
#include <FFGLSDK.h>
#include "TextRendererTypes.h"

using namespace ffglex;
using namespace tr;

class BaseLayout {
public:
    virtual bool InitGL(const FFGLViewportStruct* vp) = 0;
    virtual void DeInitGL();
    virtual void setUniforms(TextParams params, u16string textToDraw) = 0;
    virtual void updateVertices(TextParams params, u16string textToDraw) = 0;
    
    FFGLShader shader;
    float viewportAspect;
};
