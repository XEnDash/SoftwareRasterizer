#pragma comment (lib, "SDL2.lib")
#pragma comment (lib, "SDL2main.lib")
#pragma comment (lib, "SDL2_image.lib")

#include <Windows.h>
#include "Log.h"
#include "Platform.h"
#include "Renderer.h"
#include "HighResTimer.h"
#include "String.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define FPS 60.0

#define FILE_STRING_LIMIT 128

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	char logfile[128];
	int bytes = GetModuleFileName(NULL, logfile, 96);
	if (bytes == 0)
		return -1;
	
	StringCopy(&logfile[bytes - 26], "log.txt"); // HACK(danile): 26 is the length of "SoftwareTextureMapping.exe"

	if (!CLog::Init(logfile))
		return false;
	
	CPlatform *platform = CPlatform::GetSingleton();
	
	if (!platform->Init(WINDOW_WIDTH, WINDOW_HEIGHT))
		return -1;

	CRenderer *renderer = CRenderer::GetSingleton();

	if (!renderer->Init(SCREEN_WIDTH, SCREEN_HEIGHT))
		return -1;

	// HACK(daniel): for debugging
	if (*lpCmdLine == 0)
		lpCmdLine = "../data/dog.obj ../data/dog.tga";
	
	char model_file[FILE_STRING_LIMIT];
	model_file[0] = 0;

	char tex_file[FILE_STRING_LIMIT];
	tex_file[0] = 0;
	
	if (*lpCmdLine != 0)
	{
		int count = 0;

		char *s = lpCmdLine;
		char *d = model_file;
		while (*s != ' ' && *s != 0)
		{
			*d = *s;

			s++;
			d++;

			count++;
			if (count >= FILE_STRING_LIMIT - 1)
				return -1;
		}

		if (*s == 0)
			return -1;

		s++;

		model_file[count] = 0;

		d = tex_file;

		count = 0;

		while (*s != ' ' && *s != 0)
		{
			*d = *s;

			s++;
			d++;

			count++;
			if (count >= FILE_STRING_LIMIT - 1)
				return -1;
		}

		tex_file[count] = 0;
	}
	
	CWavefrontOBJ model;
	if (!model.Load(model_file))
		return -1;

	CTexture tex;
	if (!tex.Load(tex_file))
		return -1;

	uint64 old_time = platform->GetTime();
	uint64 cur_time = platform->GetTime();
	double time_elapsed = 0.0;

	double sleep = 0;

	double desired_fps = 1000.0 / FPS;

	while (platform->GetRunning())
	{
		cur_time = platform->GetTime();

		platform->HandleEvents();

		time_elapsed = (double)(cur_time - old_time);
		sleep = desired_fps - time_elapsed;

		if (sleep > 0)
		{
			platform->Sleep(sleep);
			//time_elapsed = 1000.0 / FPS; // IMPORTANT(daniel): is it ok if we do this? this will make the time_elapsed of every frame that finished faster then 16.6ms a fixed value of 16.6
		}
		
		platform->SetDT(time_elapsed * (FPS / 1000.0));
		//CLog::FLog("FPS: %f", (FPS * 1000.0 / (time_elapsed * FPS)));
		
		renderer->BeginDrawing();

		renderer->DrawOBJ(&model, &tex);
		
		renderer->EndDrawing();

		platform->UpdateWindow();

		old_time = cur_time;
	}

	platform->Quit();
	
	return 0;
}