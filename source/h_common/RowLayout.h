#pragma once

#include <FFGLSDK.h>
#include "BaseLayout.h"
#include "TextRendererTypes.h"

using namespace ffglex;
using namespace tr;

class RowLayout : public BaseLayout {
public:
    bool InitGL(const FFGLViewportStruct* vp) override;
    void setUniforms(TextParams params, u16string textToDraw) override;
    void updateVertices(TextParams params, u16string textToDraw, FontRasterizer &fr, vector<GlVertexTextured> &vertices) override;
};
