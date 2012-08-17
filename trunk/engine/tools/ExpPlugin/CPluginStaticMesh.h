#pragma once
#include "CEditMesh.h"
#include "Model.h"
#include "CPluginMeshBase.h"

namespace sqr
{
	class CPluginStaticMesh
		: public CEditStaticMesh
		, public CPluginMesh
	{
	public:
		CPluginStaticMesh();
		~CPluginStaticMesh();

	private:
		virtual CPluginMeshLevel* 
				createMeshLevel();

		virtual CPluginMeshLevel* 
				GetPluginMeshLevel( size_t level );

		virtual size_t 
				GetPluginMeshLevelNum();

		virtual void
				setBufVertexNum( int level, int vertNum );

		virtual void
				generatePosBuffer( int level, const vector<CVector3f>& posBuf );

		virtual void 
				generateNormalBuffer( int level, const vector<CVector3f>& normBuf );

		virtual void
				generateTexCoordBuffer0( int level, const vector<CVector2f>& texBuf0 );

		virtual void
				generateTexCoordBuffer1( int level, const vector<CVector2f>& texBuf1 );

		virtual void 
				generateTexCoordBuffer2( int level, const vector<CVector2f>& texBuf2 );

		virtual void
				generateSkeWeightBuffer( int level, const vector<CWeight>& skeWeightBuf );

		virtual void
				generateSkeIndexBuffer( int level, const vector<SkltIndex>& skeIndexBuf );
		
		// temp code
	public:
		virtual VERTEXTYPE&
				GetMeshType();

		virtual float&
				GetBaize();

		virtual CVector3f&
				GetScale();

		virtual CVector3f&
				GetOff();

		virtual CVector3f&
				GetOrgScale();

		virtual CVector3f&
				GetOrgOff();

		virtual float& 
				GetTexScale();

		virtual int&
				GetChannel();

		virtual bool&
				GetIsCompress();
	};
}