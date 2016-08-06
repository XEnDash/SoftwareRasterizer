#pragma once

#include "Defines.h"
#include <SDL.h>
#include <SDL_image.h>

class CPlatform
{
public:
	static CPlatform *GetSingleton();
private:
	static CPlatform *singleton;
	
public:
	bool32 Init(int32 width, int32 height);
	void HandleEvents();
	void UpdateWindow();
	void Quit();

	uint32 GetMouseState(int *x, int *y);

	void *GetWindowSurfacePixels(int *w, int *h, SDL_PixelFormat **format);

	bool32 LoadImageFile(char *filename, void **pixels, uint32 *w, uint32 *h);

	bool32 WriteToFile(char *filename, void *data, uint32 size);
	void *ReadFromFile(char *filename, uint32 *size);
	
	bool32 GetRunning() { return running; }
	void SetRunning(bool32 i) { running = i; }

	static int32 Random();

	static void *Allocate(uint64 size);
	static void *Reallocate(void *ptr, uint64 size);
	static void Free(void *ptr);

	uint32 GetTime();
	void Sleep(double sleep);
	
	void SetDT(double dt) { this->dt = dt; }
	double GetDT() { return this->dt; }
	
private:
	SDL_Window *sdl_window;
	SDL_Surface *sdl_window_surface;
	bool32 running;

	double dt;
};