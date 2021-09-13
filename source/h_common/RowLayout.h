#pragma once

#include <FFGLSDK.h>
#include "BaseLayout.h"

using namespace ffglex;

class RowLayout : public BaseLayout {
public:
    bool InitGL() override;
    void DeInitGL() override;
    FFGLShader shader;
};
