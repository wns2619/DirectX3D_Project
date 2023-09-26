#pragma once

#include "Types.h"

namespace Engine
{
	static Vec3 Unproject(const Vec3& pos, const Matrix& W, const Matrix& V, const Matrix& P, _float& winSizeX, _float& _winSizeY)
	{
		Vec3 p = pos;

		p.x = 2.f * (p.x - 0) / winSizeX - 1.f;
		p.y = -2.f * (p.y - 0) / winSizeX + 1.f;
		p.z = ((p.z - 0) / (1 - 0));

		Matrix wvp = W * V * P;
		Matrix wvpInv = wvp.Invert();

		p = Vec3::Transform(p, wvpInv);
		return p;
	}

}