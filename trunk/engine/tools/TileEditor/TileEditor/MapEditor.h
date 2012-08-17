// MapEditor.h : MapEditor 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h” 
#endif

#include "resource.h"       // 主符号

//#include <BluelyG.h>
//#include <BluelyC.h>
//#include <Timer.h>
//#include <FPSCounter.h>
//#include <Camera.h>
//#include <TerrainMesh.h>
//#include <Mouse.h>
//#include <SkyBox.h>
//#include <Operator.h>
//#include <GUI.h>
//#include <Object.h>

// CMapEditorApp:
// 有关此类的实现，请参阅 MapEditor.cpp
//

class CMapEditorApp : public CWinApp
{
private:
    
	//////////////////////////////////////////////////////////////////////////
	
	// scene oriented 
	
	//bly::COperator * m_pOperator;
	//bly::CTerrainMesh * m_pTerrain;
	//bly::CCamera * m_pCamera;
	
public:
	//bly::CTerrainMesh * GetT() { return m_pTerrain; }
	//bly::COperator * GetO() { return m_pOperator; }
	//bly::CTerrainMesh * SetT() { return m_pTerrain; }
	//bly::COperator * GetO() { return m_pOperator; }

	void BackToWorldView();

public:
	CMapEditorApp();
	~CMapEditorApp();

private:
	void Render();
	void Update();

// 重写
public:
	virtual BOOL InitInstance();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};

extern CMapEditorApp theApp;
