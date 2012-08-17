#include "stdafx.h"
#include "GUISize.h"

namespace sqr
{
	bool Size::operator==( const Size& rhs ) const
	{
		return width == rhs.width && height == rhs.height;
	}

	bool Size::operator!=( const Size& rhs ) const
	{
		return !operator==(rhs);
	}
}