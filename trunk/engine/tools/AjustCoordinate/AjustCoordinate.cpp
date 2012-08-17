// AjustCoordinate.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CodeCvs.h"
#include "CoordinateManager.h"
string sXmlPath;
string sLuaPath;
bool bGlobalUtf8;

int _tmain(int argc, _TCHAR* argv[])
{
	CoordinateManager corManager;
	corManager.ReadXml("Coordinate.xml");
	corManager.ReadCoordinateData();
	corManager.TranslateCoordinate();
	return 0;
}

