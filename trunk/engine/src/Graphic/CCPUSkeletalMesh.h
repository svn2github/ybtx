#pragma once
#include "CMesh.h"

namespace sqr
{
	class CCPUSkeletalMesh : public CMesh
	{
		friend class CDataSources;
	protected:
		struct VertexFomatFull
		{
			float		x,y,z;
			float		n,ny,nz;
			float		u,v;
			float		u1,v1;
			float		u2,v2;
		};
		CCPUSkeletalMesh();
	public:		
		~CCPUSkeletalMesh();

		VERTEXTYPE		GetType(void);
		int				BinLoad( CBufFile& QuickFile ,DWORD Version, SKELETAL_MASK& mask  );
		FilterNode* 	Render( CRenderMatrix& ModelMatrix,FilterNode& FN );
		UINT			GetRenderCommandNum(void) { return 1; };
		void			CreateRenderCommand(CRenderCommand& Com, UINT Index = 0);
	public:
		struct	PointInfoEx
		{
			CWeight		w;
			SkltIndex	i;
		};
		typedef GVector<PointInfoEx> PointInfoBuffer;
		struct  SkinInfo
		{
			CVertexHardwareBuffer*	m_pVertexBuffer;
			PointInfoBuffer*		m_pSkinBuffer;
		};

		
		PointInfoBuffer	m_SkinBuffer;
		SkinInfo		m_SkinInfo;
		
	};

	inline VERTEXTYPE CCPUSkeletalMesh::GetType(void)
	{
		return VT_SKELETALANI;
	}
}