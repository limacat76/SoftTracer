#pragma once
#include <stdint.h>

typedef uint32_t pixel;

// needs to stay for testing new targets (DirectX, Vulkan, Mantle, File Output...)
void make_picture_noise(pixel* a_picture, int width, int height);

// needs to stay for testing new targets (DirectX, Vulkan, Mantle, File Output...)
void make_picture_test(pixel* a_picture, int width, int height);

void make_picture_blank(pixel* a_picture, int width, int height);
