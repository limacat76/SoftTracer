#include <random>
#include <limits>
#include "Graphics.h"

void make_picture_noise(pixel* a_picture, int width, int height) {
	std::random_device rd;
	std::mt19937_64 eng(rd());
	std::uniform_int_distribution<pixel> distr;
	const pixel all_alpha = 255 << 24;

	for (int row = 0; row < height; row++) {
		for (int column = 0; column < width; column++) {
			a_picture[row * width + column] = distr(eng) | all_alpha;
		}
	}
}

void make_picture_test(pixel* a_picture, int width, int height) {
	for (int row = 0; row < height; row++) {
		for (int column = 0; column < width; column++) {
			a_picture[row * width + column] = ((row % 255) << 16) + ((column % 255) << 8) + ((row + column) % 255);
		}
	}
}

void make_picture_blank(pixel* a_picture, int width, int height) {
	pixel* aPicture = new pixel[width * height]{ 0 };
}
