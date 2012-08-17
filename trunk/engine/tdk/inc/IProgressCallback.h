#pragma once

namespace sqr_tools
{
	class IProgressCallback
	{
	public:
		virtual void SetProgress(float f) = 0;
		virtual float GetProgress() const = 0;
		virtual void SetDescription(const TCHAR* desc) = 0;
	};
}