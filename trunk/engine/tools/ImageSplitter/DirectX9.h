#pragma once
#include "BaseDefs.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxguid.lib")

// ---- ½Ó¿ÚÉùÃ÷ ------------------------------------------------------------------------------

	bool InitGraphicsDeviceDX9(GraphicDeviceDesc *pDesc=NULL);
	void ReleaseGraphicsDeviceDX9(void);
	bool ResetGraphicsDeviceDX9(void);

	LPDIRECT3DDEVICE9 GetGraphicsDeviceDX9(void);