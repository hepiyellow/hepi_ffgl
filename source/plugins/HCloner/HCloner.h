#pragma once
#include <string>
#include <FFGLSDK.h>
#include "../../h_common/FontManager.h"
#include "../../h_common/TextRenderer.h"
#include "../../h_common/HPlugin.h"
//#include "ShapeRenderer.h"

using namespace ffglex;

/**
 Y positive value is towards the top.
 X positive value is towards the right
 */
struct QuadPoints {
    float x0, y0; // bottom-left
    float x1, y1; // top-right
};

class HCloner : public HPlugin
{
public:
	HCloner();
	~HCloner();

	//CFFGLPlugin
	FFResult InitGL( const FFGLViewportStruct* vp ) override;
	FFResult ProcessOpenGL( ProcessOpenGLStruct* pGL ) override;
	FFResult DeInitGL() override;

private:
	ffglex::FFGLShader shader;//!< Utility to help us compile and link some shaders into a program.

	GLuint vaoID;
	GLuint vboID;

	bool InitializeResources();
	void ReleaseResources();
     
    // Vertex Stuff
    QuadPoints getCloneQuadPoints(int cloneNumber);
    void updateVertices();
    FFResult DrawClones(ProcessOpenGLStruct* pGL);
    void DrawGuide(ProcessOpenGLStruct* pGL);
    
    std::vector<GlVertexTextured> vertices;
    
    // Params
    const int numOfClones = 4;
    float getParamValue(std::string paramName);
    std::string addClonePrefix(std::string paramName, int cloneNumber);
    
    // Sub Renderers
    FontManager fontManager;
    tr::TextRenderer textRenderer;
//    ShapeRenderer shapeRenderer;
};
