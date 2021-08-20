//
//  BaseRenderer.hpp
//  HCloner
//
//  Created by Erez Makavy on 04/09/2021.
//

#ifndef BaseRenderer_h
#define BaseRenderer_h

#include <FFGLSDK.h>
#endif /* BaseRenderer_h */

class BaseRenderer {
public:
    BaseRenderer();
    ~BaseRenderer();
    
    bool InitGL(const char *vertexShaderCode, const char *fragmentShaderCode);
    void ReleaseResources();
    
    void draw();
    
protected:
    virtual void updateVertices() = 0;
    std::vector<ffglex::GlVertexTextured> vertices;
    
private:
    ffglex::FFGLShader shader;

    GLuint vaoID;
    GLuint vboID;
    GLuint texID = 0;

    bool InitializeResources();
};
