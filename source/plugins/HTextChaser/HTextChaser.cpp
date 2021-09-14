#include "HTextChaser.h"
#include <math.h>//floor
#include "../../h_common/TextRenderer.h"
#include "../../h_common/Utils.h"

using namespace ffglex;
using namespace ffglqs;
using namespace std;
using namespace tr;
using namespace utils;

static CFFGLPluginInfo PluginInfo(
	PluginFactory< HTextChaser >,// Create method
	"EM02",                        // Plugin unique ID
	"Text Chaser Pro",            // Plugin name
	2,                             // API major version number
	1,                             // API minor version number
	1,                             // Plugin major version number
	000,                           // Plugin minor version number
	FF_SOURCE,                     // Plugin type
	"HEPi's Text Chaser FFGL",// Plugin description
	"HEPi 2021"        // About
);

HTextChaser::HTextChaser()
{
    log("HTextChaser()");
    
	// Input properties
	SetMinInputs( 0 );
	SetMaxInputs( 0 );
    
    initFontOptions();

    PID_TEXT            = AddParam( ParamText::create( "Text", "ABC" ) );
    PID_FONT_FILE_NAME  = AddParam( ParamOption::Create( "Font", fontOptions, 0 ) );
    PID_LAYOUT          = AddParam( ParamOption::Create( "Layout", {{"Rows", 0}, {"Circle", 1}}, 0 ) );
    PID_SIZE            = AddParam( ParamRange::Create( "Size", 1, ParamRange::Range(0, 10) ) );
    PID_RADIUS          = AddParam( ParamRange::Create( "Radius", 0.5, ParamRange::Range(0, 4) ) );
    PID_POS_MULT        = AddParam( ParamOption::Create( "Position Mult",
                                {{"1/4", 0.25}, {"1/2", 0.5}, {"x1", 1}, {"x2", 2}, {"x4", 4}}, 2 ) );
    PID_POS_X           = AddParam( ParamRange::Create( "Position X", 0, ParamRange::Range(-1,1) ) );
    PID_POS_Y           = AddParam( ParamRange::Create( "Position Y", 0, ParamRange::Range(-1,1) ) );
    PID_ALIGN_H         = AddParam( ParamOption::Create( "Alignment H",
                                {{"Left", 0}, {"Center", 1}, {"Right", 2}}, 1 ) );
    PID_ALIGN_V         = AddParam( ParamOption::Create( "Alignment V",
                                {{"Top", 0}, {"Center", 1}, {"Bottom", 2}}, 1 ) );
    PID_LINE_WIDTH      = AddParam( Param::Create( "Line Width", 1 ) );
    PID_SPACING_X_MODE  = AddParam( ParamOption::Create( "Spacing X Mode",
                                {{"Manual", 0}, {"AutoFit", 1}}, 0 ) );
    PID_SPACING_X       = AddParam( ParamRange::Create( "Spacing X", 1, ParamRange::Range(0, 10) ) );
    PID_ROTATION        = AddParam( Param::Create( "Rotation", 0 ) );
    PID_CHAR_ROTATION   =  AddParam( Param::Create( "C.Rotation", 0 ) );
    PID_CHAR_ROTATION_FAN = AddParam( ParamRange::Create( "C.Rotation Fan", 0, ParamRange::Range(-1,1) ) );
    PID_CHAR_ROTATE_TOGETHER = AddParam( ParamBool::Create( "Concentric", 0) );
    PID_RANGE = AddParam( ParamOption::Create( "Range", {{"Characters", 0}, {"Words", 1}}, 1 ) );
    PID_LENGTH_NORM = AddParam( Param::Create( "Length", 1 ) );
    PID_OFFSET_NORM = AddParam( Param::Create( "Offset", 0 ) );
    
	FFGLLog::LogToHost( "Created Text Chaser Pro" );
    
}

HTextChaser::~HTextChaser() {
    // nothing yet
    log("~HTextChaser()");
}

FFResult HTextChaser::InitGL( const FFGLViewportStruct* vp )
{
	if(!textRenderer.InitGL(vp)) {
        FFGLLog::LogToHost( "HTextChaser::InitGL(): Failed" );
		DeInitGL();
		return FF_FAIL;
	}
    log("HTextChaser::InitGL(): text = " + GetParamText(PID_TEXT)->text);
	//Use base-class init as success result so that it retains the viewport.
    
	return CFFGLPlugin::InitGL( vp );
}

FFResult HTextChaser::DeInitGL()
{
    return textRenderer.DeInitGL();
}


void HTextChaser::initFontOptions() {
    fontManager.initFonts();
    
    fontOptions.clear();
    int i = 0;
    for (const FontMeta &font : fontManager.getFonts()) {
        fontOptions.push_back({ font.displayName, (float)font.id});
            i++;
    }
}

string HTextChaser::getCurrentFontPath() {
    int fontIndex= (int) GetParamOption(PID_FONT_FILE_NAME)->GetRealValue();
    if (fontIndex > fontManager.getFonts().size() - 1) {
        GetParamOption(PID_FONT_FILE_NAME)->SetValue(0);
        // TODO: trigger event to update Host UI. Need to update Reoslume/FFGL sdk version first
        
        return fontManager.getFonts()[0].path; // Default Font
    } else {
        return fontManager.getFonts()[fontIndex].path;
    }
}


FFResult HTextChaser::ProcessOpenGL( ProcessOpenGLStruct* pGL )
{
    float posMult = GetParamOption(PID_POS_MULT)->GetRealValue();
    
    TextParams p;
    p.text = toUtf16(GetParamText(PID_TEXT)->text);
    p.layout = static_cast<Layout>(GetParamOption(PID_LAYOUT)->GetRealValue());
    p.size = GetParam(PID_SIZE)->GetValue();
    p.pos_x = GetParam(PID_POS_X)->GetValue() * posMult;
    p.pos_y = GetParam(PID_POS_Y)->GetValue() * posMult;
    p.align_h = static_cast<AlignmentHorizontal>(GetParamOption(PID_ALIGN_H)->GetRealValue());
    p.align_v = static_cast<AlignmentVertical>(GetParamOption(PID_ALIGN_V)->GetRealValue());
    
    // Chaser
    p.range = static_cast<Range>(GetParamOption(PID_RANGE)->GetRealValue());
    p.length = GetParam(PID_LENGTH_NORM)->GetValue();
    p.offset = GetParam(PID_OFFSET_NORM)->GetValue();
    
    p.lineWidth = GetParam(PID_LINE_WIDTH)->GetValue();
    p.spacing_x = GetParam(PID_SPACING_X)->GetValue();
    p.spacing_x_mode = static_cast<SpacingMode>(GetParamOption(PID_SPACING_X_MODE)->GetRealValue());
    
    // Circle Only
    p.radius = GetParam(PID_RADIUS)->GetValue();
    p.rotation = GetParam(PID_ROTATION)->GetValue();
    p.charRotation = GetParam(PID_CHAR_ROTATION)->GetValue();
    p.charRotationFan = GetParam(PID_CHAR_ROTATION_FAN)->GetValue();
    p.rotateTogether = GetParam(PID_CHAR_ROTATE_TOGETHER)->GetValue() > 0;
    
    textRenderer.updateFontTextureIfNeeded(getCurrentFontPath(), p.text);
    textRenderer.draw(p);
    
    
    //updateParamVisibility
    SetParamVisibility(PID_RADIUS, p.layout == Layout::Circle, true);
    SetParamVisibility(PID_ROTATION, p.layout == Layout::Circle, true);
    SetParamVisibility(PID_CHAR_ROTATE_TOGETHER, p.layout == Layout::Circle, true);
    
	return FF_SUCCESS;
}

