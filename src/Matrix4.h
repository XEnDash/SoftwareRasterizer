#pragma once

#include "Defines.h"

class CMatrix4
{
public:

	CMatrix4() { CreateIdentity(); }

	void CreateIdentity();
	void CreatePerspective(float fov, float ar, float near, float far);

	void CreateTranslation(float x, float y, float z);
	void CreateScale(float x, float y, float z);
	
	void CreateRotationX(float angle);
	void CreateRotationY(float angle);
	void CreateRotationZ(float angle);

	void Multiply(CMatrix4 &b);

	float m11, m12, m13, m14;
	float m21, m22, m23, m24;
	float m31, m32, m33, m34;
	float m41, m42, m43, m44;
};
