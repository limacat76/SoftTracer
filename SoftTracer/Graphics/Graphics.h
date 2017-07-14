#pragma once
#include <Eigen/Eigen>
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

namespace Attempt1 {

enum IntersectionStatus {
	INSIDE = -1,
	MISS = 0,
	HIT = 1
};

/**
 * A class which represents a material.
 */
// REFACTOR_TO_ECS
class Material {
public:
	Material();
	
	Material(color a_color, float a_reflection = 0.0, float a_diffuse = 0.0);

	color get_color();

	float get_specular();

	float get_diffuse();

	float get_reflection();

private:
	const color m_color;

	const float reflection;

	const float diffuse;
};

/**
* A class which represents a Ray.
*/
// TN This shouldn't be refactored since Rays are "alive" and used by algorithms. It's the elements that are
// Visible to a scene that should jump in an ECS system.
class Ray {
public:
	Ray();

	Ray(Eigen::Vector3f& an_origin, Eigen::Vector3f& a_direction);

	Eigen::Vector3f& get_origin();

	Eigen::Vector3f& get_direction();

private:
	Eigen::Vector3f origin;

	Eigen::Vector3f direction;
};

/**
 * A class which represents a Primitive.
 */
 // REFACTOR_TO_ECS
class Primitive {
public:
	Primitive();

	Primitive(std::string a_name, Material a_material = Material(), bool a_light = false);

	virtual int intersect(Ray& a_Ray, float& a_Dist) = 0;

	virtual Eigen::Vector3f get_normal(Eigen::Vector3f& a_Pos) = 0;

	bool is_light();

	std::string get_name();

	virtual Material get_material();

protected:
	const Material material;

	const std::string  name;

	const bool light;
};

/**
 * A class which represents a sphere
 */
// REFACTOR_TO_ECS
class Sphere : public Primitive {
public:
	Sphere();

	Sphere(Eigen::Vector3f& a_centre, float a_radius, std::string a_name, Material a_material = Material(), bool a_light = false);

	Eigen::Vector3f& get_centre();

	float get_square_radius();

	int intersect(Ray& a_ray, float& a_dist);

	Eigen::Vector3f get_normal(Eigen::Vector3f& a_point);

private:
	Eigen::Vector3f centre;

	float square_radius;

	float radius;

	float inverse_radius;
};

/**
* A class which represents a Plane.
* Given a distance d, and a normal a, b, c, the definition of a plane is
*
* a * x + b * y + c * z - d = 0
*
* any point x, y, z that satisfies the equation lies on the plane.
*/
// REFACTOR_TO_ECS
class Plane : public Primitive {
public:
	Plane();

	Plane(Eigen::Vector3f a_normal, float a_distance, std::string a_name, Material a_material = Material(), bool a_light = false);

	Eigen::Vector3f get_normal();

	float get_distance();

	int intersect(Ray& a_ray, float& a_dist);

	Eigen::Vector3f get_normal(Eigen::Vector3f& a_point);

private:
	Eigen::Vector3f normal;

	float distance;
};

/**
 * The definition of the scene, containing primitives, that the software will render
 */
class Scene
{
public:
	Scene();

	~Scene();

	void initialize_scene();

	int get_primitives_number(); 

	Primitive* get_primitive(int at_index);
private:
	int primitives_number;

	Primitive** primitives;
};

color Color(float red, float green, float blue);

color Color(Eigen::Vector3f a_vector);

std::vector< float > Array(color a_color);

Eigen::Vector3f Vector(color a_color);

}