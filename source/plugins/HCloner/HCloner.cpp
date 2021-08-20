#include "HCloner.h"
#include "../../h_common/Utils.h"
using namespace ffglex;
using namespace ffglqs;
using namespace std;
using namespace utils;

static CFFGLPluginInfo PluginInfo(
	PluginFactory< HCloner >,// Create method
	"RE01",                      // Plugin unique ID of maximum length 4.
	"HCloner",            // Plugin name
	2,                           // API major version number
	1,                           // API minor version number
	1,                           // Plugin major version number
	0,                           // Plugin minor version number
	FF_EFFECT,                   // Plugin type
	"HEPi Cloner",  // Plugin description
	"By HEPi"      // About
);

static const char _vertexShaderCode[] = R"(#version 410 core
uniform vec2 MaxUV;
uniform vec2 offset;

layout( location = 0 ) in vec4 vPosition;
layout( location = 1 ) in vec2 vUV;

out vec2 uv;

void main()
{
	gl_Position = vPosition;
	uv = vUV * MaxUV;
}
)";

static const char _fragmentShaderCode[] = R"(#version 410 core
uniform sampler2D InputTexture;

in vec2 uv;

out vec4 fragColor;

void main()
{
	vec4 color = texture( InputTexture, uv );
	fragColor = color;
}
)";


const string PN_XPOS = "Position X";
const string PN_YPOS = "Position Y";
const string PN_SCALE = "Scale";
const string PN_WSCALE = "Scale W";
const string PN_HSCALE = "Scale H";
const string PN_SHOW_GUIDE = "Show Guide";

string HCloner::addClonePrefix(string paramName, int cloneNumber) {
    return to_string(cloneNumber + 1) + " " + paramName;
}

HCloner::HCloner() :
    vaoID( 0 ),
    vboID( 0 )
{
	// Input properties
	SetMinInputs( 1 );
	SetMaxInputs( 1 );

    Plugin::AddParam( ParamBool::Create( PN_SHOW_GUIDE, false ) );
    
    for (int c=0;c < numOfClones ; c++) {
        string groupName =  "Clone "+ to_string(c+1);
        AddParam( ParamRange::Create( addClonePrefix(PN_XPOS,c), 0, ParamRange::Range(-1, 1) ), groupName);
        AddParam( ParamRange::Create( addClonePrefix(PN_YPOS,c), 0, ParamRange::Range(-1, 1) ), groupName );
        AddParam( ParamRange::Create( addClonePrefix(PN_SCALE,c), 1, ParamRange::Range(0, 10) ), groupName );
        AddParam( ParamRange::Create( addClonePrefix(PN_WSCALE,c), 1, ParamRange::Range(0, 10) ), groupName );
        AddParam( ParamRange::Create( addClonePrefix(PN_HSCALE,c), 1, ParamRange::Range(0, 10) ), groupName );
    }
    
	FFGLLog::LogToHost( "Created HCloner effect" );
}

HCloner::~HCloner()
{
    //If any of these assertions hit you forgot to release gl resources.
    assert( vaoID == 0 );
    assert( vboID == 0 );
}

FFResult HCloner::InitGL( const FFGLViewportStruct* vp )
{
	if( !shader.Compile( _vertexShaderCode, _fragmentShaderCode ) )
	{
		DeInitGL();
		return FF_FAIL;
	}
    
    if (!InitializeResources()) {
        DeInitGL();
        return FF_FAIL;
    }

//    if (!shapeRenderer.InitGL()) {
//        DeInitGL();
//        return FF_FAIL;
//    }
    
    if (!textRenderer.InitGL()) {
        DeInitGL();
        return FF_FAIL;
    };
    fontManager.initFonts();
    textRenderer.updateFontTextureIfNeeded(fontManager.getDefaultFont().path, toUtf16("aA1234"));
    log("HCloner::InitGL(): Done");
	//Use base-class init as success result so that it retains the viewport.
	return CFFGLPlugin::InitGL( vp );
}

bool HCloner::InitializeResources()
{
    glGenVertexArrays( 1, &vaoID );
    glGenBuffers( 1, &vboID );
    if( vaoID == 0 || vboID == 0 )
    {
        ReleaseResources();
        return false;
    }
    
    return true;
}

void HCloner::ReleaseResources()
{
    glDeleteBuffers( 1, &vboID );
    vboID = 0;
    glDeleteVertexArrays( 1, &vaoID );
    vaoID = 0;
    
    
}

FFResult HCloner::ProcessOpenGL( ProcessOpenGLStruct* pGL )
{
    if (DrawClones(pGL) == FF_FAIL) {
        return FF_FAIL;
    }
    if (Plugin::GetParam(PN_SHOW_GUIDE)->GetValue()) {
        DrawGuide(pGL);
    }
//    else {
//    }
    
    
	return FF_SUCCESS;
}

FFResult HCloner::DrawClones(ProcessOpenGLStruct* pGL) {
    if( pGL->numInputTextures < 1 )
        return FF_FAIL;

    if( pGL->inputTextures[ 0 ] == NULL )
        return FF_FAIL;

    //FFGL requires us to leave the context in a default state on return, so use this scoped binding to help us do that.
    ScopedShaderBinding shaderBinding( shader.GetGLID() );
    //The shader's sampler is always bound to sampler index 0 so that's where we need to bind the texture.
    //Again, we're using the scoped bindings to help us keep the context in a default state.
    ScopedSamplerActivation activateSampler( 0 );
    Scoped2DTextureBinding textureBinding( pGL->inputTextures[ 0 ]->Handle );

    shader.Set( "inputTexture", 0 );

    //The input texture's dimension might change each frame and so might the content area.
    //We're adopting the texture's maxUV using a uniform because that way we dont have to update our vertex buffer each frame.
    FFGLTexCoords maxCoords = GetMaxGLTexCoords( *pGL->inputTextures[ 0 ] );
    shader.Set( "MaxUV", maxCoords.s, maxCoords.t );

    SendParams( shader );
    
    
    
    // Draw
    if( vaoID == 0 || vboID == 0 )
        return FF_FAIL;
    
    glBindVertexArray(vaoID);
    
    // Update Vertex Buffer
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    updateVertices();
    glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), static_cast<GLvoid*>(& vertices[0]), GL_DYNAMIC_DRAW );
    
    // Configure Vertesx Array Attributes
    glEnableVertexAttribArray( 0 );
    // xyz as 3rd, 4th and 5th floats
    glVertexAttribPointer( 0, 3, GL_FLOAT, false, sizeof( TEXTURED_QUAD_VERTICES[ 0 ] ), (char*)NULL + 2 * sizeof( float ) );
    // Texture coordinates (u,v) as 1st and 2nd floats
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 1, 2, GL_FLOAT, false, sizeof( TEXTURED_QUAD_VERTICES[ 0 ] ), (char*)NULL + 0 * sizeof( float ) );

    
    
    // Draw
    glDrawArrays( GL_TRIANGLES, 0, static_cast<int>(vertices.size()) );
    
    // Unbind
    glBindVertexArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    return FF_SUCCESS;
}

void HCloner::DrawGuide(ProcessOpenGLStruct* pGL) {

    //    shapeRenderer.draw();
//    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    tr::TextParams p;
    p.length = 1;
    for (int cloneIndex=0; cloneIndex < numOfClones; cloneIndex++) {
        QuadPoints q = getCloneQuadPoints(cloneIndex);
        p.text = toUtf16(to_string(cloneIndex + 1));
        float height = q.y1 - q.y0;
        p.pos_x = q.x0 + (q.x1 - q.x0) / 2;
        p.pos_y = q.y0 + height / 2;
        p.size = height / 2;
        p.length = 1;
        
        textRenderer.draw(p);
    }
    glBlendFunc(GL_ONE, GL_ZERO);
    glDisable(GL_BLEND);
}

float HCloner::getParamValue(string paramName)
{
    int i=0;
    while (i < params.size()) {
        if (params[i]->GetName() == paramName) {
            return params[i]->GetValue();
        }
        i++;
    }
    return 0;
}

FFResult HCloner::DeInitGL()
{
    shader.FreeGLResources();
    ReleaseResources();
    textRenderer.DeInitGL();

    return FF_SUCCESS;
}

QuadPoints HCloner::getCloneQuadPoints(int cloneNumber) {
    const float scale = getParamValue(addClonePrefix(PN_SCALE, cloneNumber));
    const float scaleW = getParamValue(addClonePrefix(PN_WSCALE, cloneNumber));
    const float scaleH = getParamValue(addClonePrefix(PN_HSCALE, cloneNumber));
    
    return {
        // bottom-left
        (-1.0f * scaleW * scale) + getParamValue(addClonePrefix(PN_XPOS, cloneNumber)), // x0
        (-1.0f * scaleH * scale) + getParamValue(addClonePrefix(PN_YPOS, cloneNumber)), // y0
        // top-right
        (1.0f  * scaleW * scale) + getParamValue(addClonePrefix(PN_XPOS, cloneNumber)), // x1
        (1.0f  * scaleH * scale) + getParamValue(addClonePrefix(PN_YPOS, cloneNumber)) // y1
    };
}

void HCloner::updateVertices()
{
    const int quadVertexCount = 6;
    const int vertexCount = quadVertexCount * numOfClones;
    
    if (vertices.size() != vertexCount) {
        vertices.resize(vertexCount);
    }
    
    int vertexIndex = 0;
    for (int c=0; c < numOfClones; c++) {
        QuadPoints q = getCloneQuadPoints(c);
        utils::addQuadVertices(
            {
                q.x0, q.y0, 0.0f, 0.0f, // bottom-left
                q.x1, q.y1, 1.0f, 1.0f // top-right
            },
            vertices,
            &vertexIndex
       );
    }
}

