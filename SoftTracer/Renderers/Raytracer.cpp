#include "../Graphics/Graphics.h"
#include "../Interface/Mailbox.h"
#include "../Interface/Parameters.h"
#include "Raytracer.h"
#ifdef _DEBUG
#include <iostream>
#endif

Raytracer::Raytracer() {
#ifdef _DEBUG
	std::cout << "ctor called! \n";
#endif
}

Raytracer::~Raytracer() {
#ifdef _DEBUG
	std::cout << "dtor called! \n";
#endif
}

void Raytracer::initialize_scene(const void* parameters) {
	// Bands does not need to initialize a scene...
}

void Raytracer::render(void* target, void* mailbox, const void* parameters) {
	pixel* image = (pixel*)target;
	
	MailBox* my_mailbox = (MailBox *)mailbox;
	const Parameters* my_parameters = (const Parameters *)parameters;

	const int thread_no = my_parameters->threadNumber;
	const int total_threads = my_parameters->totalThreads;
	const int band = my_parameters->height / total_threads;
	const int startY = thread_no * band;
	const int endY = thread_no == total_threads - 1 ? my_parameters->height : thread_no * band + band;
	const int width = my_parameters->width;

	my_mailbox->work_started(thread_no);
	pixel* currentPixel = image + (startY * width);

	// Trace rays
	bool quit = false;
	for (int y = startY; !quit && y < endY; ++y) {
		for (int x = 0; !quit && x < width; ++x, ++currentPixel) {
			*currentPixel = (255 << 24) + ((x % 255) << 16) + ((y % 255) << 8) + ((x + y) % 255);
			quit = my_mailbox->to_children_quit;
		}
	}

	my_mailbox->work_done(thread_no);
}

namespace JBRaytracer {


	JBEngine::JBEngine() {
		m_Scene = new Scene();
	}

	JBEngine::~JBEngine() {
		delete m_Scene;
	}

	void JBEngine::initialize_scene(const void* parameters) {
		m_Scene->InitScene();
	}

	void JBEngine::render(void* target, void* mailbox, const void* parameters) {
		pixel* m_Dest = (pixel*)target;

		MailBox* my_mailbox = (MailBox *)mailbox;
		const Parameters* my_parameters = (const Parameters *)parameters;
		const int total_threads = my_parameters->totalThreads;
		const int thread_no = my_parameters->threadNumber;
		const int m_Width = my_parameters->width;
		const int m_Height = my_parameters->height;
		
		float m_WX1, m_WY1, m_WX2, m_WY2, m_DX, m_DY, m_SX;

		// screen plane in world space coordinates
		m_WX1 = -16/3, m_WX2 = 16 / 3, m_WY1 = 9 / 3, m_WY2 = -9 / 3;
		// calculate deltas for interpolation
		m_DX = (m_WX2 - m_WX1) / m_Width;
		m_DY = (m_WY2 - m_WY1) / m_Height;

		// allocate space to store pointers to primitives for previous line
		// m_LastRow = new Primitive*[m_Width];
		// memset(m_LastRow, 0, m_Width * 4);


		// set firts line to draw to
		int m_CurrLine = (m_Height / total_threads * thread_no); // 0;
		// set pixel buffer address of first pixel
		int m_PPos = m_CurrLine * m_Width;
		int band_height = (m_Height / total_threads) + m_CurrLine; // 0;
		float m_SY = m_WY1 + (m_DY * m_CurrLine);


		my_mailbox->work_started(thread_no);

		// render scene
		vector3 o(0, 0, -5);
		// initialize timer
		Primitive* lastprim = 0;
		// render remaining lines
		bool quit = false;
		for (int y = m_CurrLine; !quit && y < band_height; y++)
		{
			m_SX = m_WX1;
			// render pixels for current line
			for (int x = 0; x < m_Width; x++)
			{
				// fire primary ray
				Color acc(0, 0, 0);
				vector3 dir = vector3(m_SX, m_SY, 0) - o;
				NORMALIZE(dir);
				Ray r(o, dir);
				float dist;
				Primitive* prim = Raytrace(r, acc, 1, 1.0f, dist);
				int red = (int)(acc.r * 256);
				int green = (int)(acc.g * 256);
				int blue = (int)(acc.b * 256);
				if (red > 255) red = 255;
				if (green > 255) green = 255;
				if (blue > 255) blue = 255;
				m_Dest[m_PPos++] = (red << 16) + (green << 8) + blue;
				m_SX += m_DX;
			}
			m_SY += m_DY;
			quit = my_mailbox->to_children_quit;
		}
		// all done

		my_mailbox->work_done(thread_no);
	}

	// -----------------------------------------------------------
	// Engine::Raytrace
	// Naive ray tracing: Intersects the ray with every primitive
	// in the scene to determine the closest intersection
	// -----------------------------------------------------------
	Primitive* JBEngine::Raytrace(Ray& a_Ray, Color& a_Acc, int a_Depth, float a_RIndex, float& a_Dist)
	{
		if (a_Depth > TRACEDEPTH) return 0;
		// trace primary ray
		a_Dist = 1000000.0f;
		vector3 pi;
		Primitive* prim = 0;
		int result;
		// find the nearest intersection
		for (int s = 0; s < m_Scene->GetNrPrimitives(); s++)
		{
			Primitive* pr = m_Scene->GetPrimitive(s);
			int res;
			if (res = pr->Intersect(a_Ray, a_Dist))
			{
				prim = pr;
				result = res; // 0 = miss, 1 = hit, -1 = hit from inside primitive
			}
		}
		// no hit, terminate ray
		if (!prim) return 0;
		// handle intersection
		if (prim->IsLight())
		{
			// we hit a light, stop tracing
			a_Acc = Color(1, 1, 1);
		}
		else
		{
			// determine color at point of intersection
			pi = a_Ray.GetOrigin() + a_Ray.GetDirection() * a_Dist;
			// trace lights
			for (int l = 0; l < m_Scene->GetNrPrimitives(); l++)
			{
				Primitive* p = m_Scene->GetPrimitive(l);
				if (p->IsLight())
				{
					Primitive* light = p;
					// calculate diffuse shading
					vector3 L = ((Sphere*)light)->GetCentre() - pi;
					NORMALIZE(L);
					vector3 N = prim->GetNormal(pi);
					if (prim->GetMaterial()->GetDiffuse() > 0)
					{
						float dot = DOT(N, L);
						if (dot > 0)
						{
							float diff = dot * prim->GetMaterial()->GetDiffuse();
							// add diffuse component to ray color
							a_Acc += diff * prim->GetMaterial()->GetColor() * light->GetMaterial()->GetColor();
						}
					}
				}
			}
		}
		// return pointer to primitive hit by primary ray
		return prim;
	}

	// TODO_REMOVE;
	Engine::Engine() {}

	// TODO_REMOVE;
	Engine::~Engine() {}

	// -----------------------------------------------------------
	// Engine::SetTarget
	// Sets the render target canvas
	// -----------------------------------------------------------
	void Engine::SetTarget(Pixel* a_Dest, int a_Width, int a_Height) {}
	
	// -----------------------------------------------------------
	// Engine::InitRender
	// Initializes the renderer, by resetting the line / tile
	// counters and precalculating some values
	// -----------------------------------------------------------
	void Engine::InitRender()
	{
		;
	}

	// -----------------------------------------------------------
	// Engine::Raytrace
	// Naive ray tracing: Intersects the ray with every primitive
	// in the scene to determine the closest intersection
	// -----------------------------------------------------------
	Primitive* Engine::Raytrace(Ray& a_Ray, Color& a_Acc, int a_Depth, float a_RIndex, float& a_Dist)
	{
		;
		return nullptr;
	}

	// -----------------------------------------------------------
	// Engine::Render
	// Fires rays in the scene one scanline at a time, from left
	// to right
	// -----------------------------------------------------------
	bool Engine::Render()
	{
		return false;
	}

	// ---
	// RAY
	// ---
	Ray::Ray(vector3& a_Origin, vector3& a_Dir) :
		m_Origin(a_Origin),
		m_Direction(a_Dir)
	{
	}

	// -----------------------------------------------------------
	// Primitive class implementation
	// -----------------------------------------------------------

	void Primitive::SetName(std::string a_Name)
	{
		m_Name = a_Name;
	}

	// -----------------------------------------------------------
	// Material class implementation
	// -----------------------------------------------------------

	Material::Material() :
		m_Color(Color(0.2f, 0.2f, 0.2f)),
		m_Refl(0), m_Diff(0.2f)
	{
	}

	// -----------------------------------------------------------
	// Sphere primitive methods
	// -----------------------------------------------------------

	int Sphere::Intersect(Ray& a_Ray, float& a_Dist)
	{
		vector3 v = a_Ray.GetOrigin() - m_Centre;
		float b = -DOT(v, a_Ray.GetDirection());
		float det = (b * b) - DOT(v, v) + m_SqRadius;
		int retval = MISS;
		if (det > 0)
		{
			det = sqrtf(det);
			float i1 = b - det;
			float i2 = b + det;
			if (i2 > 0)
			{
				if (i1 < 0)
				{
					if (i2 < a_Dist)
					{
						a_Dist = i2;
						retval = INPRIM;
					}
				}
				else
				{
					if (i1 < a_Dist)
					{
						a_Dist = i1;
						retval = HIT;
					}
				}
			}
		}
		return retval;
	}

	// -----------------------------------------------------------
	// Plane primitive class implementation
	// -----------------------------------------------------------

	int PlanePrim::Intersect(Ray& a_Ray, float& a_Dist)
	{
		float d = DOT(m_Plane.N, a_Ray.GetDirection());
		if (d != 0)
		{
			float dist = -(DOT(m_Plane.N, a_Ray.GetOrigin()) + m_Plane.D) / d;
			if (dist > 0)
			{
				if (dist < a_Dist)
				{
					a_Dist = dist;
					return HIT;
				}
			}
		}
		return MISS;
	}

	vector3 PlanePrim::GetNormal(vector3& a_Pos)
	{
		return m_Plane.N;
	}

	// -----------------------------------------------------------
	// Scene class implementation
	// -----------------------------------------------------------

	Scene::~Scene()
	{
		delete m_Primitive;
	}

	void Scene::InitScene()
	{
		m_Primitive = new Primitive*[100];
		// ground plane
		m_Primitive[0] = new PlanePrim(vector3(0, 1, 0), 4.4f);
		m_Primitive[0]->SetName("plane");
		m_Primitive[0]->GetMaterial()->SetReflection(0);
		m_Primitive[0]->GetMaterial()->SetDiffuse(1.0f);
		m_Primitive[0]->GetMaterial()->SetColor(Color(0.4f, 0.3f, 0.3f));
		// big sphere
		m_Primitive[1] = new Sphere(vector3(1, -0.8f, 3), 2.5f);
		m_Primitive[1]->SetName("big sphere");
		m_Primitive[1]->GetMaterial()->SetReflection(0.6f);
		m_Primitive[1]->GetMaterial()->SetColor(Color(0.7f, 0.7f, 0.7f));
		// small sphere
		m_Primitive[2] = new Sphere(vector3(-5.5f, -0.5, 7), 2);
		m_Primitive[2]->SetName("small sphere");
		m_Primitive[2]->GetMaterial()->SetReflection(1.0f);
		m_Primitive[2]->GetMaterial()->SetDiffuse(0.1f);
		m_Primitive[2]->GetMaterial()->SetColor(Color(0.7f, 0.7f, 1.0f));
		// light source 1
		m_Primitive[3] = new Sphere(vector3(0, 5, 5), 0.1f);
		m_Primitive[3]->Light(true);
		m_Primitive[3]->GetMaterial()->SetColor(Color(0.6f, 0.6f, 0.6f));
		// light source 2
		m_Primitive[4] = new Sphere(vector3(2, 5, 1), 0.1f);
		m_Primitive[4]->Light(true);
		m_Primitive[4]->GetMaterial()->SetColor(Color(0.7f, 0.7f, 0.9f));
		// set number of primitives
		m_Primitives = 5;
	}

}; // namespace JBRaytracer