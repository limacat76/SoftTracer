#include "../Graphics/Graphics.h"
#include "../Interface/Mailbox.h"
#include "../Interface/Parameters.h"
#include "Raytracer.h"

void Raytracer::initialize_scene() {
	// Bands does not need to initialize a scene...
}

/**
In mathematics, the dot product or scalar product[note 1] is an algebraic operation that takes two equal - length sequences of numbers(usually coordinate vectors) and returns a single number.

https://math.stackexchange.com/questions/77/understanding-dot-and-cross-product

http://immersivemath.com/ila/ch03_dotproduct/ch03.html
In the ray tracing program above, the dot product was used to calculate the intersection between a ray and a sphere, and the dot product was also used to measure the length to the intersection points.In addition, the law of reflection, which is used to calculate reflective objects, was implemented using dot products.

https://www.mathsisfun.com/algebra/vectors-dot-product.html
*/
void * dot_product;

/**
https://www.khanacademy.org/computing/computer-programming/programming-natural-simulations/programming-vectors/a/vector-magnitude-normalization
Normalizing refers to the process of making something “standard” or , well, “normal.” In the case of vectors, let’s assume for the moment that a standard vector has a length of 1. To normalize a vector, therefore, is to take a vector of any length and, keeping it pointing in the same direction, change its length to 1, turning it into what is called a unit vector.

https://stackoverflow.com/a/7020605/
you never need to normalize a vector unless you are working with the angles between vectors(in which case, all of your trig functions require your vectors to land perfectly on that unit circle), or unless you are rotating a vector(by applying a bunch of vectors to it, perhaps).In the latter case, you are dividing out the magnitude, rotating the vector, making sure it stays a unit, and then multiplying the magnitude back in.
...
The dot product only needs normalized vectors when you are using it to get the angle between two vectors.
*/
void * normalize;


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