#include "stdafx.h"
#include "CDx9YYAdapter.h"
#include "../Graphic/Graphic_dx9/CDx9MainWindow.h"

 void*	GetDx9UIParam(void)
 {
	 return (void*)GetD3DDevice();
 }