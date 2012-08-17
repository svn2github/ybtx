#pragma once
#include "TSingleton.h"
#include "GraphicClassDefine.h"

namespace sqr
{
	class CTexture2DImp;
	class ITexture;

	class CDataSources : public Singleton<CDataSources>
	{
	public:
		CDataSources();
		virtual ~CDataSources();

	public: //MOD
		virtual CPieceGroup*			NewPieceGroup( const string& szName , const wstring& Alias);
		virtual CPiece*					NewPiece(CPieceGroup* pGrp);
		virtual CMesh*					NewVertexAniMesh(void);
		virtual CMesh*					NewStaticMesh(void);
		virtual CMesh*					NewSkeletalMesh(void);
	
	public: //Buffer
		virtual CHardwareBuffer*		NewBuffer(size_t bufferSize);
		virtual CVertexHardwareBuffer*	NewVertexBuffer(size_t vertexSize, size_t numVerts, Usage usage,void* pInitData, bool useSysMem, bool useShadowBuffer);
		virtual CIndexHardwareBuffer*	NewIndexBuffer(size_t numIndexes, Usage usage,void* pInitData, bool useSysMem, bool useShadowBuffer);
	
	public: //ANI
		virtual CAnimationGroup*		NewAnimationGroup(const string& szName, const wstring& Alias );
		virtual CAnimate*				NewAnimate(CAnimationGroup* pAniGrp, const string& szFileName , IDNAME& AniName);
		virtual CSkeletalFrame*			NewSkeletalFrame(const string& szName, const wstring& Alias );
		virtual CSkeletal*				NewSkeletal(void);
		virtual CSkeletalUnit*			NewSkeletalUnit(void);

	public: //Texture
		virtual	IResTexture*			NewTexture( const string& szName, const wstring& Alias);
		virtual CRenderTexture*			NewRenderTexture(void);
		virtual CRenderTexture*			NewDepthTexture(void);
		virtual CWindowTarget*			NewWindow(const RenderSettings& settings);
		virtual CTexture2DImp*			NewTex2DImp(const ITexture* pParent);
		
	public: //Other
		virtual CRenderStyle*			NewRenderStyle(const string& szName, const wstring& Alias);
		virtual CRenderFont*			NewRenderFont(const string& szName, const wstring& Alias);
		virtual CPieceRenderStyle*		NewPieceRenderStyle();
		virtual CVertexDeclaration*		NewVertexDeclaration(DWORD FVF);
	};
}