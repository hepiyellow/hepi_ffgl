#pragma once
#include <FFGLSDK.h>
#include "../../h_common/TextRenderer.h"
#include "../../h_common/FontManager.h"
#include "../../h_common/HPlugin.h"

using namespace ffglqs;
using namespace tr;


class HTextChaser : public HPlugin
{
public:
	HTextChaser();
    ~HTextChaser();

	//CFFGLPlugin
	FFResult InitGL( const FFGLViewportStruct* vp ) override;
	FFResult ProcessOpenGL( ProcessOpenGLStruct* pGL ) override;
	FFResult DeInitGL() override;

private:
    // State
    vector<ParamOption::Option> fontOptions;
    string lastFontPath;

    // Resouces
    TextRenderer textRenderer;
    FontManager fontManager;
    
    
    // Methods
    void initFontOptions();
    string getCurrentFontPath();
    
    // Param IDs
    unsigned int PID_TEXT;
    unsigned int PID_FONT_FILE_NAME;
    unsigned int PID_LAYOUT;
    unsigned int PID_SIZE;
    unsigned int PID_LINE_WIDTH;
    unsigned int PID_POS_MULT;
    unsigned int PID_POS_X;
    unsigned int PID_POS_Y;
    unsigned int PID_ALIGN_H;
    unsigned int PID_ALIGN_V;
    unsigned int PID_SPACING_X_MODE;
    unsigned int PID_SPACING_X;
    
    unsigned int PID_RADIUS;
    unsigned int PID_ROTATION;
    unsigned int PID_CHAR_ROTATION;
    unsigned int PID_CHAR_ROTATION_FAN;
    unsigned int PID_CHAR_ROTATE_TOGETHER;
    
    unsigned int PID_RANGE;
    unsigned int PID_LENGTH_NORM;
    unsigned int PID_OFFSET_NORM;

};
