#pragma once

#include "CCoreServerResult.h"

namespace sqr
{

	class CCoreServerAppQuitResult : public CCoreServerResult
	{
	public:
		CCoreServerAppQuitResult();
		~CCoreServerAppQuitResult();
	};

	class CCoreServerAppCleanResult : public CCoreServerResult
	{
	public:
		CCoreServerAppCleanResult();
		~CCoreServerAppCleanResult();
	};
}
