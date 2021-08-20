//
//  ShapeRenderer.hpp
//  HCloner
//
//  Created by Erez Makavy on 04/09/2021.
//

#ifndef ShapeRenderer_h
#define ShapeRenderer_h
#include <FFGLSDK.h>
#include "BaseRenderer.h"
#endif /* ShapeRenderer_h */

class ShapeRenderer : public BaseRenderer {
public:
    
    bool InitGL();
    
    
protected:
    void updateVertices() override;
    
};
