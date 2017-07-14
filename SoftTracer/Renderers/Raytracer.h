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

namespace JBRaytracer {

	typedef unsigned int Pixel;

	inline float Rand(float a_Range) { return ((float)rand() / RAND_MAX) * a_Range; }

#define DOT(A,B)		(A.x*B.x+A.y*B.y+A.z*B.z)
#define NORMALIZE(A)	{float l=1/sqrtf(A.x*A.x+A.y*A.y+A.z*A.z);A.x*=l;A.y*=l;A.z*=l;}
#define LENGTH(A)		(sqrtf(A.x*A.x+A.y*A.y+A.z*A.z))
#define SQRLENGTH(A)	(A.x*A.x+A.y*A.y+A.z*A.z)
#define SQRDISTANCE(A,B) ((A.x-B.x)*(A.x-B.x)+(A.y-B.y)*(A.y-B.y)+(A.z-B.z)*(A.z-B.z))

#define EPSILON			0.0001f
#define TRACEDEPTH		6

#define PI				3.141592653589793238462f

	class vector3
	{
	public:
		vector3() : x(0.0f), y(0.0f), z(0.0f) {};
		vector3(float a_X, float a_Y, float a_Z) : x(a_X), y(a_Y), z(a_Z) {};
		void Set(float a_X, float a_Y, float a_Z) { x = a_X; y = a_Y; z = a_Z; }
		void Normalize() { float l = 1.0f / Length(); x *= l; y *= l; z *= l; }
		float Length() { return (float)sqrt(x * x + y * y + z * z); }
		float SqrLength() { return x * x + y * y + z * z; }
		float Dot(vector3 a_V) { return x * a_V.x + y * a_V.y + z * a_V.z; }
		vector3 Cross(vector3 b) { return vector3(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x); }
		void operator += (vector3& a_V) { x += a_V.x; y += a_V.y; z += a_V.z; }
		void operator += (vector3* a_V) { x += a_V->x; y += a_V->y; z += a_V->z; }
		void operator -= (vector3& a_V) { x -= a_V.x; y -= a_V.y; z -= a_V.z; }
		void operator -= (vector3* a_V) { x -= a_V->x; y -= a_V->y; z -= a_V->z; }
		void operator *= (float f) { x *= f; y *= f; z *= f; }
		void operator *= (vector3& a_V) { x *= a_V.x; y *= a_V.y; z *= a_V.z; }
		void operator *= (vector3* a_V) { x *= a_V->x; y *= a_V->y; z *= a_V->z; }
		vector3 operator- () const { return vector3(-x, -y, -z); }
		friend vector3 operator + (const vector3& v1, const vector3& v2) { return vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }
		friend vector3 operator - (const vector3& v1, const vector3& v2) { return vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }
		friend vector3 operator + (const vector3& v1, vector3* v2) { return vector3(v1.x + v2->x, v1.y + v2->y, v1.z + v2->z); }
		friend vector3 operator - (const vector3& v1, vector3* v2) { return vector3(v1.x - v2->x, v1.y - v2->y, v1.z - v2->z); }
		friend vector3 operator * (const vector3& v, float f) { return vector3(v.x * f, v.y * f, v.z * f); }
		friend vector3 operator * (const vector3& v1, vector3& v2) { return vector3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z); }
		friend vector3 operator * (float f, const vector3& v) { return vector3(v.x * f, v.y * f, v.z * f); }
		union
		{
			struct { float x, y, z; };
			struct { float r, g, b; };
			struct { float cell[3]; };
		};
	};

	class plane
	{
	public:
		plane() : N(0, 0, 0), D(0) {};
		plane(vector3 a_Normal, float a_D) : N(a_Normal), D(a_D) {};
		union
		{
			struct
			{
				vector3 N;
				float D;
			};
			float cell[4];
		};
	};

	typedef vector3 Color;


	// -----------------------------------------------------------
	// Ray class definition
	// -----------------------------------------------------------
	class Ray
	{
	public:
		Ray() : m_Origin(vector3(0, 0, 0)), m_Direction(vector3(0, 0, 0)) {};
		Ray(vector3& a_Origin, vector3& a_Dir);
		void SetOrigin(vector3& a_Origin) { m_Origin = a_Origin; }
		void SetDirection(vector3& a_Direction) { m_Direction = a_Direction; }
		vector3& GetOrigin() { return m_Origin; }
		vector3& GetDirection() { return m_Direction; }
	private:
		vector3 m_Origin;
		vector3 m_Direction;
	};

	// -----------------------------------------------------------
	// Engine class definition
	// Raytracer core
	// -----------------------------------------------------------
	class Scene;
	class Primitive;
	class Engine
	{
	public:
		Engine();
		~Engine();
		void SetTarget(Pixel* a_Dest, int a_Width, int a_Height);
		Scene* GetScene() { return m_Scene; }
		Primitive* Raytrace(Ray& a_Ray, Color& a_Acc, int a_Depth, float a_RIndex, float& a_Dist);
		void InitRender();
		bool Render();
	protected:
		// renderer data
		float m_WX1, m_WY1, m_WX2, m_WY2, m_DX, m_DY, m_SX, m_SY;
		Scene* m_Scene;
		Pixel* m_Dest;
		int m_Width, m_Height, m_CurrLine, m_PPos;
		Primitive** m_LastRow;
	};


	// Intersection method return values
#define HIT		 1		// Ray hit primitive
#define MISS	 0		// Ray missed primitive
#define INPRIM	-1		// Ray started inside primitive

	// -----------------------------------------------------------
	// Material class definition
	// -----------------------------------------------------------

	class Material
	{
	public:
		Material();
		void SetColor(Color& a_Color) { m_Color = a_Color; }
		Color GetColor() { return m_Color; }
		void SetDiffuse(float a_Diff) { m_Diff = a_Diff; }
		void SetReflection(float a_Refl) { m_Refl = a_Refl; }
		float GetSpecular() { return 1.0f - m_Diff; }
		float GetDiffuse() { return m_Diff; }
		float GetReflection() { return m_Refl; }
	private:
		Color m_Color;
		float m_Refl;
		float m_Diff;
	};

	// -----------------------------------------------------------
	// Primitive class definition
	// -----------------------------------------------------------

	class Primitive
	{
	public:
		enum
		{
			SPHERE = 1,
			PLANE
		};
		Primitive() : m_Name(""), m_Light(false) {};
		Material* GetMaterial() { return &m_Material; }
		void SetMaterial(Material& a_Mat) { m_Material = a_Mat; }
		virtual int GetType() = 0;
		virtual int Intersect(Ray& a_Ray, float& a_Dist) = 0;
		virtual vector3 GetNormal(vector3& a_Pos) = 0;
		virtual Color GetColor(vector3&) { return m_Material.GetColor(); }
		virtual void Light(bool a_Light) { m_Light = a_Light; }
		bool IsLight() { return m_Light; }
		void SetName(std::string a_Name);
		std::string GetName() { return m_Name; }
	protected:
		Material m_Material;
		std::string m_Name;
		bool m_Light;
	};

	// -----------------------------------------------------------
	// Sphere primitive class definition
	// -----------------------------------------------------------

	class Sphere : public Primitive
	{
	public:
		int GetType() { return SPHERE; }
		Sphere(vector3& a_Centre, float a_Radius) :
			m_Centre(a_Centre), m_SqRadius(a_Radius * a_Radius),
			m_Radius(a_Radius), m_RRadius(1.0f / a_Radius) {};
		vector3& GetCentre() { return m_Centre; }
		float GetSqRadius() { return m_SqRadius; }
		int Intersect(Ray& a_Ray, float& a_Dist);
		vector3 GetNormal(vector3& a_Pos) { return (a_Pos - m_Centre) * m_RRadius; }
	private:
		vector3 m_Centre;
		float m_SqRadius, m_Radius, m_RRadius;
	};

	// -----------------------------------------------------------
	// PlanePrim primitive class definition
	// -----------------------------------------------------------

	class PlanePrim : public Primitive
	{
	public:
		int GetType() { return PLANE; }
		PlanePrim(vector3& a_Normal, float a_D) : m_Plane(plane(a_Normal, a_D)) {};
		vector3& GetNormal() { return m_Plane.N; }
		float GetD() { return m_Plane.D; }
		int Intersect(Ray& a_Ray, float& a_Dist);
		vector3 GetNormal(vector3& a_Pos);
	private:
		plane m_Plane;
	};

	// -----------------------------------------------------------
	// Scene class definition
	// -----------------------------------------------------------

	class Scene
	{
	public:
		Scene() : m_Primitives(0), m_Primitive(0) {};
		~Scene();
		void InitScene();
		int GetNrPrimitives() { return m_Primitives; }
		Primitive* GetPrimitive(int a_Idx) { return m_Primitive[a_Idx]; }
	private:
		int m_Primitives;
		Primitive** m_Primitive;
	};

	class JBEngine : public CallableAlgorithm {
	private:
		Scene* m_Scene;
	
		Primitive* Raytrace(Ray& a_Ray, Color& a_Acc, int a_Depth, float a_RIndex, float& a_Dist);
	public:
		JBEngine();

		~JBEngine();

		void initialize_scene(const void* parameters);

		void render(void* target, void* mailbox, const void* parameters);
	protected:
		// renderer data
	};


}; // namespace JBRaytracer
