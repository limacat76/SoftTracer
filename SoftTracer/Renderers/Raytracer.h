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

	void initialize_scene();

	void render(void* target, void* mailbox, const void* parameters);
};

// Ported from Jacco Bikker 2004 Tutorial engine
class JBEngine : public CallableAlgorithm {
protected:
	// renderer data
	float m_WX1, m_WY1, m_WX2, m_WY2, m_DX, m_DY, m_SX, m_SY;
	Scene* m_Scene;
	pixel* m_Dest;
	int m_Width, m_Height, m_CurrLine, m_PPos;
	Primitive** m_LastRow;

public:
	JBEngine();

	~JBEngine();

	void initialize_scene();

	void render(void* target, void* mailbox, const void* parameters);

private:
#ifdef _DEBUG
	int my_thread_no;
#endif

//	void InitRender();
//	void SetTarget(pixel* a_Dest, int a_Width, int a_Height);

//	bool Render();
//	Primitive* Raytrace(Ray& a_Ray, color& a_Acc, int a_Depth, float a_RIndex, float& a_Dist);

};
