#include "Texture.h"

#include "Platform.h"

bool CTexture::Load(char *file)
{
	CPlatform *platform = CPlatform::GetSingleton();
	
	uint8 *ptr = 0;
	platform->LoadImageFile(file, (void **)(&ptr), &w, &h);

	if (ptr == 0)
		return false;

	pixels = (Color *)lnew(sizeof(Color) * w * h);

	for (int x = 0; x < w * 4; x += 4)
	{
		for (int y = 0; y < h * 4; y += 4)
		{
			pixels[x / 4 + y / 4 * w].r = ptr[x + y * w + 3];
			pixels[x / 4 + y / 4 * w].g = ptr[x + y * w + 2];
			pixels[x / 4 + y / 4 * w].b = ptr[x + y * w + 1];
		}
	}

	ldelete(ptr);
	
	bits_per_pixel = 24;

	return true;
}