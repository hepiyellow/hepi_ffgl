#include "BaseLayout.h"


bool BaseLayout::InitGL(const FFGLViewportStruct* vp) {
    viewportAspect =  (float)vp->height / (float)vp->width;
    log("BaseLayout::InitGL(): vp->width=" + to_string(vp->width) + ", vp->height=" + to_string(vp->height));
    return true;
}

void BaseLayout::DeInitGL() {
    shader.FreeGLResources();
}
