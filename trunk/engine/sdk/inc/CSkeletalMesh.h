#pragma once
#include "CMesh.h"
#define _USEVEC_
namespace sqr
{
#ifdef _USEVEC_
	typedef GVector<uint8>	  BoneMap;
#else
	typedef GMap<INT32,INT32> BoneMap;
#endif

	const int32 GpuMaxBoneNum = 76;

	class CSkeletalMesh : public CMesh
	{
		friend class CDataSources;
	public:
		~CSkeletalMesh();
	protected:
		CSkeletalMesh();
		int32				m_boneNum;
		size_t				m_boneSize;
		size_t				m_GpuBoneSize;
		uint				m_ParamIndex;

		struct SkeSubMesh
		{
			int32		Start;
			int32		Count;
			BoneMap		BonePool;
		};
		GVector< SkeSubMesh > mRenderPiece;

	public:
		struct SkeletalParams
		{

			CMatrixUnit Bone[GpuMaxBoneNum];
			size_t	 BoneSize;
			BoneMap* pBoneMap;
		};

		VERTEXTYPE		GetType(void)
		{
			return VT_SKELETALANI;
		}
		int				BinLoad( CBufFile& QuickFile ,DWORD Version, SKELETAL_MASK& mask  );
		FilterNode* 	Render( CRenderMatrix& ModelMatrix,FilterNode& FN );
		UINT			GetRenderCommandNum(void);
		void			CreateRenderCommand(CRenderCommand& Com, UINT Index = 0);
		
	protected:		
		void	SetSkeletalMask(SKELETAL_MASK& Mask,
			GVector<VertexSkeletalFormat>& VertexBuf,
			GVector<uint16>&	IndexBuf );

		BOOL  IsAddface( INT32& index, GMap<int32, int32>& loadbonelist,
			GMap<int32, int32>& bonelist,
			GVector<VertexSkeletalFormat>& SrcVertexBuf,
			GVector<VertexSkeletalFormat>& VertexBuf,
			GVector<uint16>&	IndexBuf,
			GVector<int32>& mindexinfo,
			GMap<int32, int32>& mvector,
			GMap<int32, int32>& mallvector,
			GMap<int32, int32>& mSrcId );

		static BOOL  IsLoadBone( GMap<int32, int32>& loadbonelist, INT32& boneid );
		static int32 FindBone( GMap<int32, int32>& loadbonelist, INT32& boneid );
		static int32 FindBoneIndex( GMap<int32, int32>& loadbonelist, INT32& boneid );

		virtual void _RecordVertex(GVector<VertexSkeletalFormat>& VertexOrgBuf)	{};
	};
}