#include "../Graphics/Graphics.h"
#include "../Interface/Mailbox.h"
#include "../Interface/Parameters.h"
#include "Tutorial.h"

namespace JBikker {
// ------------------------
// 2004 - Jacco Bikker  <><
// ------------------------

	Engine::Engine() {
		m_Scene = new Scene();
	}

	Engine::~Engine() {
		delete m_Scene;
	}

	void Engine::initialize_scene(const void* myParameters) {
		m_Scene->InitScene();

		const Parameters* my_parameters = (const Parameters *)myParameters;
		m_Width = my_parameters->width;
		m_Height = my_parameters->height;
		// screen plane in world space coordinates
		m_WX1 = -16 / 3, m_WX2 = 16 / 3, m_WY1 = 9 / 3, m_WY2 = -9 / 3;
		// calculate deltas for interpolation
		m_DX = (m_WX2 - m_WX1) / m_Width;
		m_DY = (m_WY2 - m_WY1) / m_Height;

	}

	void Engine::job(void* target, void* mailbox, const void* parameters) {

		MailBox* my_mailbox = (MailBox *)mailbox;
		const Parameters* myParameters = (const Parameters *)parameters;

		WorkUnit * aValue = nullptr;
		const int thread_no = myParameters->threadNumber;
		my_mailbox->work_started(thread_no);
		while (!my_mailbox->to_children_quit && !(my_mailbox->to_children_no_more_work && my_mailbox->work_queue.size() == 0)) {
			{
				std::lock_guard<std::mutex> lock(my_mailbox->work_mutex);
				if (my_mailbox->work_queue.size() > 0) {
					aValue = my_mailbox->work_queue.front();
					my_mailbox->work_queue.pop_front();
				}
			}

			if (aValue != nullptr) {
				render(target, aValue, myParameters, my_mailbox);
				aValue = nullptr;
			}

			std::this_thread::yield();
		}
		my_mailbox->work_done(thread_no);
	}

	void Engine::render(void* target, WorkUnit* wuParameters, const Parameters* my_parameters, MailBox* my_mailbox) {
		pixel* m_Dest = (pixel*)target;

		// set firts line to draw to
		int m_CurrColumn = wuParameters->allocated_width; // 0;
		int m_CurrLine = wuParameters->allocated_height; // 0;
		int band_height = wuParameters->real_block_height; // 0;
		int band_width = wuParameters->real_block_width;

		float m_SY = m_WY1 + (m_DY * m_CurrLine);

		// render scene
		vector3 o(0, 0, -5);
		// initialize timer
		Primitive* lastprim = 0;
		// render remaining lines
		bool quit = false;

		int m_start = wuParameters->allocated_width + (wuParameters->allocated_height) * m_Width;
		float m_reset_X = m_WX1 + (m_CurrColumn * m_DX);
		for (int y = 0; !quit && y < band_height; y++)
		{
			int m_PPos = m_start;
			m_start += m_Width;
			float m_SX = m_reset_X;
			// render pixels for current line
			for (int x = 0; x < band_width; x++)
			{
				// fire primary rays
				Color acc(0, 0, 0);
				vector3 dir = vector3(m_SX, m_SY, 0) - o;
				NORMALIZE(dir);
				Ray r(o, dir);
				float dist;
				Primitive* prim = Raytrace(r, acc, 1, 1.0f, dist);
				int red, green, blue;
				if (prim != lastprim)
				{
					lastprim = prim;
					Color acc(0, 0, 0);
					for (int tx = -1; tx < 2; tx++) for (int ty = -1; ty < 2; ty++)
					{
						vector3 dir = vector3(m_SX + m_DX * tx / 2.0f, m_SY + m_DY * ty / 2.0f, 0) - o;
						NORMALIZE(dir);
						Ray r(o, dir);
						float dist;
						Primitive* prim = Raytrace(r, acc, 1, 1.0f, dist);
					}
					red = (int)(acc.r * (256 / 9));
					green = (int)(acc.g * (256 / 9));
					blue = (int)(acc.b * (256 / 9));
				}
				else
				{
					red = (int)(acc.r * 256);
					green = (int)(acc.g * 256);
					blue = (int)(acc.b * 256);
				}
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
	}

	// -----------------------------------------------------------
	// Engine::Raytrace
	// Naive ray tracing: Intersects the ray with every primitive
	// in the scene to determine the closest intersection
	// -----------------------------------------------------------
	Primitive* Engine::Raytrace(Ray& a_Ray, Color& a_Acc, int a_Depth, float a_RIndex, float& a_Dist)
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
					// handle point light source
					float shade = 1.0f;
					if (light->GetType() == Primitive::SPHERE)
					{
						vector3 L = ((Sphere*)light)->GetCentre() - pi;
						float tdist = LENGTH(L);
						L *= (1.0f / tdist);
						Ray r = Ray(pi + L * EPSILON, L);
						for (int s = 0; s < m_Scene->GetNrPrimitives(); s++)
						{
							Primitive* pr = m_Scene->GetPrimitive(s);
							if ((pr != light) && (pr->Intersect(r, tdist)))
							{
								shade = 0;
								break;
							}
						}
					}
					if (shade > 0)
					{
						// calculate diffuse shading
						vector3 L = ((Sphere*)light)->GetCentre() - pi;
						NORMALIZE(L);
						vector3 N = prim->GetNormal(pi);
						if (prim->GetMaterial()->GetDiffuse() > 0)
						{
							float dot = DOT(L, N);
							if (dot > 0)
							{
								float diff = dot * prim->GetMaterial()->GetDiffuse() * shade;
								// add diffuse component to ray color
								a_Acc += diff * prim->GetMaterial()->GetColor() * light->GetMaterial()->GetColor();
							}
						}
						// determine specular component
						if (prim->GetMaterial()->GetSpecular() > 0)
						{
							// point light source: sample once for specular highlight
							vector3 V = a_Ray.GetDirection();
							vector3 R = L - 2.0f * DOT(L, N) * N;
							float dot = DOT(V, R);
							if (dot > 0)
							{
								float spec = powf(dot, 20) * prim->GetMaterial()->GetSpecular() * shade;
								// add specular component to ray color
								a_Acc += spec * light->GetMaterial()->GetColor();
							}
						}
					}
				}
			}
			// calculate reflection
			float refl = prim->GetMaterial()->GetReflection();
			if ((refl > 0.0f) && (a_Depth < TRACEDEPTH))
			{
				vector3 N = prim->GetNormal(pi);
				vector3 R = a_Ray.GetDirection() - 2.0f * DOT(a_Ray.GetDirection(), N) * N;
				Color rcol(0, 0, 0);
				float dist;
				Raytrace(Ray(pi + R * EPSILON, R), rcol, a_Depth + 1, a_RIndex, dist);
				a_Acc += refl * rcol * prim->GetMaterial()->GetColor();
			}
			// calculate refraction
			float refr = prim->GetMaterial()->GetRefraction();
			if ((refr > 0) && (a_Depth < TRACEDEPTH))
			{
				float rindex = prim->GetMaterial()->GetRefrIndex();
				float n = a_RIndex / rindex;
				vector3 N = prim->GetNormal(pi) * (float)result;
				float cosI = -DOT(N, a_Ray.GetDirection());
				float cosT2 = 1.0f - n * n * (1.0f - cosI * cosI);
				if (cosT2 > 0.0f)
				{
					vector3 T = (n * a_Ray.GetDirection()) + (n * cosI - sqrtf(cosT2)) * N;
					Color rcol(0, 0, 0);
					float dist;
					Raytrace(Ray(pi + T * EPSILON, T), rcol, a_Depth + 1, rindex, dist);
					// apply Beer's law
					Color absorbance = prim->GetMaterial()->GetColor() * 0.15f * -dist;
					Color transparency = Color(expf(absorbance.r), expf(absorbance.g), expf(absorbance.b));
					a_Acc += rcol * transparency;
				}
			}
		}
		// return pointer to primitive hit by primary ray
		return prim;
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
		m_Refl(0), m_Diff(0.2f), m_Spec(0.8f), m_RIndex(1.5f)
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
		m_Primitive = new Primitive*[500];
		// ground plane
		m_Primitive[0] = new PlanePrim(vector3(0, 1, 0), 4.4f);
		m_Primitive[0]->SetName("plane");
		m_Primitive[0]->GetMaterial()->SetReflection(0.0f);
		m_Primitive[0]->GetMaterial()->SetRefraction(0.0f);
		m_Primitive[0]->GetMaterial()->SetDiffuse(1.0f);
		m_Primitive[0]->GetMaterial()->SetColor(Color(0.4f, 0.3f, 0.3f));
		// big sphere
		m_Primitive[1] = new Sphere(vector3(2, 0.8f, 3), 2.5f);
		m_Primitive[1]->SetName("big sphere");
		m_Primitive[1]->GetMaterial()->SetReflection(0.2f);
		m_Primitive[1]->GetMaterial()->SetRefraction(0.8f);
		m_Primitive[1]->GetMaterial()->SetRefrIndex(1.3f);
		m_Primitive[1]->GetMaterial()->SetColor(Color(0.7f, 0.7f, 1.0f));
		// small sphere
		m_Primitive[2] = new Sphere(vector3(-5.5f, -0.5, 7), 2);
		m_Primitive[2]->SetName("small sphere");
		m_Primitive[2]->GetMaterial()->SetReflection(0.5f);
		m_Primitive[2]->GetMaterial()->SetRefraction(0.0f);
		m_Primitive[2]->GetMaterial()->SetRefrIndex(1.3f);
		m_Primitive[2]->GetMaterial()->SetDiffuse(0.1f);
		m_Primitive[2]->GetMaterial()->SetColor(Color(0.7f, 0.7f, 1.0f));
		// light source 1
		m_Primitive[3] = new Sphere(vector3(0, 5, 5), 0.1f);
		m_Primitive[3]->Light(true);
		m_Primitive[3]->GetMaterial()->SetColor(Color(0.4f, 0.4f, 0.4f));
		// light source 2
		m_Primitive[4] = new Sphere(vector3(-3, 5, 1), 0.1f);
		m_Primitive[4]->Light(true);
		m_Primitive[4]->GetMaterial()->SetColor(Color(0.6f, 0.6f, 0.8f));
		// extra sphere
		m_Primitive[5] = new Sphere(vector3(-1.5f, -3.8f, 1), 1.5f);
		m_Primitive[5]->SetName("extra sphere");
		m_Primitive[5]->GetMaterial()->SetReflection(0.0f);
		m_Primitive[5]->GetMaterial()->SetRefraction(0.8f);
		m_Primitive[5]->GetMaterial()->SetColor(Color(1.0f, 0.4f, 0.4f));
		// back plane
		m_Primitive[6] = new PlanePrim(vector3(0.4f, 0, -1), 12);
		m_Primitive[6]->SetName("back plane");
		m_Primitive[6]->GetMaterial()->SetReflection(0.0f);
		m_Primitive[6]->GetMaterial()->SetRefraction(0.0f);
		m_Primitive[6]->GetMaterial()->SetSpecular(0);
		m_Primitive[6]->GetMaterial()->SetDiffuse(0.6f);
		m_Primitive[6]->GetMaterial()->SetColor(Color(0.5f, 0.3f, 0.5f));
		// ceiling plane
		m_Primitive[7] = new PlanePrim(vector3(0, -1, 0), 7.4f);
		m_Primitive[7]->SetName("back plane");
		m_Primitive[7]->GetMaterial()->SetReflection(0.0f);
		m_Primitive[7]->GetMaterial()->SetRefraction(0.0f);
		m_Primitive[7]->GetMaterial()->SetSpecular(0);
		m_Primitive[7]->GetMaterial()->SetDiffuse(0.5f);
		m_Primitive[7]->GetMaterial()->SetColor(Color(0.4f, 0.7f, 0.7f));
		// grid
		int prim = 8;
		for (int x = 0; x < 8; x++) for (int y = 0; y < 7; y++)
		{
			m_Primitive[prim] = new Sphere(vector3(-4.5f + x * 1.5f, -4.3f + y * 1.5f, 10), 0.3f);
			m_Primitive[prim]->SetName("grid sphere");
			m_Primitive[prim]->GetMaterial()->SetReflection(0);
			m_Primitive[prim]->GetMaterial()->SetRefraction(0);
			m_Primitive[prim]->GetMaterial()->SetSpecular(0.6f);
			m_Primitive[prim]->GetMaterial()->SetDiffuse(0.6f);
			m_Primitive[prim]->GetMaterial()->SetColor(Color(0.3f, 1.0f, 0.4f));
			prim++;
		}
		// set number of primitives
		m_Primitives = prim;
	}

}; // namespace JBikker