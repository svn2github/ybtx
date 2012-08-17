#pragma once
#include "CUIBase.h"

#define BASEUI_TITLE_HEIGHT		30
#define BASEUI_TITLE_WIDTH		120

namespace sqr_tools {

	public ref class CUIPropBase : public CUIBase
	{
	public:
		CUIPropBase(void);
		~CUIPropBase(void);

	public:
		virtual void			SetContentData(void* content);


	};

}