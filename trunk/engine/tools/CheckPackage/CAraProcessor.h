#pragma once

#include <string>
#include <vector>
#include <map>
#include <d3d9.h>
#include <math.h>
#include <D3dx9math.h>
#include <fstream>
#include "iresprocessormgr.h"
#include "GraphicBase.h"
#include "GraphicCommon.h"

//骨骼的单个SOCKET
struct CResSkeletalSocket
{
	IDNAME						m_SocketName;			//SOCKET名字
	D3DXMATRIX					m_Socket;				//SOCKET矩阵
};

//每根骨骼的SOCKET集合
struct CResSkeletalUnit
{
	D3DXMATRIX					m_InverseMatrix;		//每个骨骼上面的皮肤从其全局空间到骨骼局部的矩阵
	vector<CResSkeletalSocket>		m_SocketList;			//骨骼上SOCKET的列表

	int BinLoad( ifstream& File );
};

//骨骼动画
class CResSkeletal
{
	BYTE						m_SkeletalID;           //骨头ID
	string						m_SkeletalName;			//骨头名字

	vector<CResSkeletal*>			m_Children;				//子关节

public:
	int		BinLoad( ifstream& File );
};

//动画组
class CSkeFrame
{
	BYTE						m_SkeletalTotalNum;		//骨骼数量

	float						m_FPS;
	D3DXMATRIX					m_Globe;

	CResSkeletal					m_SkeletalRoot;			//根骨骼
	vector<CResSkeletalUnit*>		m_SkeletalList;			//所有骨骼矩阵（SOCKET）单元列表
	//vector<IDNAME>				m_AnimationList;		//所有动画列表

public:
	int BinLoad( string FileName, CBaseResProcessor* pProcessor );
};

class CAraProcessor : public CBaseResProcessor
{
public:
	CAraProcessor(void);
	virtual ~CAraProcessor(void);

	virtual bool			RunProcess( const char* szFilename );
	virtual size_t			GetCountCanBeProcess(){ return 1; };
	virtual unsigned long	GetTypeCanBeProcess( size_t nNum ){ return 'ske'; }
};
