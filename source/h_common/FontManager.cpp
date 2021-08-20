//
//  FontManager.cpp
//  HTextChaser
//
//  Created by Erez Makavy on 09/09/2021.
//

#include "FontManager.h"
#include <filesystem>
namespace fs = std::__fs::filesystem;

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>

string modulePath;
__attribute__((constructor))
void initModulePath(void) {
    Dl_info dl_info;
    dladdr((void *)initModulePath, &dl_info);
    string path;
    path.append(dl_info.dli_fname);
    modulePath = path;
}


FontManager::FontManager() {
    initModulePath();
}

void FontManager::initFonts() {
    fonts.clear();
    
    // Default Font
    fs::path defaultFontPath = modulePath;
    defaultFontPath.remove_filename();
    defaultFontPath += "../Resources/ariblk.ttf";
    fonts.push_back({0, defaultFontPath.string() , "Default (Arial Black)"});

    
    // Other Fonts from `fonts` folder which sits besides the bundle
    fs::path fontsPath = modulePath;
    fontsPath.remove_filename();
    fontsPath += "../../../fonts/"; // fonts folder besides the bundle
    
    int i = 1;
    for (const auto & entry : fs::directory_iterator(fontsPath.string())) {
        fs::path p = entry.path();
        
        if (p.extension() == ".ttf" || p.extension() == ".ttc") {
            fonts.push_back({i,  p.string(), p.stem()});
            i++;
        }
    }
}

vector<FontMeta> FontManager::getFonts() {
    return fonts;
}

FontMeta FontManager::getDefaultFont() {
    return fonts[0];
}

//string HTextChaser::getCurrentFontFileName() {
//    int fontOptionIndex = (int) GetParamOption(PN_FONT_FILE_NAME)->GetRealValue();
//    if (fontOptionIndex == 0) {
//        fs::path defaultFontPath = modulePath;
//        defaultFontPath.remove_filename();
//        defaultFontPath += "../Resources/ariblk.ttf";
//        return defaultFontPath.string();
//    } else {
//        const string fontFileName = fontOptions[(int) GetParamOption(PN_FONT_FILE_NAME)->GetRealValue()].name;
//        fs::path fontPath = modulePath;
//        fontPath.remove_filename();
//        fontPath += "../../../fonts/"; // fonts folder besides the bundle
//        
//        fontPath += fontFileName;
//        
//        return fontPath.string();
//    }
//}
