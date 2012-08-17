#pragma once
#include "ITexture.h"

namespace bly
{
class SVertex;
class SGrid;

class CTextureNode
{
public:

	std::string m_strTextureNames[4];
	std::vector<int> m_vGridIndices;
	int m_dwGridCnt;
	DWORD m_dwTextureLayer;

	//纹理对象
	ITexture *m_pLayerOneTexture;
	ITexture *m_pLayerSecondTexture;
	ITexture *m_pLayerThreeTexture;
	ITexture *m_pLayerFourTexture;
	RenderState *m_pRS;

	void AddGrid(int dw);

public:

	CTextureNode();
	~CTextureNode();

	int GetGridIndex(int dw);

	int GetGridCnt();

	void SetTextureLayer(DWORD dw);
	DWORD GetTextureLayer();

	std::string GetTextureName(int layer);

	void SetTextureName(std::string name, int layer);

	void Render(SGrid * pGrids, SVertex * pVertices, bool bWater, float fTimeRatio);
};

class CTextureNodeManager
{
public:
	CTextureNodeManager(void) : layerone(0), layertwo(0), layerthree(0), layerfour(0) {};
	~CTextureNodeManager(void) {};

	void Insert(const bly::STextureNodeInfo & nodeInfo);
	void Render(SGrid * pGrids, SVertex * pVertices, bool bWater, float fTimeRatio);
	void Clear();
private:

	std::vector<CTextureNode*> m_TextureNodeOneList; //第一层texurenode集合
	std::vector<CTextureNode*> m_TextureNodeTwoList; //第二层texurenode集合
	std::vector<CTextureNode*> m_TextureNodeThreeList; //第三层texurenode集合
	std::vector<CTextureNode*> m_TextureNodeFourList; //第四层texurenode集合

public:
	std::map<std::string, int> m_LayerOneIndex;
	std::map<std::string, int> m_LayerTwoIndex;
	std::map<std::string, int> m_LayerThreeIndex;
	std::map<std::string, int> m_LayerFourIndex;

	int layerone;
	int layertwo;
	int layerthree;
	int layerfour;
};
}
