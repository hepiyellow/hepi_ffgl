//
//  FontManager.hpp
//  HTextChaser
//
//  Created by Erez Makavy on 09/09/2021.
//

#ifndef FontManager_h
#define FontManager_h

#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

struct FontMeta {
    int id;
    string path;
    string displayName;
};

class FontManager {
public:
    FontManager();
    
    // Methods
    void initFonts();
    vector<FontMeta> getFonts();
    /**
        Get the default built-in font.
     */
    FontMeta getDefaultFont();
    
private:
    vector<FontMeta> fonts;
};

#endif /* FontManager_h */
