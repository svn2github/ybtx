#pragma once

#include "../TileEditorEngine/CTileEditorModel.h"
#include "../TileEditorEngine/Face.h"
using namespace bg;

class CResSetUnit
{	
	std::vector<std::string> m_unitNames;
	bool m_bIsEdited;
	bool m_bSubUnit;
	std::string m_strItemName;
		
public:
	void * h;
	
	CResSetUnit() {	m_bIsEdited = false; }
	
	void AddName(std::string s)
	{
		m_unitNames.push_back(s);
	}
	
	std::vector<std::string> & GetName() 
	{
		return m_unitNames;
	}
	
	int GetNameSize() 
	{
		return m_unitNames.size();
	}
	
	std::string & GetNameByIndex(int n) 
	{
		return m_unitNames[n];
	}
	
	void SetIsSubUnit(bool b)
	{
		m_bSubUnit = b;
	}
	
	bool GetIsSubUnit() 
	{
		return m_bSubUnit;
	}

	bool GetIsEdited()
	{
		return m_bIsEdited;
	}

	void SetIsEdited(bool b)
	{
		m_bIsEdited = b;
	}
	
	std::string & GetItemName()
	{
		return m_strItemName;
	}
	
	void SetItemName(std::string & str)
	{
		m_strItemName = str;
	}
};

struct CResSetGroup
{
	std::string				m_groupName;
	std::vector<CResSetUnit*>	m_resSetUnits;
};

struct CDimSet
{
	std::vector<CResSetGroup*> m_resSetGroups;
};

class CResSet
{
public:
	std::string		m_resSetName;
	CDimSet		m_2DResSet;
	CDimSet		m_3DResSet;
	CDimSet		m_FXResSet;
	void * m_pEffectGroup;
	
	CResSet();
	~CResSet();
};

class C2DResSetUnit : public CResSetUnit
{
	CTerrainMesh::CTextureNode::ETRANS_TYPE m_eTransType;
	
public:
	C2DResSetUnit();
	~C2DResSetUnit();
	
	CTerrainMesh::CTextureNode::ETRANS_TYPE GetTransType();
	void SetTransType(CTerrainMesh::CTextureNode::ETRANS_TYPE e);
};

class C3DResSetUnit : public CResSetUnit
{
	int m_eCenterType;
	float m_fLift;
	float m_fShiftZ;
	float m_fShiftX;
	CFace::ETrans_Type m_eTransType;
	std::string m_strAraFileName;
	bool m_bShadow;
			
public:
	std::string m_strEffectName;
	void * m_pEffectGroup;

private:
	std::vector< std::vector<CTileEditorModel::SBlockInfo> > m_vvBlockInfo;
	std::vector< std::vector<CTileEditorModel::SLogicHeightInfo> > m_vvLogicHeightInfo;

public:
	C3DResSetUnit();
	~C3DResSetUnit();
	
	void SetCenterType(int eCenterType);
	int GetCenterType();
	float GetLift();
	float GetShiftX();
	float GetShiftZ();
	void SetLift(float f);
	void SetShiftX(float f);
	void SetShiftZ(float f);
	void SetModelBlockSize(int nModelIndex, int nSize);
	void SetTotalBlockSize(int nSize);
	int GetTotalBlockSize() {return m_vvBlockInfo.size();}
	int GetModelBlockSize(int n);
	void FlipModelBlockMask(int nModelIndex, int nPosition);
	void SetModelBlockMask(int nModelIndex, int nPosition, int nMask);
	std::vector< std::vector<CTileEditorModel::SBlockInfo> > & GetTotalBlock(); 
	std::vector<CTileEditorModel::SBlockInfo> & GetModelBlock(int n); 
	void SetModelBlockType(int nModelIndex, int nPosition, int nBlockType);
	void SetModelLogicHeight(int nModelIndex, int nPosition, float fLogicHeight);
	void SetTransType(CFace::ETrans_Type e);
	CFace::ETrans_Type GetTransType();
	void SetAnimationByAraFileName(std::string strAraFileName);
	std::string & GetAraFileName();
	void SetTotalHeightSize(int nSize);
	int GetTotalHeightSize() {return m_vvLogicHeightInfo.size();}
	void SetModelHeightSize(int nModelIndex, int nSize);
	int GetModelHeightSize(int n);
	std::vector<CTileEditorModel::SLogicHeightInfo> & GetModelHeight(int n);
	std::vector< std::vector<CTileEditorModel::SLogicHeightInfo> > & GetTotalHeight();
	void SetShadow(bool);
	bool GetShadow();
};