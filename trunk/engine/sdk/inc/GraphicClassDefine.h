#pragma once
#include "CGraphicMallocObject.h"

namespace sqr
{
	//Interface
	class IModelHandler;
	class CDataSources;
	class IWater;
	class CRenderFilter;

	class IGraphic;
	class CRendererBase;
	class CRenderStyle;
	class CGpuProgramMgr;

	class CResUnitGroup;
	class CResUintNode;
	class ITexture;
	class IResTexture;
	class CRenderFont;
	class CPiece;
	class CPieceGroup;
	
	//class
	class CHardwareBufferMgr;
	class CHardwareBuffer;
	class CIndexHardwareBuffer;
	class CVertexHardwareBuffer;
	class CShadowFilter;
	class CRenderGroup;
	class CPieceRenderStyle;
	class CRenderPipeline;
	class FilterNode;

	class CDynamicVB;

	class CViewport;
	class RenderSettings;
	class CWindowTarget;	
	class CRenderTarget;
	class CRenderTexture;
	
	class CVertexDeclaration;

	class CLightSystem;
	struct LightStruct;
	class CLight;
	
	class CGraphic;
	class CCamera;
	
	class CBaseModel;
	class CModelInst;
	class CModel;
	class CMesh;
	class CSkeletal;
	class CSkeletalFrame;
	class CSkeletalUnit;
	class CMatrixUnit;
	class CMatrixUnit;
	class IDNAME;
	class CAnimate;
	class CAnimationGroup;
	//class CAnimationsRes;
	class CAnimationController;
	class CAnimateSeparation;
	struct CRenderMatrix;
	struct SKELETAL_MASK;
	class CRenderPiece;
	

	class CResourceContainer;
	class CResourceRecycle;
	typedef GVector<CMatrixUnit*>			CMatrixUnitPtrVector;
	typedef GVector<CAnimateSeparation*>	CAnimateSeparationPtrVector;
	typedef GVector<CAnimationController*>	CAnimationControllerPtrVector;

	///描述缓存的用发，属性之间不相互排斥
	enum Usage
	{
		/** 静态缓存，一旦创建应用的时候基本不会被修改。修改这个缓存区内容包括performace hit
		*/
		HBU_STATIC = 1,
		/**应用这个属性说明CPU可以经常修改这个缓存区的内容。
		使用这个属性创建的缓存区AGP内存(3D高速缓存)的效率将不会高于一般内存。
		*/
		HBU_DYNAMIC = 2,
		/**应用这个属性的缓存区中的内容是不会通过函数返回被读取的，他只能不断的写入数据。
		锁定一个这样的缓存区总是会返回一个指向新区域的指针，空白的内存会比与拥有原
		数据区的缓存区更优越；这样避免了对存储器直接存取因为你可以在使用原来的数据的同时
		写入一段新的数据。
		*/
		HBU_WRITE_ONLY = 4,
		/// 结合了HBU_STATIC 和 HBU_WRITE_ONLY 的特点
		HBU_STATIC_WRITE_ONLY = 5,
		/// 结合了HBU_DYNAMIC 和 HBU_WRITE_ONLY 的特点
		HBU_DYNAMIC_WRITE_ONLY = 6
	};

	//coder
	class CFileFormatMgr;
	
	template<typename Type>
	inline void GfkSafeDelete(Type& p)
	{
		Type t = p;
		p = NULL;
		delete t;
	}
}