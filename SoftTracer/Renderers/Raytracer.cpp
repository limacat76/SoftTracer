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

JBEngine::JBEngine() {
#ifdef _DEBUG
	std::cout << "construtor called\n";
#endif
}

JBEngine::~JBEngine() {
#ifdef _DEBUG
	std::cout << "destructor called\n";
#endif
	delete my_scene;
}

void JBEngine::initialize_scene(const void* parameters) {
	const Parameters* my_parameters = (const Parameters *)parameters;
	width = my_parameters->width;
	height = my_parameters->height;


	my_scene = new Scene();
	my_scene->initialize_scene();
}

color calculateDiffuse(color start, float diffuse, color first, color second) {
	std::vector<float> a_start = Array(start);
	std::vector<float> a_first = Array(first);
	std::vector<float> a_second = Array(second);
	float r = a_start[0] + diffuse * a_first[0] * a_second[0];
	float g = a_start[1] + diffuse * a_first[1] * a_second[1];
	float b = a_start[2] + diffuse * a_first[2] * a_second[2];

	return Color(r, g, b);

}

// -----------------------------------------------------------
// Engine::Raytrace
// Naive ray tracing: Intersects the ray with every primitive
// in the scene to determine the closest intersection
// -----------------------------------------------------------
color JBEngine::Raytrace(Ray& a_Ray, int a_Depth, float a_RIndex, float& a_Dist) {
	color acc = Color(0, 0, 0);

	if (a_Depth > TRACE_DEPTH) return 0;
	// trace primary ray
	a_Dist = 1000000.0f;
	Eigen::Vector3f pi;
	Primitive* prim = 0;
	int result;
	// find the nearest intersection
	for (int s = 0; s < my_scene->get_primitives_number() ; s++) {

		Primitive* pr = my_scene->get_primitive(s);
		int res;
		if (res = pr->intersect(a_Ray, a_Dist)) {
			prim = pr;
			result = res; // 0 = miss, 1 = hit, -1 = hit from inside primitive
		}
	}
	// no hit, terminate ray
	if (!prim) return acc;
	// handle intersection
	if (prim->is_light()) {
		// we hit a light, stop tracing
		acc = Color(1, 1, 1);
	} else {
		// determine color at point of intersection
		pi = a_Ray.get_origin() + a_Ray.get_direction() * a_Dist;
		// trace lights
		for (int l = 0; l < my_scene->get_primitives_number(); l++) {
			Primitive* p = my_scene->get_primitive(l);
			if (p->is_light()) {
				Primitive* light = p;
				// calculate diffuse shading
				Eigen::Vector3f L = ((Sphere*)light)->get_centre() - pi;
				L.normalize();
				Eigen::Vector3f N = prim->get_normal(pi);
				Material local_material = prim->get_material();
				if (local_material.get_diffuse() > 0) {
					float dot = N.dot(L);
					if (dot > 0) {
						float diff = dot * local_material.get_diffuse();
						// add diffuse component to ray color
						acc = calculateDiffuse(acc, diff, prim->get_material().get_color(), light->get_material().get_color());
					}
				}
			}
		}
	}
	// return pointer to primitive hit by primary ray
	return acc;
}

void JBEngine::render(void* target, void* mailbox, const void* parameters) {
	//// screen plane in world space coordinates
	//m_WX1 = -4, m_WX2 = 4, m_WY1 = m_SY = 3, m_WY2 = -3;
	//// calculate deltas for interpolation
	//m_DX = (m_WX2 - m_WX1) / width;
	//m_DY = (m_WY2 - m_WY1) / height;
	//m_SY += 20 * m_DY;
	//// allocate space to store pointers to primitives for previous line
	//m_LastRow = new Primitive*[width];
	//memset(m_LastRow, 0, width * 4);

	pixel* image = (pixel*)target;

	MailBox* my_mailbox = (MailBox *)mailbox;
	const Parameters* my_parameters = (const Parameters *)parameters;

	const int thread_no = my_parameters->threadNumber;
	const int total_threads = my_parameters->totalThreads;
	const int band = my_parameters->height / total_threads;
	const int startY = thread_no * band;
	const int endY = thread_no == total_threads - 1 ? height : thread_no * band + band;

	//// set firts line to draw to
	//int m_CurrLine = 20;
	//// set pixel buffer address of first pixel
	//int m_PPos = 20 * width;

	my_mailbox->work_started(thread_no);
	pixel* currentPixel = image + (startY * width);

	// The Origin point
	Eigen::Vector3f origin(0, 0, -5);

	// Trace rays
	bool quit = false;
	for (int y = startY; !quit && y < endY; ++y) {
		for (int x = 0; !quit && x < width; ++x, ++currentPixel) {

			// fire primary ray
			Eigen::Vector3f dir = Eigen::Vector3f(x, y, 0) - origin;
			dir.normalize();
			Ray r(origin, dir);
			float dist;
			color acc = Raytrace(r,  1, 1.0f, dist);

			*currentPixel = acc;
			quit = my_mailbox->to_children_quit;
		}
	}

	my_mailbox->work_done(thread_no);
}





