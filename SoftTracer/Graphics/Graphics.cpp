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

Material::Material() : m_color(0), reflection(0.0), diffuse(0.0) {
	;
}

Material::Material(const color a_color, float a_reflection, float a_diffuse)
	: m_color(a_color), reflection(a_reflection), diffuse(a_diffuse) {
	assert(diffuse <= 1.0f);
}

color Material::get_color() { 
	return m_color;
}

float Material::get_specular() { 
	return 1.0f - diffuse; 
}

float Material::get_diffuse() { 
	return diffuse;
}

float Material::get_reflection() { 
	return reflection; 
}

Eigen::Vector3f Plane::get_normal() {
	return normal;
}

Eigen::Vector3f Plane::get_normal(Eigen::Vector3f& a_point) {
	return normal;
}

float Plane::get_distance() {
	return distance;
}

Ray::Ray() : origin(Eigen::Vector3f(0, 0, 0)), direction(Eigen::Vector3f(0, 0, 0)) {
	;
}

Ray::Ray(Eigen::Vector3f& an_origin, Eigen::Vector3f& a_direction) : origin(an_origin), direction(a_direction) {
	;
}

Eigen::Vector3f& Ray::get_origin() {
	return origin; 
}

Eigen::Vector3f& Ray::get_direction() {
	return direction; 
}

Primitive::Primitive()
	: name("unnamed primitive"), material(Material()), light(false) {
	;
};

Primitive::Primitive(std::string a_name, Material a_material, bool a_light)
	: name(a_name), material(a_material), light(a_light) {
	;
};

bool Primitive::is_light() { 
	return light; 
}

std::string Primitive::get_name() { 
	return name; 
}

Sphere::Sphere() 
	: centre(Eigen::Vector3f(0, 0, 0)), square_radius(1.0f), radius(1.0f), inverse_radius(1.0f / 1.0f) {
	;
};

Sphere::Sphere(Eigen::Vector3f& a_centre, float a_radius, std::string a_name, Material a_material, bool a_light)
	: Primitive(a_name, a_material, a_light), centre(a_centre), square_radius(a_radius * a_radius), radius(a_radius), inverse_radius(1.0f / a_radius) {
	;
}

Eigen::Vector3f& Sphere::get_centre() {
	return centre; 
}

float Sphere::get_square_radius() {
	return square_radius;
}

int Sphere::intersect(Ray& a_ray, float& a_distance) {
	Eigen::Vector3f v = a_ray.get_origin() - centre;
	float b = v.dot(a_ray.get_direction());
	float det = (b * b) - v.dot(v) + square_radius;
	int retval = IntersectionStatus::MISS;
	if (det > 0) {
		det = sqrtf(det);
		float i1 = b - det;
		float i2 = b + det;
		if (i2 < 0) {
			if (i1 < 0) {
				if (i2 < a_distance) {
					a_distance = i2;
					retval = IntersectionStatus::INSIDE;
				}
			} else {
				if (i1 < a_distance) {
					a_distance = i1;
					retval = IntersectionStatus::HIT;
				}
			}
		}
	}
	return retval;
}

Eigen::Vector3f Sphere::get_normal(Eigen::Vector3f& a_point) { 
	return (a_point - centre) * inverse_radius; 
}

Plane::Plane() : normal(0, 0, 0), distance(0) {
	;
};

Plane::Plane(Eigen::Vector3f a_normal, float a_distance, std::string a_name, Material a_material, bool a_light)
	: Primitive(a_name, a_material, a_light), normal(a_normal), distance(a_distance) {
	;
};

int Plane::intersect(Ray& a_ray, float& a_Dist) {
	float d = normal.dot(a_ray.get_direction());
	if (d != 0) {
		float dist = normal.dot(a_ray.get_origin()) + distance / d;
		if (dist > 0 && dist < a_Dist) {
			a_Dist = dist;
			return IntersectionStatus::HIT;
		}
	}
	return IntersectionStatus::MISS;
}

Scene::Scene() : primitives_number(0), primitives(0) {
	;
}

int Scene::get_primitives_number() {
	return primitives_number;
}

Primitive* Scene::get_primitive(int at_index) {
	return primitives[at_index];
}

Material Primitive::get_material() {
	return material;
}

Scene::~Scene()
{
	delete[] primitives;
}

int ranged_int(float value) {
	float ranged = 0.0f;
	if (value < 0.0f) {
		ranged = 0.0f;
	} else if (value > 1.0f) {
		ranged = 1.0f;
	} else {
		ranged = value;
	}
	ranged *= 255;
	return (int)ranged;
}

color Color(float red, float green, float blue) {
	return 255 << 24 | ranged_int(red) << 16 | ranged_int(green) << 8 | ranged_int(blue);
}

color Color(Eigen::Vector3f a_vector) {
	return 255 << 24 | ranged_int(a_vector.x()) << 16 | ranged_int(a_vector.y()) << 8 | ranged_int(a_vector.z());
}

std::vector< float > Array(color a_color) {
	std::vector< float > result;
	result.push_back(((float)((0x00FF0000 & a_color) >> 16)) / 255);
	result.push_back(((float)((0x0000FF00 & a_color) >> 8)) / 255);
	result.push_back(((float)((0x000000FF & a_color))) / 255);
	return result;
}

Eigen::Vector3f Vector(color a_color) {
	float r = ((0x00FF0000 & a_color) >> 16) / 255;
	float g = ((0x0000FF00 & a_color) >> 8) / 255;
	float b = ((0x000000FF & a_color)) / 255;
	return Eigen::Vector3f(r, g, b);
}

void Scene::initialize_scene() {
	primitives = new Primitive*[100];

	// ground plane
	primitives[0] = new Plane(Eigen::Vector3f(0, 1, 0), 4.4f, "plane", Material(Color(0.4f, 0.3f, 0.3f), 0, 1));
	primitives[1] = new Sphere(Eigen::Vector3f(1, -0.8f, 3), 2.5f, "big sphere", Material(Color(0.7f, 0.7f, 0.7f), 0.6f));
	primitives[2] = new Sphere(Eigen::Vector3f(-5.5f, -0.5, 7), 2.0f, "small sphere", Material(Color(0.7f, 0.7f, 1.0f), 1.0f, 0.1f));
	primitives[3] = new Sphere(Eigen::Vector3f(0.0f, 5.0f, 5.0f), 0.1f, "light source 1", Material(Color(0.6f, 0.6f, 0.6f)), true);
	primitives[4] = new Sphere(Eigen::Vector3f(2.0f, 5.0f, 1.0f), 0.1f, "light source 2", Material(Color(0.7f, 0.7f, 0.9f)), true);
//	primitives[5] = new Sphere(Eigen::Vector3f(1.0f, 2, 5), 10.0f, "fanculo sphere", Material(Color(0.7f, 0.7f, 1.0f), 1.0f, 0.1f));
	primitives_number = 5;

}