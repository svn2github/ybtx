#pragma once
#include "CPos.h"


enum ELinkRegionState
{
	eLRS_UN_USED,
	eLRS_OPEN,	
	eLRS_CLOSE
};


class CLinkRegionBuildNode;
class CLinkInfoBuild;
class CLinkRegionBuild;


//用于计算LinkRegion使用的节点 (含很多为了提高效率而存在的属性)
class CLinkRegionBuildNode
{
	typedef	 uint8		Num_t;
	typedef vector<CLinkRegionBuildNode*>	VecRegionList_t;
	typedef vector<CLinkInfoBuild*>	VecLinkList_t;

	friend CLinkRegionBuild;
public:
	CLinkRegionBuildNode();
	~CLinkRegionBuildNode();

	bool IsInOtherLinkRegion(CLinkRegionBuildNode* pOhterNode);
	uint32 GetLayerLv();
	inline Num_t GetChildNum(){return m_nChildRegionNum;};
	inline CLinkRegionBuildNode* GetChild(uint32 index){return m_ppChildRegion[index];};
	
	void SetChildLinkRegion(VecRegionList_t* pVecLinkRegionList);
	void SetLinkInfo(VecLinkList_t* pVecLinkInfoList);
	void SetFatherLinkRegion(CLinkRegionBuildNode* pFatherLinkRegion);


	inline const CPos& GetKeyPos(){return m_KeyPos;};
	inline bool IsOpen(uint32 nSearchId){return m_nSearchId == nSearchId && m_eState == eLRS_OPEN;};
	inline bool IsClose(uint32 nSearchId){return m_nSearchId == nSearchId && m_eState == eLRS_CLOSE;};
	inline void Open(uint32 nSearchId){	m_eState = eLRS_OPEN;m_nSearchId = nSearchId;};
	inline void Close(){m_eState = eLRS_CLOSE;};
protected:
//记录寻路时数据的属性
	CLinkRegionBuildNode*			m_pFrontLinkRegion;		//路径的前个节点
	float							m_fG;				//寻路起点到该连通区域的代价
	ELinkRegionState				m_eState;
	uint32							m_nSearchId;

	
//固定属性==============================================================================================
	CPos				m_KeyPos;

	
	CLinkRegionBuildNode*			m_pFatherRegion;		//父连通区域(所属高一级的连通区域, this 将在 m_pFatherRegion->m_ppChildRegion 中)
	Num_t							m_nChildRegionNum;	//子连通区域的个数
	CLinkRegionBuildNode**			m_ppChildRegion;		//子连通区域列表
	Num_t							m_nLinkNum;		//同级连通区域中与此连通区域相连的连通区域个数
	CLinkInfoBuild**				m_ppLinkInfo;		//相连的连通区域信息列表
	uint32							m_nGlobalIndex;//保存时候用到
};

class CLinkInfoBuild
{
	friend CLinkRegionBuild;
public:
	CLinkInfoBuild(CLinkRegionBuildNode* pDestLinkRegion, float fFare);
private:
	CLinkRegionBuildNode*			m_pDestLinkRegion;		//目标连通区域,
	float				m_fFare;			//到目标连通区域的费用
};