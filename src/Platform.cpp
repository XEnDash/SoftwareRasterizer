#include "Platform.h"

#include "Assert.h"
#include "Log.h"

#include <windows.h>

#include <stdlib.h>
#include <time.h>

CPlatform *CPlatform::singleton = 0;

CPlatform *CPlatform::GetSingleton()
{
	if (singleton == 0)
	{
		singleton = (CPlatform *)lnew(sizeof(CPlatform));
	}

	return singleton;
}

bool32 CPlatform::Init(int32 width, int32 height)
{
	Assert((width <= 0 || height <= 0), "width or height negative");

	srand(time(0));

	if (SDL_Init(SDL_INIT_EVERYTHING) != NULL)
	{
		CLog::FLog("SDL_Init failed: %s", SDL_GetError());
		return false;
	}

	sdl_window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

	if (sdl_window == NULL)
	{
		CLog::FLog("SDL_CreateWindow failed: %s", SDL_GetError());
		return false;
	}

	sdl_window_surface = SDL_GetWindowSurface(sdl_window);
	
	if (!sdl_window_surface)
	{
		CLog::FLog("Failed to get windows surface: %s", SDL_GetError());
		return false;
	}

	if (!IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF))
	{
		CLog::FLog("IMG_Init failed: %s", IMG_GetError());
		return false;
	}
	
	dt = 1.0;

	running = true;
	
	return true;
}

void CPlatform::HandleEvents()
{
	SDL_Event ev;
	while (SDL_PollEvent(&ev))
	{
		switch (ev.type)
		{
			case SDL_QUIT:
			{
				SetRunning(false);
			} break;
		}
	}
}

void CPlatform::UpdateWindow()
{
	SDL_UpdateWindowSurface(sdl_window);
}

void CPlatform::Quit()
{
	IMG_Quit();

	SDL_DestroyWindow(sdl_window);

	SDL_Quit();
}

uint32 CPlatform::GetMouseState(int *x, int *y)
{
	return SDL_GetMouseState(x, y);
}

void *CPlatform::GetWindowSurfacePixels(int *w, int *h, SDL_PixelFormat **format)
{
	*w = sdl_window_surface->w;
	*h = sdl_window_surface->h;
	*format = sdl_window_surface->format;

	return sdl_window_surface->pixels;
}

bool32 CPlatform::LoadImageFile(char *filename, void **pixels, uint32 *w, uint32 *h)
{
	SDL_Surface *image = IMG_Load(filename);

	if (!image)
	{
		CLog::FLog("IMG_Load failed: %s", IMG_GetError());
		return false;
	}
	
	SDL_PixelFormat *format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBX8888);
	image = SDL_ConvertSurface(image, format, 0);
	SDL_FreeFormat(format);
	
	*w = image->w;
	*h = image->h;
	uint8 *pixels_out = (uint8 *)lnew(*w * *h * 4); // image->pixels get erased when we free the surface
	memcpy(pixels_out, image->pixels, *w * *h * 4);
	*pixels = pixels_out;
	
	SDL_FreeSurface(image);

	return true;
}

bool32 CPlatform::WriteToFile(char *filename, void *data, uint32 size)
{
	FILE *f;
	fopen_s(&f, filename, "w");

	return (fwrite(data, 1, size, f) == size);
}

void *CPlatform::ReadFromFile(char *filename, uint32 *size)
{
	FILE *f;
	fopen_s(&f, filename, "r");

	if (!f)
		return 0;

	fseek(f, 0, SEEK_END);
	*size = ftell(f);
	fseek(f, 0, SEEK_SET);

	void *data = lnew(*size);
	if (fread(data, 1, *size, f) != *size)
		data = 0;

	return data;
}

int32 CPlatform::Random()
{
	return rand();
}

void *CPlatform::Allocate(uint64 size)
{
	//return VirtualAlloc(0, size, MEM_COMMIT, PAGE_READWRITE);
	return malloc(size); // NOTE(daniel): using this can screw up C++ variable initialization when using singletons!!! but we really want realloc so we use this
}

void *CPlatform::Reallocate(void *ptr, uint64 size)
{
	//return 0;
	return realloc(ptr, size);
}

void CPlatform::Free(void *ptr)
{
	//VirtualFree(ptr, 0, MEM_RELEASE);
	free(ptr);
}

uint32 CPlatform::GetTime()
{
	return SDL_GetTicks();
}

void CPlatform::Sleep(double sleep)
{
	SDL_Delay(sleep);
}
