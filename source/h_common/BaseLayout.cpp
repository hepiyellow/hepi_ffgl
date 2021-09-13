#include "BaseLayout.h"


bool BaseLayout::InitGL(const FFGLViewportStruct* vp) {
    viewportAspect =  (float)vp->height / (float)vp->width;
    return true;
}

void BaseLayout::DeInitGL() {
    shader.FreeGLResources();
}
