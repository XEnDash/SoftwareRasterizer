#pragma comment (lib, "SDL2.lib")
#pragma comment (lib, "SDL2main.lib")
#pragma comment (lib, "SDL2_image.lib")

#include <Windows.h>
#include "Log.h"
#include "Platform.h"
#include "Renderer.h"
#include "HighResTimer.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define FPS 60.0

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (!CLog::Init("log.txt"))
		return false;
	
	CPlatform *platform = CPlatform::GetSingleton();
	
	if (!platform->Init(WINDOW_WIDTH, WINDOW_HEIGHT))
		return -1;

	CRenderer *renderer = CRenderer::GetSingleton();

	if (!renderer->Init(SCREEN_WIDTH, SCREEN_HEIGHT))
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

		renderer->EndDrawing();

		platform->UpdateWindow();

		old_time = cur_time;
	}

	platform->Quit();
	
	return 0;
}