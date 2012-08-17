#pragma once
#include "CEventOwner.h"
#include "TSingleton.h"

namespace sqr
{
class ITexture;
struct RenderState;

class CToolSetTileBlock: public Singleton<CToolSetTileBlock>
{
public:
	enum EBlockType			{ EBT_NONE = 0,  EBT_LOW = 1, EBT_MID = 2, EBT_HIGH = 3, EBT_CNT = 4 };
	enum ERenderBlockType	{ EBRT_LOW = 0,  EBRTT_MID = 1, EBRT_HIGH = 2, EBRT_ALL = 3 };
	enum EEditType			{ EET_WRITE = 0, EET_ERASE };
	enum EEditBrushStyle	{ EBT_NORMAL = 0,EBT_EXCAT = 1};///刷地表的类型 普通1*1 精确 2*2

	struct SProcessParams
	{
		SProcessParams()
		{
			eBlockTypeHot = EBT_HIGH;
			eEditType = EET_WRITE;
			eEditBrushType = EBT_NORMAL;
			dwBrushSizeInX = 1;
			dwBrushSizeInZ = 1;
		}

		EBlockType eBlockTypeHot;
		EEditType eEditType;
		EEditBrushStyle eEditBrushType;
		DWORD dwBrushSizeInX;
		DWORD dwBrushSizeInZ;
	};

private:
	bool					 m_bShowBlock;
	SProcessParams			 m_processParams;
	RenderState				 *m_pRS;
	map<int, int>			 m_DivideGrids; ///first grid index second which small grid

	void RenderNormalBlockBrush();
	void RenderExactBlockBrush();
	void SetBrushRS();

public:

	CToolSetTileBlock();
	~CToolSetTileBlock();
	void OnEvent(const CMsg& msg);
	void Render();
	void SetEditType(int nEditType);
	void SetEditBrushType(int nEditBrushType);
	void SetBlockBrushType();
	void SetBlockType(int nBlockType);

	DWORD GetBrushSizeInX();
	void SetBrushSizeInX(DWORD dw);
	void IncBrushSizeInX();
	void DecBrushSizeInX();

	DWORD GetBrushSizeInZ();
	void SetBrushSizeInZ(DWORD dw);
	void IncBrushSizeInZ();
	void DecBrushSizeInZ();
	map<int, vector<int>> & GetBrushSubGridIndices();
	vector<int> &			GetBrushGridIndices();

	bool Save(const string& path);
	bool ReSizeSave(const string& path, int nWidth, int nDepth);
	void Load(const string& path);

	void DeleteBlock();
	///delete scene all blocks by parameter
	void DeleteSceneBlocks(const int nType);
	int	 GetExactGridIndex(const int nGridIndex);
	void SetExactGridIndex();
	void SetTerrainBlock();

	bool GetDoesShowBlock() const;
	void SetDoesShowBlock(bool b);

	///删掉所有阻挡信息
	void DeleteTerrainAllBlocks();
};

inline
DWORD CToolSetTileBlock::GetBrushSizeInX()
{
	return m_processParams.dwBrushSizeInX;
}

inline
void CToolSetTileBlock::SetBrushSizeInX(DWORD dw)
{
	m_processParams.dwBrushSizeInX = dw;
}

inline
void CToolSetTileBlock::IncBrushSizeInX()
{
	m_processParams.dwBrushSizeInX++;
}

inline
void CToolSetTileBlock::DecBrushSizeInX()
{
	if ( m_processParams.dwBrushSizeInX == 1 )
		return;

	m_processParams.dwBrushSizeInX--;
}

inline
DWORD CToolSetTileBlock::GetBrushSizeInZ()
{
	return m_processParams.dwBrushSizeInZ;
}

inline
void CToolSetTileBlock::SetBrushSizeInZ(DWORD dw)
{
	m_processParams.dwBrushSizeInZ = dw;
}

inline
void CToolSetTileBlock::IncBrushSizeInZ()
{
	m_processParams.dwBrushSizeInZ++;
}

inline
void CToolSetTileBlock::DecBrushSizeInZ()
{
	if ( m_processParams.dwBrushSizeInZ == 1 )
		return;

	m_processParams.dwBrushSizeInZ--;
}

inline
void CToolSetTileBlock::SetEditType(int nEditType)
{
	EEditType eEditType = EEditType(nEditType);
	m_processParams.eEditType = eEditType;
}

inline
void CToolSetTileBlock::SetEditBrushType(int nEditBrushType)
{
	EEditBrushStyle eEditBrushType = EEditBrushStyle(nEditBrushType);
	m_processParams.eEditBrushType = eEditBrushType;
}

inline
void CToolSetTileBlock::SetBlockType(int nBlockType)
{
	EBlockType eBlockType = EBlockType(nBlockType);
	m_processParams.eBlockTypeHot = eBlockType;
}

inline
bool CToolSetTileBlock::GetDoesShowBlock() const
{
	return m_bShowBlock;
}

inline
void CToolSetTileBlock::SetDoesShowBlock(bool b)
{
	m_bShowBlock = b;
}

}