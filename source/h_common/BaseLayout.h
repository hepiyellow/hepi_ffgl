#pragma once

#include <FFGLSDK.h>

using namespace ffglex;

class BaseLayout {
public:
    virtual bool InitGL() = 0;
    virtual void DeInitGL();
    
    FFGLShader shader;
};
