#pragma once

namespace sqr
{
/// äÖÈ¾Æ÷
class CRendererBase
{
public:
	CRendererBase();
	virtual ~CRendererBase();
	virtual void Render() = 0;
};

}// namespace sqr
