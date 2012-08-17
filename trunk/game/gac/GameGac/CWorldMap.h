#pragma once
#include "SQRWnd.h"

class CWorldMap
{
private:
	static const int WORLD_MAP_ROW = 4;
	static const int WORLD_MAP_COL = 4;
private:
	ITexture*		m_pTexture[WORLD_MAP_ROW][WORLD_MAP_COL];
	sqr::SQRWnd*      m_pWnd;
public:
	CWorldMap(void);
	virtual ~CWorldMap(void);
public:
	void InitData(sqr::SQRWnd* pWnd);
	void Clearup();
	void Render();
};
