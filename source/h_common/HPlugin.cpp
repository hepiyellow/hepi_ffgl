//
//  HPlugin.cpp
//  HCloner
//
//  Created by Erez Makavy on 10/09/2021.
//

#include "HPlugin.h"
//#include <FFGLSDK.h>
using namespace ffglqs;

unsigned int HPlugin::AddParam( std::shared_ptr< Param > param ) {
    return AddParamBase(param);
}

unsigned int HPlugin::AddParam( std::shared_ptr< ParamRange > param ) {
    unsigned int id = AddParamBase(param);
    SetParamRange( id, param->GetRange().min, param->GetRange().max );
    return id;
}

unsigned int HPlugin::AddParam( std::shared_ptr< ParamOption > param ) {
    unsigned int new_index = (unsigned int)params.size();
    SetOptionParamInfo( new_index, param->GetName().c_str(), (unsigned int)param->options.size(), param->GetValue() );

    for( unsigned int i = 0; i < param->options.size(); i++ )
    {
        SetParamElementInfo( new_index, i, param->options[ i ].name.c_str(), (float)i );
    }
    params.push_back( param );
    
    return new_index;
}


unsigned int HPlugin::AddParam( std::shared_ptr< ParamRange > param, string groupName)
{
    unsigned int id = AddParam(param);
    SetParamGroup(id, groupName);
    return id;
}

unsigned int HPlugin::AddParamBase( std::shared_ptr< Param > param ) {
    unsigned int new_index = (unsigned int)params.size();
    SetParamInfo( new_index, param->GetName().c_str(), param->GetType(), param->GetValue() );
    params.push_back( param );
    return new_index;
}


std::shared_ptr< Param > HPlugin::GetParam( unsigned int paramIndex ) {
    if (paramIndex < params.size()) {
        return params[paramIndex];
    } else {
        return NULL;
    }
}

std::shared_ptr< ParamText > HPlugin::GetParamText( unsigned int paramIndex )
{
    auto param = GetParam( paramIndex );
    if( !param )
        return NULL;
    auto textParam = std::dynamic_pointer_cast< ParamText >( param );
    if( !textParam )
        return NULL;
    return textParam;
}

std::shared_ptr< ParamOption > HPlugin::GetParamOption( unsigned int paramIndex )
{
    auto param = GetParam( paramIndex );
    if( !param )
        return NULL;
    auto option = std::dynamic_pointer_cast< ParamOption >( param );
    if( !option )
        return NULL;
    return option;
}


