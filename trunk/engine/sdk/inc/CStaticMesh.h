#ifndef _CStatic_Mesh_H_
#define _CStatic_Mesh_H_

#include "CMesh.h"
#include "SyncSystem.h"

namespace sqr
{
class CStaticMesh : public CMesh
{
	friend class CStaticMeshCoder;
	friend class CDataSources;
protected:
	struct VertexFomat
	{
		float		x,y,z;
		uint16		n;
		uint16		u,v;
		uint16		u1,v1;
		uint16		u2,v2;
	};
	struct VertexFomatFull
	{
		float		x,y,z;
		float		n,ny,nz;
		float		u,v;
		float		u1,v1;
		float		u2,v2;
	};
	CStaticMesh();
public:
	~CStaticMesh();
	VERTEXTYPE		GetType(void)
	{
		return VT_STATICVERTEX;
	}
	int				BinLoad( CBufFile& QuickFile ,DWORD Version,SKELETAL_MASK& mask  );
	FilterNode* 	Render( CRenderMatrix& ModelMatrix,FilterNode& FN );

	UINT		GetRenderCommandNum(void) { return 1; };
	void		CreateRenderCommand(CRenderCommand& Com, UINT Index = 0);
};
}

#endif