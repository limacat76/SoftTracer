#pragma once
#include <string>
#include <stdint.h>

typedef uint32_t pixel;

typedef uint32_t color;

// needs to stay for testing new targets (DirectX, Vulkan, Mantle, File Output...)
void make_picture_noise(pixel* a_picture, int width, int height);

// needs to stay for testing new targets (DirectX, Vulkan, Mantle, File Output...)
void make_picture_test(pixel* a_picture, int width, int height);

void make_picture_blank(pixel* a_picture, int width, int height);

// REFACTOR_TO_NAMESPACE SOFTTRACER::MATHS
const float  PI = 3.14159265358979f; 

// REFACTOR_TO_NAMESPACE SOFTTRACER::MATHS
const double PI_D = 3.141592653589793238463;
