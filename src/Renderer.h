#pragma once

#include "Defines.h"
#include "Vector2.h"
#include "Matrix4.h"
#include "Triangle.h"
#include "Texture.h"
#include "WavefrontOBJ.h"

class CRenderer
{
public:
	static CRenderer *GetSingleton();
private:
	static CRenderer *singleton;
	
public:
	bool32 Init(int32 screen_width, int32 screen_height);
	void BeginDrawing();
	void EndDrawing();

	void ClearBuffers();

	void Plot(int x, int y, Color c);
	bool CheckAndUpdateDepthBuffer(int x, int y, float d);
	void DrawLine(V2 a, V2 b, Color color);
	void DrawSquare(V2 p, float w, float h, Color c);

	void DrawTriangle(CTriangle *t, Color color);
	void DrawTriangleTextured(CTriangle *t, CTexture *tex);

	void DrawTextureStraight(CTexture *t, V2 loc);

	void DrawOBJ(CWavefrontOBJ *obj, CTexture *tex);

	void ProjectTriangle(V2 *a, V2 *b, V2 *c, CTriangle *t);
private:

	int32 window_width, window_height;
	int32 screen_width, screen_height;
	void *pixels;

	float *depth_buffer;
	
	CMatrix4 projection;
	CMatrix4 modelview;

	bool wireframe;
	bool depth;
	bool antialiazing;
};