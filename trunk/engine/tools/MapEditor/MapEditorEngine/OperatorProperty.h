#pragma once
#include "TSingleton.h"

namespace sqr
{
	///operator中抽出的变量
	class COperatorPropertyCtrl
	{
	public:

		HWND hMainFrame;
		HWND hParamsView;
		HWND hParamsExtraView;
		HWND hResSetView;
		HWND hFXSetView;
		HWND hTextureView;
		HWND hMeshView;
		HWND hVertexView;
		HWND hLoginSetView;
		HWND hRecordView;
		HWND hAudioView;
		HWND hRegionView;
		HWND hTileRegionView;

		bool bEnableRecordCtrl;		// 录像快捷键（数字键）是否有效
		bool bSnapMode;				// 是否显示纹理框

	public:
		COperatorPropertyCtrl()
		{
			bEnableRecordCtrl = true;
			bSnapMode		  = false;
		}

		~COperatorPropertyCtrl()
		{

		}
	};
}