#include "Renderer.h"

#include "Platform.h"
#include "Assert.h"

CRenderer *CRenderer::singleton = 0;

CRenderer *CRenderer::GetSingleton()
{
	if (singleton == 0)
	{
		singleton = (CRenderer *)lnew(sizeof(CRenderer));
	}

	return singleton;
}

bool32 CRenderer::Init(int32 screen_width, int32 screen_height)
{
	Assert((screen_width <= 0 || screen_height <= 0), "screen_width or screen_height negative");

	CPlatform *platform = CPlatform::GetSingleton();

	SDL_PixelFormat *format;
	this->pixels = platform->GetWindowSurfacePixels(&window_width, &window_height, &format);
	
	Assert((window_width <= 0 || window_height <= 0), "window_width or window_height negative");

	if (format->format != SDL_PIXELFORMAT_RGB888)
	{
		CLog::FLog("Wrong pixel format for window surface");
		return false;
	}

	depth_buffer = (float *)lnew(sizeof(float) * window_width * window_height);
	
	projection.CreateIdentity();
	projection.CreatePerspective(90.0f, window_width, window_height, 1.0f, 1000.0f);
	
	modelview.CreateIdentity();
	
	wireframe = false;
	depth = false;
	antialiazing = true;
	
	return true;
}

void CRenderer::BeginDrawing()
{
	ClearBuffers();

	Gamepad *gamepad = CPlatform::GetSingleton()->GetGamepad();
	Gamepad *prev_gamepad = CPlatform::GetSingleton()->GetPrevGamepad();
	
	if (gamepad->ew && !prev_gamepad->ew)
		wireframe = !wireframe;

	if (gamepad->ed && !prev_gamepad->ed)
		depth = !depth;

	if (gamepad->ea && !prev_gamepad->ea)
		antialiazing = !antialiazing;
}

void CRenderer::EndDrawing()
{
	if (depth)
	{
		float *d = (float *)depth_buffer;
		for (int x = 0; x < window_width; x++)
		{
			for (int y = 0; y < window_height; y++)
			{
				float depth = d[x + y * window_width];

				float c = depth * 255.0f;

				if (depth > 255)
					c = 255;
				if (depth < 0)
					c = 0;

				Color color;

				color.r = color.g = color.b = c;

				Plot(x, y, color);
			}
		}
	}
}

void CRenderer::ClearBuffers()
{
	uint32 *p = (uint32 *)pixels;
	for (int x = 0; x < window_width; x++)
	{
		for (int y = 0; y < window_height; y++)
		{
			p[x + y * window_width] = RGBtob32(0, 0, 0);
		}
	}

	for (int x = 0; x < window_width; x++)
	{
		for (int y = 0; y < window_height; y++)
		{
			depth_buffer[x + y * window_width] = INFINITY;
		}
	}
}

void CRenderer::Plot(int x, int y, Color c)
{
	if (x < 0 || y < 0 || x >= window_width || y >= window_height)
		return;

	Clamp(c.r, 0, 255);
	Clamp(c.g, 0, 255);
	Clamp(c.b, 0, 255);
	
	uint32 *p = (uint32 *)pixels;
	p[x + y * window_width] = RGBtob32(c.r, c.g, c.b);
}

bool CRenderer::CheckAndUpdateDepthBuffer(int x, int y, float d)
{
	if (x < 0 || y < 0 || x >= window_width || y >= window_height)
		return false;
	
	float *p = (float *)depth_buffer;
	if (p[x + y * window_width] > d)
	{
		p[x + y * window_width] = d;
		return true;
	}
	else
	{
		return false;
	}
}

void CRenderer::DrawLine(V2 a, V2 b, Color color)
{
	int x1 = a.x;
	int y1 = a.y;
	int x2 = b.x;
	int y2 = b.y;

	int dx = abs(x2 - x1), sx = x1<x2 ? 1 : -1;
	int dy = abs(y2 - y1), sy = y1<y2 ? 1 : -1;
	int err = (dx>dy ? dx : -dy) / 2, e2;

	for (;;)
	{
		Plot(x1, y1, color);

		if (x1 == x2 && y1 == y2)
			break;

		e2 = err;

		if (e2 >-dx)
		{
			err -= dy; x1 += sx;
		}
		if (e2 < dy)
		{
			err += dx; y1 += sy;
		}
	}
}

void CRenderer::DrawSquare(V2 p, float w, float h, Color c)
{
	for (int x = p.x - w / 2; x < p.x + w / 2; x++)
	{
		for (int y = p.y - h / 2; y < p.y + h / 2; y++)
		{
			Plot(x, y, c);
		}
	}
}

void CRenderer::DrawTriangle(CTriangle *t, Color color)
{
	V2 a, b, c;
	ProjectTriangle(&a, &b, &c, t);
	
	if (wireframe)
	{
		DrawLine(a, b, color);
		DrawLine(b, c, color);
		DrawLine(c, a, color);

		return;
	}
	
	float min_x, min_y;
	float max_x, max_y;
	
	min_x = min3(a.x, b.x, c.x);
	min_y = min3(a.y, b.y, c.y);
	max_x = max3(a.x, b.x, c.x);
	max_y = max3(a.y, b.y, c.y);
	
	for (int x = min_x; x < max_x; x++)
	{
		for (int y = min_y; y < max_y; y++)
		{
			V2 point = V2(x, y);
			
			float u = 0, v = 0, w = 0;
			BarycentricCoords(a, b, c, point, &u, &v, &w);
			
			if (u >= 0 && v >= 0 && u + v < 1)
			{
				float one_over_depth = (1.0f / t->a.z) * u + (1.0f / t->b.z) * v + (1.0f / t->c.z) * w;
				float depth = one_over_depth;
				
				if (CheckAndUpdateDepthBuffer(x, y, depth))
				{
					Plot(x, y, color);
				}
			}
		}
	}
}

void CRenderer::DrawTriangleTextured(CTriangle *t, CTexture *tex)
{
	// HACK(daniel): weird things happen when we get close to zero
	if (t->a.z < 3 || t->b.z < 3 || t->c.z < 3)
		return;
	
	V2 a, b, c;
	ProjectTriangle(&a, &b, &c, t);

	if (wireframe)
	{
		//Color color = tex->pixels[0];
		Color color = Color::white;

		DrawLine(a, b, color);
		DrawLine(b, c, color);
		DrawLine(c, a, color);

		return;
	}

	float min_x, min_y;
	float max_x, max_y;

	min_x = min3(a.x, b.x, c.x);
	min_y = min3(a.y, b.y, c.y);
	max_x = max3(a.x, b.x, c.x);
	max_y = max3(a.y, b.y, c.y);
	
	for (int x = min_x; x < max_x; x++)
	{
		for (int y = min_y; y < max_y; y++)
		{
			V2 point = V2(x, y);

			float u = 0, v = 0, w = 0;
			BarycentricCoords(a, b, c, point, &u, &v, &w);
			
			if (u >= 0 && v >= 0 && u + v < 1)
			{
				V2 point_tex = t->a_tex * u + t->b_tex * v + t->c_tex * w;

				point_tex.x *= tex->w;
				point_tex.y *= tex->h;

				int tx = point_tex.x;
				int ty = point_tex.y;

				if (tx >= 0 && tx < tex->w &&
					ty >= 0 && ty < tex->h)
				{
					float one_over_depth = (1.0f / t->a.z) * u + (1.0f / t->b.z) * v + (1.0f / t->c.z) * w;
					float depth = one_over_depth;

					if (CheckAndUpdateDepthBuffer(x, y, depth))
					{
						Color color = tex->pixels[tx + ty * tex->w];
						if (antialiazing)
						{
							if (tx + 1 < tex->w && ty + 1 < tex->h)
							{
								color += tex->pixels[(tx + 1) + ty * tex->w];
								color += tex->pixels[tx + (ty + 1) * tex->w];
								color += tex->pixels[(tx + 1) + (ty + 1) * tex->w];
								
								color.r /= 4;
								color.g /= 4;
								color.b /= 4;
							}
						}
						
						Plot(x, y, color);
					}
				}
			}
		}
	}
}

void CRenderer::DrawTextureStraight(CTexture *t, V2 loc)
{
	for (int x = 0; x < t->w; x++)
	{
		for (int y = 0; y < t->h; y++)
		{
			Plot(loc.x + x , loc.y + y, t->pixels[x + y * t->w]);
		}
	}
}

void CRenderer::DrawOBJ(CWavefrontOBJ *obj, CTexture *tex)
{
	CPlatform *platform = CPlatform::GetSingleton();
	// NOTE(daniel): most of the variables are for testing
	double dt = platform->GetDT();

	Gamepad *gamepad = platform->GetGamepad();

	static float a_x = 0, a_y = 0, a_z = 0;

	static float x = 0, y = 0, z = 25;

	float speed = 0.1f * dt;
	if (gamepad->u)
		y += speed;
	if (gamepad->d)
		y -= speed;

	if (gamepad->l)
		x += speed;
	if (gamepad->r)
		x -= speed;

	float rspeed = M_PI / 180.0f * dt;
	if (gamepad->su)
		z += speed;
	if (gamepad->sd)
		z -= speed;

	if (gamepad->sl)
		a_y += speed;
	if (gamepad->sr)
		a_y -= speed;

	if (z < 1)
		z = 1;
	
	CMatrix4 rot_x, rot_y, rot_z;
	CMatrix4 rot_origin;
	CMatrix4 scl;
	CMatrix4 pos;

	rot_x.CreateRotationX(a_x);
	rot_y.CreateRotationY(a_y);
	rot_z.CreateRotationZ(a_z);

	rot_origin.CreateTranslation(0, 0, 0);

	scl.CreateScale(0.1f, 0.1f, 0.1f);
	pos.CreateTranslation(x, y, z);
	
	modelview.CreateIdentity();
	modelview.Multiply(rot_origin);
	modelview.Multiply(rot_x);
	modelview.Multiply(rot_y);
	modelview.Multiply(rot_z);
	modelview.Multiply(scl);
	modelview.Multiply(pos);

	for (int i = 0; i < obj->tris_num; i++)
	{
		CTriangle t = obj->tris[i];

		t.Transform(modelview);

		DrawTriangleTextured(&t, tex);
		//DrawTriangle(&t, Color::red);
	}
}

void CRenderer::ProjectTriangle(V2 *a, V2 *b, V2 *c, CTriangle *t)
{
	//CTriangle temp = *t;
	//temp.Transform(projection);

	//*a = temp.a.ToV2();
	//*b = temp.b.ToV2();
	//*c = temp.c.ToV2();

	t->Transform(projection);

	*a = t->a.ToV2() / t->a.z;
	*b = t->b.ToV2() / t->b.z;
	*c = t->c.ToV2() / t->c.z;
	
	*a *= V2(window_width, window_height);
	*b *= V2(window_width, window_height);
	*c *= V2(window_width, window_height);

	*a += V2(window_width / 2, window_height / 2);
	*b += V2(window_width / 2, window_height / 2);
	*c += V2(window_width / 2, window_height / 2);
}