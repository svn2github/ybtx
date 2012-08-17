#pragma once
#include <string.h>
#include <set>
#include "PkgStream.h"
#include "IEffectUnit.h"
#include "IEffectUnitProp.h"
#include "CEffectClassDesc.h"
#include "CColor.h"
#include "CEffectClass.h"
#include "CQuickRand.h"
#include "Algorithm.h"

class CEffectProp;
namespace sqr
{
	enum SPR_BLENDMODE
	{
		SPRB_NONE			= 0,
		SPRB_MULTIPLY		= 1,
		SPRB_ADD			= 2,
		SPRB_LIGHT			= 3
	};

	enum SPR_BLENDOP
	{
		SPRO_SELECTARG1           = 0,      // the default
		SPRO_SELECTARG2           = 1,

		// Modulate
		SPRO_MODULATE             = 2,      // multiply args together
		SPRO_MODULATE2X           = 3,      // multiply and  1 bit
		SPRO_MODULATE4X           = 4,      // multiply and  2 bits

		// Add
		SPRO_ADD                  =  5,   // add arguments together
		SPRO_ADDSIGNED            =  6,   // add with -0.5 bias
		SPRO_ADDSIGNED2X          =  7,   // as above but left  1 bit
		SPRO_SUBTRACT             =  8,   // Arg1 - Arg2, with no saturation
		SPRO_ADDSMOOTH            =  9,   // add 2 args, subtract product

		// Linear alpha blend: Arg1*(Alpha) + Arg2*(1-Alpha)
		SPRO_BLENDDIFFUSEALPHA    = 10, // iterated alpha
		SPRO_BLENDTEXTUREALPHA    = 11, // texture alpha
	};

	enum SPR_FACE
	{
		SPRF_NORMAL			= 0,
		SPRF_CAMERA			= 1,
		SPRF_CAMERANONEX	= 2,
		SPRF_WORLDNORPAR	= 3,
		SPRF_TERRAIN		= 4,	
		SPRF_NORMAL_STATIC	= 5,
		SPRF_TERRAIN_ROTATE	= 6,
	};

	enum Z_TEST
	{
		Z_NONE				= 0,
		Z_WRITE				= 1,
		Z_TESTONLY			= 2,
	};

	struct EFFECT_RESULT : public AniResult
	{
		EFFECT_RESULT() : m_UnitbEmpty(true){}
		EFFECT_RESULT( const IDNAME* pFrameString,  bool bIsAniEnd, bool bInAniBlend, bool bEmpty = true )
			: AniResult( pFrameString,  bIsAniEnd, bInAniBlend ), m_UnitbEmpty( bEmpty ){}

		bool m_UnitbEmpty;
	};

	class EFFECT_API CEffectUnitProp  : public IEffectUnitProp
	{
	public:
		int32						m_TimeRange;
		eUpdatePosType				m_eUpdatePos;
		EString						m_szAttachName;
		ESet<EString>				m_szAniName;
		int32						m_nTextureCircle;

		uint32						m_ID;
		EString						m_CustomName;
		EString						m_EffectClassName;
		float						m_EffectLod;
		bool						m_isUseVel;
		EMap< float, CVector3f >	m_vecOffset;
		EMap< float, EString >		m_TextName;
		EMap< float, CColor >		m_Color;
		EMap< float, BYTE >			m_Scale;
		EMap< float, CVector3f >	m_XYZcale;
		EMap< float, CVector3f >	m_UVRCenter;///朝向地表公告板旋转中心
		///粒子发射器最大小半径
		EMap< float, float >		m_EmissionMinRadius;
		EMap< float, float >		m_EmissionMaxRadius;

		bool						m_bTextValid;
		EMap< float, ITexture* >	m_Texture;

		CEffectClassDesc*			m_pEffectClassDesc;
		CEffectProp*				m_pEffectProp;

		bool						m_bSlownDisappear;		//是否特效立即删除
		CAxisAlignedBox				m_MaxAABB;

		void GetCurAdvanceProp( float CurPos, CVector3f& Offset, ITexture*& pTexture, uint32& Color, float& Scale ) const;

		void GetCurAdvanceProp( float CurPos, CVector3f& Offset, ITexture*& pTexture, uint32& Color, float& Scale, CVector3f& XYZcale) const;

		void GetCurAdvanceProp( float CurPos, CVector3f& Offset, ITexture*& pTexture, uint32& Color, float& Scale, CVector3f& XYZcale, CVector3f& UVRCenter) const;

		void GetCurAdvanceProp( float CurPos, float& EmissionMinRadius, float& EmissionMaxRadius) const;
		virtual void	_UpdateParams(void){};
		friend class CEffectUnit;
	public:
		CEffectUnitProp( uint32 ID, IEffectClassDesc* pEffectClassDesc );
		virtual ~CEffectUnitProp(void);

		virtual const GUID		GetGUID()							{ return m_pEffectClassDesc->GetClassID(); 						}
		virtual const char*		GetClassDescription()				{ return m_pEffectClassDesc->GetClassDescription();				}
		virtual uint32			GetID()								{ return m_ID;													}
		virtual const char*		GetEffectFileName();
		virtual const char*		GetEffectName()						{ return CEffectClassEx::GetEffectName( (EBaseEffectUnit)m_ID );}
		virtual const char*		GetEffectClassName()				{ return m_EffectClassName.c_str();								}
		virtual const char*		GetCustomName()						{ return m_CustomName.c_str();									}
		virtual void			SetCustomName( const char* str )	{ m_CustomName = str;											}
		virtual eUpdatePosType	GetUpdatePosType()					{ return m_eUpdatePos;											}
		virtual const char*		GetAttachName()						{ return m_szAttachName.c_str();								}
		virtual bool			IsFitAniName( const char* AniName ){ return m_szAniName.find( AniName ) != m_szAniName.end();	}
		virtual uint32			GetTimeRang()						{ return m_TimeRange;											}
		virtual void			SetTimeRang( uint32 TimeRang )		{ m_TimeRange = TimeRang;										}

		virtual void			SetEffectLod(float Lod);
		virtual float			GetEffectLod(void);

		virtual const EMap<float,CVector3f>&	GetOffset()				{ return m_vecOffset; };
		virtual const EMap<float,ITexture*>&	GetTexture()			{ return m_Texture; };
		virtual const EMap<float,CColor>&		GetColor()				{ return m_Color; };
		virtual const EMap<float,BYTE>&			GetScale()				{ return m_Scale; };
		virtual const EMap<float,CVector3f>&	GetXYZcale()			{ return m_XYZcale; };
		virtual const EMap<float,CVector3f>&	GetUVRotateCenter()		{ return m_UVRCenter; };
		virtual const EMap<float,float>&		GetEmissionMinRadius()  { return m_EmissionMinRadius; };
		virtual const EMap<float,float>&		GetEmissionMaxRadius()  { return m_EmissionMaxRadius; };

		virtual int32			LoadBin( IEffectRead& File );
		virtual int32			SaveBin( IEffectWrite& File );
		virtual int32			LoadFullRes();
		virtual void			_FreeRes(void);
		virtual void			_Reclaim(void);
		virtual int32			AddRef();
		virtual void			_CalculateAABB();
	};

	//////////////////////////////////////////////////////////////////////////

	class EFFECT_API CEffectUnit :	public IEffectUnit
	{
	protected:
		CEffectUnitProp*		m_pEffectUnitProp;
		IEffectUnitHandler*		m_pUnitHandler;

		int16					m_nSkeletalID;
		uint8					m_nSocketID;

		float					m_FrameTime;
		float					m_DeltaTime;

		uint32					m_CurTime;
		uint32					m_StartTime;
		uint32					m_DelayTime;
		int32					m_DistortTime;
		//bool					m_bRefreshMat;

		//AniResult				m_ParenResult;
		//float					m_FrameRatio;
		//float					m_InvFrameRatio;
		CRenderable*			m_pParent;		// 父对象

		CMatrix					m_matWorld;

		
		CVector3f				m_vecRotateCenter;
		//float					m_Velocity;

		bool					m_bVisible;
		bool					m_bRefreshMat;
		

		friend class CEffectClassDesc;
	public:

		CEffectUnit( IEffectUnitProp* pEffectUnitProp );
		virtual ~CEffectUnit(void);

		uint32						GetCurTime( void )									{ return m_CurTime; }

		virtual void				GetDesc( char * str )								{}
		virtual bool				IsValid()											{ return true;						}
		virtual ERenderObjType		GetType()											{ return ROT_UNKNOW;				}
		virtual IEffectUnitProp*	GetProp()											{ return m_pEffectUnitProp;			}
		virtual const CMatrix&		GetMatrix()											{ return m_matWorld;				}
		virtual CRenderable*		GetParent()											{ return m_pParent; };
		virtual bool				SetProp( IEffectUnitProp* pEffectUnitProp )					;
		virtual void				SetStartTime( uint32 STime,uint32 DTime );
		virtual void				ResetTime( uint32 STime,uint32 DTime );
		virtual void				ClearLink( void )										{ return;					};
		virtual int32				AddLinkObj( CRenderable*, const char*,  bool )		{ return -1;				};
		virtual void				DeleteLinkObj( const char* )						{ return;					};
		virtual int32				RefLinkObj( const char*,  CRenderable** )			{ return -1;				};
		virtual void				OnLinked( CRenderable* );
		virtual void				OnUnlinked( void );
		//virtual void				OnPreRender( const AniResult&, float )				;
		virtual void				SetRenderCallback( IEffectUnitHandler* pUnitHandler );
		virtual const float			GetAABBHeight();
		virtual IntersectFlag		IntersectRay( const CVector3f& rayPos, const CVector3f& rayDir ){return IF_IMPOSSIBLE;}
		virtual void				RenderChild(IEffectUnit* pEffectUnit, uint32 uCurTime, RenderObjStyle* pRORS);
		virtual void				SetBindID( int16 SkeletalID, uint8 SocketID );
		int16						GetBindSkeletalId(void);
		uint8						GetBindSocketId(void);
		virtual const AniResult		Render( const CMatrix& matWorld, uint32 uCurTime,  RenderObjStyle* pRORS = NULL );		//渲染模型	
		virtual void				SetRenderVelocity( float Velocity , int32 id = -1 );
		virtual const EFFECT_RESULT	RenderEx( const CMatrix& matWorld,uint32 uCurTime,  RenderObjStyle* pRORS = NULL );	//渲染模型	
		virtual const AniResult		Render(  const CMatrix& matWorld, RenderObjStyle* pRORS = NULL );
		virtual const EFFECT_RESULT	RenderEx( const CMatrix& matWorld,  RenderObjStyle* pRORS = NULL ) = 0;	//渲染模型	
		virtual void				SetVisible( bool isVisible ){ m_bVisible = isVisible; };
		virtual bool				IsVisible() const { return m_bVisible; }
		virtual bool				UpdateTime(uint32 uCurTime);
				float				GetUnitPercent(void);
				bool				IsInRenderTime(void);
				bool				IsEndTime(void);
				void				SetDistortTime(uint32 dTime);
		virtual void				SetRotate( float fY ){};
		virtual EMap<float,CVector3f>& GetDestMap(void) 
		{
			static EMap<float,CVector3f> DestMap;
			return DestMap; 
		};
		float						CalculateDifZBias( float zDis, const CMatrix& matWorld, const CMatrix& matCam, const float f, const float n );
	};

}
