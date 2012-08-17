#ifndef _CVertexAni_Mesh_H_
#define _CVertexAni_Mesh_H_

#include "CMesh.h"



namespace sqr
{
#define  veraniID 131661824
class CRenderPiece;

class CVertexAniMesh : public CMesh
{
	friend class CDataSources;
protected:
	struct VertexFomat
	{
		int16 x,y,z;
		uint16 n;
	};
	struct VertexFomatEx : public VertexFomat
	{
		uint16		u, v ;
		uint16		u1,v1;
		uint16		u2,v2;
	};
	struct TextCoord
	{
		uint16 u,v;
	};

	struct LevelAni
	{
		IDNAME							m_AnimationeName;	//动画名
		uint32							m_FramePerKeyFrame;	//顶点动画采样率
		GVector< GVector<VertexFomat> >	m_FrameVertex;;
		GVector<CVertexHardwareBuffer*>	m_FrameBuffer;
	};
	CVertexAniMesh();
public:

	typedef GVector<LevelAni> AniVector;
	struct VertexAniParams
	{
		float			Weight;
		float			InvWeight;
		AniVector*		pAniVector;
		CRenderPiece*	pRenderPiece;
		uint8			iPiece;
	};
public:
	
	int BinLoad( CBufFile& QuickFile ,DWORD Version, SKELETAL_MASK& mask  )
	{
		return 0;
	};
};
}
#endif