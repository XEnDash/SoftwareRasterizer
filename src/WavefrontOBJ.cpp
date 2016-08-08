#include "WavefrontOBJ.h"

#include "Platform.h"

bool CWavefrontOBJ::Load(char *file)
{
	CPlatform *platform = CPlatform::GetSingleton();

	uint32 fsize = -1;
	void *fdata = platform->ReadFromFile(file, &fsize);

	if (fsize <= 0 || fdata == 0)
		return false;

	int verts_slice = 2048;
	V3 *verts = (V3 *)lnew(sizeof(V3) * verts_slice);
	int verts_num = 0;
	int verts_num_of_reallocs = 1;

	int texs_slice = 2048;
	V2 *texs = (V2 *)lnew(sizeof(V2) * verts_slice);
	int texs_num = 0;
	int texs_num_of_reallocs = 1;

	int tris_slice = 2048;
	tris = (CTriangle *)lnew(sizeof(CTriangle) * tris_slice);
	tris_num = 0;
	int tris_num_of_reallocs = 1;

	char *ptr = (char *)fdata;
	while (*ptr != 0)
	{
		switch (*ptr)
		{
			case '\n':
			{
				ptr++;
			} break;

			case 'o':
			case '#':
			{
				while (*ptr != '\n')
				{
					ptr++;
				}
			} break;
			
			case 'v':
			{
				ptr++;

				switch (*ptr)
				{
					case ' ':
					{
					float va, vb, vc;
					sscanf_s(ptr, "%f %f %f\n", &va, &vb, &vc);
					
					verts[verts_num].x = va;
					verts[verts_num].y = vb;
					verts[verts_num].z = vc;
					verts_num++;

					if (verts_num % verts_slice == 0)
					{
						verts_num_of_reallocs++;
						verts = (V3 *)lrealloc(verts, sizeof(V3) * verts_slice * verts_num_of_reallocs);
					}
					} break;

					case 'n':
					{

					} break;
					
					case 't':
					{
						ptr++;

						float ta, tb;
						sscanf_s(ptr, "%f %f\n", &ta, &tb);

						texs[texs_num].x = ta;
						texs[texs_num].y = tb;
						texs_num++;
						
						if (texs_num % texs_slice == 0)
						{
							texs_num_of_reallocs++;
							texs = (V2 *)lrealloc(texs, sizeof(V2) * texs_slice * texs_num_of_reallocs);
						}
					} break;
				}
				
				while (*ptr != '\n')
				{
					ptr++;
				}
				
			} break;

			case 's':
			{
				while (*ptr != '\n')
				{
					ptr++;
				}
			} break;
			
			case 'f':
			{
				ptr++;

				int vi1, vi2, vi3;
				int ti1, ti2, ti3;
				int ni1, ni2, ni3;
				
				/*sscanf_s(ptr, " %i/", &vi1); ptr++; ptr++; ptr++;
				sscanf_s(ptr, "%i/", &ti1); ptr++; ptr++;
				sscanf_s(ptr, "%i", &ni1); ptr++; ptr++;

				sscanf_s(ptr, "%i/", &vi2); ptr++; ptr++;
				sscanf_s(ptr, "%i/", &ti2); ptr++; ptr++;
				sscanf_s(ptr, "%i", &ni2); ptr++; ptr++;
				
				sscanf_s(ptr, "%i/", &vi3); ptr++; ptr++;
				sscanf_s(ptr, "%i/", &ti3); ptr++; ptr++; 
				sscanf_s(ptr, "%i", &ni3); ptr++;*/

				sscanf_s(ptr, " %i/%i/%i %i/%i/%i %i/%i/%i", &vi1, &ti1, &ni1, &vi2, &ti2, &ni2, &vi3, &ti3, &ni3);
				ptr += 18;

				vi1 -= 1;
				vi2 -= 1;
				vi3 -= 1;

				ti1 -= 1;
				ti2 -= 1;
				ti3 -= 1;

				V3 a = verts[vi1];
				V3 b = verts[vi2];
				V3 c = verts[vi3];

				V2 a_tex = texs[ti1];
				V2 b_tex = texs[ti2];
				V2 c_tex = texs[ti3];

				tris[tris_num].a = a;
				tris[tris_num].b = b;
				tris[tris_num].c = c;

				//tris[tris_num].a_tex = a_tex;
				//tris[tris_num].b_tex = b_tex;
				//tris[tris_num].c_tex = c_tex;
				
				// HACK(daniel): texture coordinates are upside down, we fix this here
				tris[tris_num].a_tex = V2(a_tex.x, 1.0f - a_tex.y);
				tris[tris_num].b_tex = V2(b_tex.x, 1.0f - b_tex.y);
				tris[tris_num].c_tex = V2(c_tex.x, 1.0f - c_tex.y);

				tris_num++;

				if (tris_num % tris_slice == 0)
				{
					tris_num_of_reallocs++;
					tris = (CTriangle *)lrealloc(tris, sizeof(CTriangle) * tris_slice * tris_num_of_reallocs);
				}
			} break;

			default:
			{
				ptr++;
			} break;
		}
	}

	ldelete(verts);
	ldelete(texs);

	return true;
}