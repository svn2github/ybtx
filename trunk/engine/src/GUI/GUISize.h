#pragma once
#include "SQRGuiMallocObject.h"
namespace sqr
{
	struct Size : public SQRGuiMallocObject
	{
		Size() {}
		Size(float width_in, float height_in) : width(width_in), height(height_in) {}

		bool operator==(const Size& rhs) const;
		bool operator!=(const Size& rhs) const;

		float width, height;
	};
}