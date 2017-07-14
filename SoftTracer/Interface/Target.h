#pragma once
#include <SDL.h>

class Target {
public:
	virtual void loop(bool &quit, int &frames, bool &allClosed) = 0;

	virtual void stop() = 0;
};

class Headless : public Target {
public:
	void loop(bool &quit, int &frames, bool &allClosed);

	void stop();
};

class SDLTarget : public Target {
protected:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	SDL_Event event;
	int remainingFrames = 60;
	bool copy = true;
	uint32_t* pixels;
	int width;
	int height;

public:
	SDLTarget(uint32_t* pixels, int width, int height);

	~SDLTarget();

	void loop(bool &quit, int &frames, bool &allClosed);

	void stop();
};
