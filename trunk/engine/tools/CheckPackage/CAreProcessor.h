#pragma once
#include "iresprocessormgr.h"
#include "EffectBase.h"
#include "IEffectGraphic.h"
#include "IEffectClassDesc.h"
#include "GraphicCommon.h"
#include <map>
#include <vector>
#include <set>

class CAreManager;

struct EFFECTKEY
{
	EFFECTKEY() : nParentTrack(-1)
				, nParentStartTime(-1)
				, pEffectUnit(NULL)
	{}

	int					nParentTrack;
	int					nParentStartTime;
	IEffectUnitProp*	pEffectUnit;
};

class CEffectReadFile : public IEffectRead
{
	ipkgstream	m_File;

public:
	string m_strFileName;

	CEffectReadFile( const char* szFileName, bool bBinary )
		: m_File( L"", szFileName, bBinary ),m_strFileName(szFileName) {}
	
	bool fail(){ return m_File.fail(); }

	void read( void* pBuf, size_t uSize )
	{
		m_File.read( (char*)pBuf, (std::streamsize)uSize );
	}
};

class CPakEffect
{
	vector< map< int, EFFECTKEY > >	m_Tracks;
	int								m_TimeRange;
	set< string >					m_AttachAniName;
	string							m_UserDesc;
	bool							m_bFrameSynch;
	bool							m_bForbidScale;

	DWORD							m_CurTime;
	float							m_CurFrame;

	CAreManager*					m_pEffectMgr;

public:

	CPakEffect( CAreManager* pEffectMgr ) : m_pEffectMgr( pEffectMgr ){};
	virtual ~CPakEffect(void);

	virtual int						LoadBin( CEffectReadFile& File, DWORD dwVer, string szFxName );
};

class CAreProcessor;
class CAreManager
{
	CAreProcessor*							m_pProcessor;
	map< string, map< string, CPakEffect* > >	m_pEffects;
public:
	CAreManager( CAreProcessor* pProcessor );
	virtual ~CAreManager(void);

	CAreProcessor*		GetProcessor(){ return m_pProcessor; }
	IEffectUnitProp*	CreateEffectUnitProp( const GUID Guid, int Num );
	int					LoadEffectGroups( string LoadName );
};

class CAreProcessor
	: public CBaseResProcessor, public IEffectGraphic
{
	IEffectClassDesc*	m_pEffectDesc;
public:
	CAreProcessor(void);
	~CAreProcessor(void);

	virtual bool			RunProcess( const char* szFilename );
	virtual size_t			GetCountCanBeProcess(){ return 1; };
	virtual unsigned long	GetTypeCanBeProcess( size_t nNum ){ return 'efx'; }
	IEffectClassDesc*		GetClassDesc(){ return m_pEffectDesc; }

	virtual void			Release(){};
	virtual void			InsertRenderer( CRendererBase* pRenderSys ){};
	virtual void			DeleteRenderer( CRendererBase* pRenderSys ){};
	virtual void			SetFog( uint32 Color, float Start, float End ){};
	virtual void			SetFogColor( const uint32& Color ){};
	virtual void			SetLinerFogPara( const float& fogstart, const float& fogend ){};
	virtual void			SetFogMode( const int& fogmode ){};
	virtual void			SetFogDensity( const float& density ){};
	virtual void			EnableFog( bool bEnable ){};
	virtual void			SetEffectLevel(float level){};
	virtual float			GetEffectLevel(void){return 1.0f;};
	virtual void			SetEffectCallBack( SetFun sfun,GetFun gFun ){};

	virtual void			SetCameraShake(bool b){};
	virtual bool			GetCameraShake(void){return true;};
	virtual void			SetCameraShakeCallBack( SetboolFun sfun , GetboolFun gfun ){};

	virtual HRESULT			CreateTexture( int32 nWidth, int32 nHeight, eTextureFormat eFormat, ITexture** pText ){ *pText = 0; return 0; };
	virtual HRESULT			CreateEmptyTexture( int32 nWidth, int32 nHeight, uint32 nLevel, eTextureFormat eFormat, ITexture** pText ){ *pText = 0; return 0; };
	virtual CRenderTexture* CreateRenderTarget(void) { return NULL; };
	virtual HRESULT			CreateTexture( const wchar_t* alias, LPCTSTR szName, ITexture** pText ){ *pText = 0;return 0; };
	virtual HRESULT			CreateFont( int32 nFontID, bool bBold, float fSize, CRenderFont** pFont ){ *pFont = 0;return 0; };
	virtual HRESULT			CreatePieceGroup( LPCTSTR szName, CPieceGroup** pPieceGroup ){ *pPieceGroup = 0; return 0; };
	//virtual HRESULT			CreateAnimationsRes( LPCTSTR szName, CAnimationsRes** pAnimationGroup ){ *pAnimationGroup = 0; return 0; };
	virtual HRESULT			GetSkeInPool( const TCHAR* szName, CSkeletalFrame** pSkeletalFrame ){*pSkeletalFrame = 0;return 0;}
	virtual HRESULT			CreateAnimationGroup( const TCHAR* szName, CAnimationGroup** pAnimationGroup ){*pAnimationGroup = 0;return 0;}
	virtual void			DrawRect( const CFRect& rt, uint32 Color, ITexture* pTexture, const CFRect* rtText = NULL, ERenderStyle rs = RS_GUI, float z = MAX_DEPTH ){};
	virtual void			DrawRect( const CFRect& rt, uint32 Color, ITexture* pTexture, const CPosRect& rtText, ERenderStyle rs = RS_GUI, float z = MAX_DEPTH ){};
	virtual void			DrawSkyBoard( const CFRect& rt, uint32 Color, ITexture* pTexture, const float& uoffset, const float& voffset, ERenderStyle rs = RS_GUI, float z = MAX_DEPTH ){};
	virtual void			Draw3DPlane(sqr::ITexture* pTexture, sqr::CVector3f *vec3d, DWORD Color, sqr::ERenderStyle rs, sqr::CVector2f *tex_uv){};
	virtual void			DrawRectGroup( ITexture* pTexture, ERenderStyle rs, const RectGroup* rt, int nNum ){};
	virtual void			DrawLine( const PointGroup* pPointGroup, int nNum, bool b2D = true, ERenderStyle rs = RS_GUI ){};
	
	virtual bool			PickUp( int xScreen, int yScreen, const CVector3f& vScale, const CVector3f& vOffset, const CMatrix& matWorld ){ return 0; };
	virtual float			SceneToScreen( int& xScreen, int& yScreen, const CVector3f& vScene ){ return 0; };

	virtual void			SetZDelta( float Z ){};
	virtual bool			IsFogEnabled() { return false; };
	virtual void			SetTextureMatrix( uint32 uTextStage, const CMatrix* pMatrix ){};
	virtual void			GetLightSystem( CLightSystem** ppLightSystem ){};
	virtual CMatrix*		GetRenderStack( ERenderStyle, ITexture*,	ITexture*, EPrimitiveType, uint32, uint32, uint32, uint32, void**, void**,	void**,DWORD ){ return NULL; }
	virtual CMatrix*		GetRenderStack( ERenderStyle RenderStyle,	ERenderObjStyle RenderObjStyle,	ITexture* Text0, ITexture* Text1, EPrimitiveType PrimitiveType, 
		uint32 VertexNum, uint32 PrimitiveCount, uint32 VerFormat, uint32 VerStride, void** ppVB, void** ppIB, void** ppRS = NULL, DWORD ShaderID = 0) { return NULL; }
	virtual CMatrix*		GetRenderThreeLayerStack( ERenderStyle RenderStyle,
		ITexture* Text0,
		ITexture* Text1,
		ITexture* Text2,
		EPrimitiveType PrimitiveType,
		uint32 VertexNum,
		uint32 PrimitiveCount,
		uint32 VerFormat,
		uint32 VerStride,
		void** ppVB,
		void** ppIB,
		void** ppRS = NULL,
		DWORD ShaderID = 0 ){return NULL;};
	virtual void			DoScreenShot(const char * szFilename) {};
	virtual void			ShowFPS(bool isShow) {};	
	virtual void			SetAntiAliasQuality(sqr::EAntiAliasQuality quality){};
	virtual void			SetAsynSwitch(bool bEnable) {};
	virtual void			EnableAsynRead(bool bEnable){};
	virtual void			SwapBegin(void) {}; //这两个东西很玄妙 不要经常用
	virtual void			SwapEnd(void)	{}; //这两个东西很玄妙 不要经常用
};
