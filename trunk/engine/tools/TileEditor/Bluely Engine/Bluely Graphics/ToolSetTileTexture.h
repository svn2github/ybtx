//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyG.h"
#include "Input.h"

namespace bg
{
namespace sc
{

class CToolSetTileTexture
{
//	COperator::SProcessParams m_processParams;

public:
	CToolSetTileTexture();
	~CToolSetTileTexture();
	void OnEvent(ip::CInputDevice::SEvent & event);
	void Render();
};

}
}