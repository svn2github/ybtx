#pragma once
#include "CTexture.h"

class GUITextureHelper
{
public:
	virtual void InsertTexture(ITexture* pTexture) = 0;
	virtual bool IsAsynLoad() = 0;
};