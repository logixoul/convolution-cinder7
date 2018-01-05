#pragma once

#include "D.h"

const int imageSize = 300;
extern Image image;
extern float zoom;
const int windowSize = 600;
const int windowWidth = windowSize;
const int windowHeight = windowSize;
const float windowScale = windowSize / (float)imageSize;
extern Array2D<vec4> pic;
extern int framesElapsed;
