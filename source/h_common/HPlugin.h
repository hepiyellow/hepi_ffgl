//
//  HPlugin.hpp
//  HCloner
//
//  Created by Erez Makavy on 10/09/2021.
//

#ifndef HPlugin_h
#define HPlugin_h

#include <FFGLSDK.h>
using namespace ffglqs;
using namespace std;

class HPlugin : public ffglqs::Plugin {
protected:
    /** Returns the param index in Plugin::params, which is the same as the paramID (ParamInfo.ID)  */
    unsigned int AddParam( std::shared_ptr< Param > param );
    unsigned int AddParam( std::shared_ptr< ParamRange > param );
    unsigned int AddParam( std::shared_ptr< ParamOption > param );
    unsigned int AddParam( std::shared_ptr< ParamRange > param, string groupName);
    
    std::shared_ptr< Param > GetParam( unsigned int paramIndex );
    std::shared_ptr< ParamText > GetParamText( unsigned int paramIndex );
    std::shared_ptr< ParamOption > GetParamOption( unsigned int paramIndex );
    std::shared_ptr< ParamInfo > GetParamInfo( unsigned int paramID );
    
private:
    unsigned int AddParamBase( std::shared_ptr< Param > param );
};

#endif /* HPlugin_h */
