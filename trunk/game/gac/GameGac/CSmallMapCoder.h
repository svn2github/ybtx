#pragma once
#include "CElementManager.h"

namespace sqr{
class CDataScene;
}

class CSmallMapCoder : public CElementCoderEx
{
public:
	bool	_CElementDeCode( const DeCodeParams& Params );
};
