#pragma once

namespace sqr
{
	typedef TPos<float> CFPos;
}

void CalTriangleVertex(const CFPos& posFrom, const CFPos& posTo, CFPos& posBegin, CFPos& posLeft, CFPos& posRight);

