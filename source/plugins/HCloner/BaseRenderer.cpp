//
//  BaseRenderer.cpp
//  HCloner
//
//  Created by Erez Makavy on 04/09/2021.
//

#include "BaseRenderer.h"

using namespace ffglex;
using namespace ffglqs;

#include "../../h_common/Utils.h"

BaseRenderer::BaseRenderer():
    vaoID( 0 ),
    vboID( 0 ) {
};

BaseRenderer::~BaseRenderer() {
    //If any of these assertions hit you forgot to release gl resources.
    assert( vaoID == 0 );
    assert( vboID == 0 );
}

bool BaseRenderer::InitGL(const char *vertexShaderCode, const char *fragmentShaderCode) {
    if( !shader.Compile( vertexShaderCode, fragmentShaderCode ) )
    {
        return false;
    }
    
    if (!InitializeResources()) {
        return false;
    }
    
    return true;
}

bool BaseRenderer::InitializeResources() {
    glGenVertexArrays( 1, &vaoID );
    glGenBuffers( 1, &vboID );
    if( vaoID == 0 || vboID == 0 )
    {
        ReleaseResources();
        return false;
    }
    return true;
}

void BaseRenderer::ReleaseResources()
{
    glDeleteBuffers( 1, &vboID );
    vboID = 0;
    glDeleteVertexArrays( 1, &vaoID );
    vaoID = 0;
}

void BaseRenderer::draw() {
    glUseProgram( shader.GetGLID() );
    glActiveTexture( GL_TEXTURE0 );
    
    glBindTexture(GL_TEXTURE_2D, texID);
    // Populate Vertex data
    glBindVertexArray(vaoID);
    
    // Update Vertex Buffer
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    updateVertices();
    glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), static_cast<GLvoid*>(& vertices[0]), GL_STATIC_DRAW );
    
    // Configure Vertesx Array Attributes
    glEnableVertexAttribArray( 0 );
    // xyz as 3rd, 4th and 5th floats
    glVertexAttribPointer( 0, 3, GL_FLOAT, false, sizeof( TEXTURED_QUAD_VERTICES[ 0 ] ), (char*)NULL + 2 * sizeof( float ) );
    // Texture coordinates (u,v) as 1st and 2nd floats
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 1, 2, GL_FLOAT, false, sizeof( TEXTURED_QUAD_VERTICES[ 0 ] ), (char*)NULL + 0 * sizeof( float ) );

    
    // Draw
    glDrawArrays( GL_TRIANGLES, 0, static_cast<int>(vertices.size()) );
    
    // Unbind Vertex Data
    glBindVertexArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    
    // Unbind Texture Data
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture( GL_TEXTURE0 ); // Is this redundant? (it's what the ScopedSamplerActivation does)
//    
    // Unbind Shader
    glUseProgram( 0 );
}


