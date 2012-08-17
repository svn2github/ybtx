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

class CToolSetTileHeight
{
public:
	CToolSetTileHeight();
	~CToolSetTileHeight();
	void OnEvent(ip::CInputDevice::SEvent & event);
	void Render();
};

}
}