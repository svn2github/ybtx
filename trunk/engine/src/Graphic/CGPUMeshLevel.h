//-------------------------------------------------
// 文件名：	CGPUMeshLevel.h
// 说明：	利用GPU对Mesh顶点计算的类<D3D9实现>
// 创建人：	董恺
// 创建时间：2008年6月27日
// 备注：	用于测试
//-------------------------------------------------
#ifndef _GPU_MeshLevel_H_
#define _GPU_MeshLevel_H_
#include "CMeshLevel.h"

class CGSkeletalLevel : public CMeshLevel
{
public:
	struct VertexFomat
	{
		int16		x,y,z;
		uint16		n;
		uint16		u,v;
		CTinyWeight w;
		SkltIndex	i;
	};

	//DWORD FVF = FVF_XYZ|FVF_NORMAL|FVF_TEX1||D3DFVF_LASTBETA_UBYTE4
};
#endif //_GPUMeshLevel_H_