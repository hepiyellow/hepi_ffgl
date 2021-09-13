#pragma once

#include "Utils.h"

using namespace utils;

namespace tr {

enum class AlignmentHorizontal {
    Left, Center, Right
};

enum class SpacingMode {
    Manual, AutoFit
};

enum class AlignmentVertical {
    Top, Center, Bottom
};

enum class Range {
    Characters,
    Words,
    Lines
};

enum class Layout {
    Rows,
    Circle,
};



struct TextParams {
    std::u16string text = utils::toUtf16("");
    float size = 1; // Size of 1 equals to 1/4 of the screen height
    float pos_x = 0;
    float pos_y = 0;
    AlignmentHorizontal align_h = AlignmentHorizontal::Center;
    AlignmentVertical align_v = AlignmentVertical::Center;
    
    // Circle Mode
    float radius = 0.5;
    float rotation = 0;
    bool rotateTogether = true;
    
    // Char Rotation
    float charRotation = 0;
    float charRotationFan = 0;
    
    // Spacing
    float lineWidth = 1;
    float spacing_x = 1;
    SpacingMode spacing_x_mode = SpacingMode::Manual;
    
    // Chaser
    Range range = Range::Characters;
    float length = 0;
    float offset = 0;
    Layout layout = Layout::Rows;
};

const int MAX_UNIQ_CHARACTERS = 100;

} // namespace tr
