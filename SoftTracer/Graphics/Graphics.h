#pragma once
#include <Eigen/Eigen>
#include <stdint.h>

typedef uint32_t pixel;

// needs to stay for testing new targets (DirectX, Vulkan, Mantle, File Output...)
void make_picture_noise(pixel* a_picture, int width, int height);

// needs to stay for testing new targets (DirectX, Vulkan, Mantle, File Output...)
void make_picture_test(pixel* a_picture, int width, int height);

void make_picture_blank(pixel* a_picture, int width, int height);

// SOFTTRACER::MATHS
const float  pi = 3.14159265358979f; 

// SOFTTRACER::MATHS
const double pi_d = 3.141592653589793238463;

/**
 * Given a distance d, and a normal a, b, c, the definition of a plane is
 *
 * a * x + b * y + c * z - d = 0
 *
 * any point x, y, z that satisfies the equation lies on the plane.
 */
class Plane {
public:
	Plane() : normal(0, 0, 0), distance(0) {};

	Plane(Eigen::Vector3f a_normal, float a_distance) : normal(a_normal), distance(a_distance) {};

	Eigen::Vector3f normal;

	float distance;
};