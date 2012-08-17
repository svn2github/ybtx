#pragma once
//#include "TPart.h"
#include "CDir.h"
#include "CRenderSpace.h"
#include "CLapseDeleteQueue.h"
#include "CModel.h"
#include "CRenderDialog.h"
#include "IRenderTime.h"
#include "CEffectPlayer.h"

namespace sqr
{
class CCamera;
class  IEffectGroup;
class  IEffectUnitHandler;

enum RES_TYPE
{
	RFT_ARP = MAKEFOURCC( 'A', 'R', 'P', 0 ),		//模型配置文件
	RFT_ARA = MAKEFOURCC( 'A', 'R', 'A', 0 ),		//动画配置文件
	RFT_ARE = MAKEFOURCC( 'A', 'R', 'E', 0 ),		//特效文件
	RFT_AGP = MAKEFOURCC( 'A', 'G', 'P', 0 ),		//动作包文件
	RFT_SKE = MAKEFOURCC( 'S', 'K', 'E', 0 ),		//骨架文件
};

enum EffectCheckResult
{
	eECR_NoExist  = 0,
	eECR_Normal   = 1,
	eECR_BoneFollow = 2,
	eECR_FrameSynch = 4,
	eECR_NormalSynch = 5,
	eECR_BoneSynch = 6,
};

struct ResCreateInfo : public CSceneMallocObject
{
	ResCreateInfo()
	{
		pRes = NULL;
		eResType = 0;
	}
	GraphicRes*		pRes;
	uint32					eResType;		//资源类型:ARA,ARP,ARS,ARE,ARAS		(必填)
	SString					szFileName;
	SString					szClassName;	//资源类名							(必填)
	SString					szParam;		//类型参数
};

class CRenderSystem;
class CRenderScene;
class CEffectPlayer;
struct WaterFx : public CSceneMallocObject
{
	CRenderScene*	pScene;
	uint32			uPreTime;
	uint16			uPrePosX;
	uint16			uPrePosY;
};

struct AniParam : public CSceneMallocObject
{
	//播放的动作
	SString							m_PreAction;
	//播放的动作起始帧
	int16							m_nFrameStart;
	//播放的动作结束帧
	int16							m_nFrameEnd;
};

class CRenderObject
			:public CRenderSpace
			,public CModel
			,public CEffectPlayer
{
private:
	friend class CRenderScene;
	friend class CRenderSystem;
	friend class CMiniScene;
	friend class CTestSuiteCoreObjectClient;
	friend class CObjectDialog;
protected:

protected:
	bool			m_IsVailedShow;
	uint32			m_uCurTime;
	float			m_EffectVelocity;

	//可渲染物体
	IRenderTime*	m_pTime;

	ERenderObjStyle m_RenderStyle;
	PortraitInfo*	m_pPortraitInfo;	///< 头像信息
	SString			m_PortraitClassName;

	//方向
	uint8			m_uCurDirection;

	//模型初始倍数
	CMatrix			m_FixMat;
	//float			m_fSize;
	
	//当前帧经过的时间
	uint64			m_uFrameTime;
	//上一帧的时刻，用来计算当前帧经过的时间
	uint64			m_uLastFrameTime;
	//渐变最终放大倍数
	float			m_fFinalScale;
	//渐变最终时刻
	uint64			m_uFinalTime;
	//渐变完成标志
	bool			m_bScaleIsFinal;
	bool			m_bCanBeSelect;

	//透明变化
	uint8			m_uAlphaDes;
	uint8			m_uAlpha;
	uint8			m_rDiffuse;
	uint8			m_gDiffuse;
	uint8			m_bDiffuse;

	bool			m_IsAsynLoadPiece;
	
	//动作分离相关
	typedef SMap< int32, AniParam >		PartAniMap;
	PartAniMap							m_PartAni;	
	int32								m_ActivePart;

	//方向
	uint8								m_uDesDirection;

	SString								m_AniFileName;
	

	uint8								m_rAmbient;
	uint8								m_gAmbient;
	uint8								m_bAmbient;



	///地表材质索引
	int8								m_nMaterialIndex;

	//产生水特效
	WaterFx*							m_pWaterFx;

	//待创建的物体
	SList<ResCreateInfo>				m_ListResInCreate;
	SMap<WORD, bool>					m_FootStepFrameList;
	WORD								m_uLastKeyFrame;
	//是否受地形高度影响
	bool								m_bTerrainheight;

	void				IntAddPiece( ResCreateInfo& Info );
	void				CreateBaseEffect( ResCreateInfo& Info );
	void				_Render( void );

	bool				_RegisterEffect( EffectNode* pEfxNode );
	bool				_LoadedEffect( EffectNode* pEfxNode );
	void				_ClearEffect(int32 nPart);

	void				_DestoryRenderChild(CRenderSpaceNode* pObj);
	void				_UpdateAttachChild(CRenderSpaceNode* pChild);
	CRenderObject(IRenderTime* pTime = NULL,ILogicHandler* pHandler = NULL);
	virtual ~CRenderObject(void);
		
	uint32				m_LapseTime;
public:
	static const DWORD  st_RenderObjectType;
	DWORD				GetNodeType()	{ return CRenderObject::st_RenderObjectType; };
	//临时代码
	void				SetLapseTime( uint32 tLapse )	{ m_LapseTime = tLapse; }
	uint32				GetLapseTime(void)				{ return m_LapseTime;	}

	void				SetCanBeSelect(bool isSel)		{ m_bCanBeSelect = isSel; }
	bool				GetCanBeSelect(void)			{ return m_bCanBeSelect; }

	void				Release();

	//创建渲染对象, 参数等同于ResOnCreate结构
	bool				AddPiece( RES_TYPE eResType, const char* szFileName, const char* szClassName, const char* szParam );
	void				SetAsynLoadPieceSwitch(bool bEnable);
	void				SetPortraitPiece(const char* className);
	PortraitInfo*		GetPortraitInfo() const;


	//void				AbsolutecalculateUpBody( int32 boneid );
	
	//是否有效
	bool				IsValid();

	//得到某个动作的长度
	int32				GetAniMaxFrame( const char* Action );
	//得到某个动作的对地速度
	float				GetGroundSpeed( const char* Action );

	// 得到某个动作某个关键帧的位置
	int32				GetFrameCountToFrameStr( const char* Action, const char* szFrameStr, int nFrameStart, int nFrameEnd, int nStr );

	//指定起始帧播放
	bool				DoAni( const char* szAniFileName,int32 nPartId,bool bLoop,uint32 uDelayTime,int16 nStartFrame,int16 nEndFrame , float AniSpeed = -1.0f  );
	bool				DoAni( const char* szAniFileName, bool bLoop, int16 nStartFrame , float AniSpeed = -1.0f  );
	bool				DoAni( const char* szAniFileName,int32 nPartId,bool bLoop , float AniSpeed = -1.0f );
	bool				DoAni( const char* szAniFileName,bool bLoop , float AniSpeed = -1.0f );
	
	bool				DoDefaultAni(int32 nPartId, uint32 uDelayTime, float AniSpeed = -1.0f );
	bool				DoAniFromPart( int32 nPartId, uint32 uDelayTime = 200 );
	void				StopAni( int32 nParId );
	void				StopAni( int32 nParId, int32 DelayTime );

	//
	void				SetTerrainInfluence( bool bValue );
	//删除当前的已有骨架
	void				Delframework();
	//删除模型身上所有Piece
	void				RemoveAllPiece(void);

	//移除渲染对象的主体部件
	bool				RemovePiece( const char* szClassName );
	//显示或隐藏部件中的某个piece加入缓冲
	void				ShowPiece(const char* szPieceName,bool bShow);

	//设置渲染对象的方向
	void	SetDirection( const CDir& Dir );
	
	//设置渲染对象对象所在格子的法线
	void	SetNormal( const CVector3f& Normal );
	//得到渲染对象的模型中心坐标的世界坐标
	const CVector3f		GetCenterPosition();
	//根据渲染对象骨骼名得到骨骼世界坐标
	const CVector3f		GetSkeletalPositionByName(const char* szSkeletalName);
	//得到渲染对象的动作原点的世界坐标
	void	GetRootPosition(CVector3f* pVec);
	//得到渲染对象的动作原点的平面坐标
	CFPos	Get2DPosition();


	//得到方向
	const CDir&	GetDirection();
	void GetDirection(CDir& dir);

	//设置渲染对初始模型尺寸
	void		SetSize( float fSize );
	//得到渲染对象当前放大倍数
	float		GetScale();
	//得到渐变最终放大倍数
	float		GetFinalScale();
	//设置渲染对象放大倍数
	void		SetScale( float fScale );
	//设置渐变最终放大倍数
	void		SetFinalScale( float fScale, uint64 uFinalTime );
	//渐变更新当前放大倍数
	void		UpdateScale( uint64 uCurTime );

	////得到当前矩阵
	//const CMatrix& GetMatrix();

	//得到渲染对象
	//CRenderable*	GetRenderObj();

	float GetCurFrameNum();

	virtual void	GetAttachMatrix( CMatrix& matLink, uint32 uChildLinkID );
	//virtual CVector3f GetObjectPosition();

	//得到渲染对象高度
	float				GetObjHeight();
	////得到渲染对象包络盒
	IntersectFlag		IntersectRay( const CVector3f& rayPos, const CVector3f& rayDir );

	//得到透明度
	uint8				GetAlphaValue();
	//设置透明度
	void				SetAlphaValue( uint8 uAlpha );
	void				SetAlphaValueEx( uint8 uAlpha );
	//设置顶点色
	void				SetDiffuse( CColor uColor );
	//设置环境色
	void				SetAmbient( CColor uColor );
	//获取顶点色
	CColor				GetDiffuse(void);
	//获取环境色
	CColor				GetAmbient(void);
	//产生水特效
	void				MakeWaterFx( CRenderScene* pRenderScene );

	//渲染前调用
	void				OnPreRender( uint32 uCurTime,CCamera* pCamera );
	//获取是否渲染
	bool				GetWillRender();
	
	bool				UpdateTime(uint32 uCurTime);
	//渲染物体
	void				Render(void);
	//渲染被选状态
	void				RenderSel( uint32 uCurTime, RenderObjStyle* pRS );

	void Activate();

	void				AddChild( CRenderSpaceNode* pChild, eLinkType linkType, const char* szAttachName );
	void				DelChild( CRenderSpaceNode* pChild );
	EffectCheckResult	CheckEffect(const char* szFileName,const char* szParam);
	
	void				SetRenderStyle( ERenderObjStyle Style );
	ERenderObjStyle		GetRenderStyle() const;
	void				HoldAniStill(int32 Partid);

	CRenderDialog*		CreateRenderDialog(eLinkType linkType, const char* szAttachName);
	CRenderObject*		CreateRenderObject(eLinkType linkType, const char* szAttachName);
	void				DestroyRenderDialog(CRenderDialog* pWnd);
	void				DestroyRenderObject(CRenderObject* pRenderObj);

	//这两个函数不应该在 CRenderObject身上 想办法拿走
	void				SetTerrainMaterialIndex(const int8 index);
	int8				GetTerrainMaterialIndex() const;

	void				ReCallFootStep(const WORD uCurKeyFrame);
	IRenderTime*		GetRenderTime(){return m_pTime;}

	int32				AddSkeletal( const char* name );

	void				SetVisible(bool isVisible);
public:
	bool				IsValidEx(CAnimate* pAni);
	void				_UpdateAniGrp(void);
	CAxisAlignedBox*	GetOrgAABB(void);
	CMatrix&			GetCenterMatrix(void);
	void				Update(void);
};


}

