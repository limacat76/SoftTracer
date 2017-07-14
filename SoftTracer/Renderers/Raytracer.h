#pragma once
#include <Eigen/Eigen>
#include "../Interface/CallableAlgorithm.h"

/**
* Epsilon is an arbitrarily low value
*/
// Physics
const float EPSILON = 0.0001f;

/**
* Number of times we will reflect a ray
*/
const int TRACE_DEPTH = 6;

class Raytracer : public CallableAlgorithm {
private:

public:
	Raytracer();

	~Raytracer();

	void initialize_scene(const void* parameters);

	void render(void* target, void* mailbox, const void* parameters);
};

// Ported from Jacco Bikker 2004 Tutorial engine
class JBEngine : public CallableAlgorithm {
protected:
	// renderer data
	Scene* my_scene;

	// float m_WX1, m_WY1, m_WX2, m_WY2, m_DX, m_DY, m_SX, m_SY;
	int width, height;

	//Primitive** m_LastRow;

public:
	JBEngine();

	~JBEngine();

	void initialize_scene(const void* parameters);

	void render(void* target, void* mailbox, const void* parameters);

private:
	color Raytrace(Ray& a_Ray, int a_Depth, float a_RIndex, float& a_Dist);
	
//	bool Render();

};
