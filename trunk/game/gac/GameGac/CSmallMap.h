#pragma once
#include "CCommpressFile.h"
#include "SQRWnd.h"
#include "CCoreObjectFollower.h"
#include "GameDef.h"
#include "SQRButton.h"
#include "CTick.h"
#include "CElementManager.h"

class MAPROLE
{
	map<CCoreObjectFollower*,WND_IMAGE_LIST*>	m_aRoleInfo;
public:
	MAPROLE();

	~MAPROLE();

	const map<CCoreObjectFollower*,WND_IMAGE_LIST*>& GetRoleInfo()const
	{
		return m_aRoleInfo;
	}
	size_t Size()const
	{
		return m_aRoleInfo.size();
	}

	bool PushItem(CCoreObjectFollower* Obj)
	{
		map<CCoreObjectFollower*,WND_IMAGE_LIST*>::iterator it = m_aRoleInfo.find(Obj);
		if(it==m_aRoleInfo.end())
			return m_aRoleInfo.insert( pair<CCoreObjectFollower*,WND_IMAGE_LIST*>(Obj, new WND_IMAGE_LIST) ).second;
		return true;
	}

	bool DelItem(CCoreObjectFollower* Obj);

	WND_IMAGE_LIST& operator[](CCoreObjectFollower* Obj)
	{
		return *m_aRoleInfo[Obj];
	}
};

class MapImageData : public SQRGuiMallocObject
{
public:
	UIString	imageStrInfo;
	int32		imageHeight;
	int32		imageWidth;
	void InitMapImageData(const string& imageStrInfo, int32 imageHeight, int32 imageWidth);
};
//--------------------------------------------------------------------------------------------
class CSmallMapTex : public CElementNode
{
	friend class CSmallMapCoder;
public:
	CSmallMapTex(CElementManager* pParent , URegionID id);
	~CSmallMapTex();
	ITexture*		GetTexture(void);
	uint32			GetXRegion(void);
	uint32			GetYRegion(void);
protected:
	ITexture*	m_MapText;
	uint32		m_uX;
	uint32		m_uY;
};

inline ITexture* CSmallMapTex::GetTexture(void)
{
	return m_MapText;
}

inline 	uint32 CSmallMapTex::GetXRegion(void)
{
	return m_uX;
}

inline  uint32 CSmallMapTex::GetYRegion(void)
{
	return m_uY;
}
class CCharacterFollower;

class CSmallMap : 
	public sqr::SQRWnd, 
	public CTick,
	public CElementManager
{
	DYNAMIC_DECLARE_WND( CSmallMap )
public:
	enum EMapRoleType
	{ 
		eMRT_None = -1,
		eMRT_EnemyNpc, 
		eMRT_FriendNpc , 
		eMRT_TeamMate , 
		eMRT_EnemyPlayer ,	
		eMRT_FriendPlayer ,	
	};

	static const double MapRotateRadian;	///< 地图偏转的角度
	static const uint32	st_uRegionScale;
	static const uint32 st_uCenterId;
	static const uint32 ms_uImageSize;

public:
	CSmallMap();
	virtual ~CSmallMap(void);
	void SetScale(float fScale);
	void InitData( uint8 uHorBlocks, uint8 uVerBolcks, uint8 uPiecesPerBlock, uint8 uPiecesSize, bool bBigMap = false );
	void DrawWndBackground();
	int32 DestroyWnd();
	void ClearUp();

	/// @brief 初始化地图数据
	virtual void InitMapData( const TCHAR* szMapFile, uint8 uSceneID );
	virtual	void SetPlayerPos( const CVector3f& pos );
	/// @brief 得到npc和其他玩家的信息
	void ReceiveRoleData( CCoreObjectFollower* Obj, uint8 nType );
	bool RemoveRoleData( CCoreObjectFollower* Obj );
	void SetSmallMapWnd(SQRWnd * pWnd);
	void SetPlayerTex();
	void ClearAllNpcAndPlayerInfo();
	void ClearNpcOrPlayerInfoByID(uint32 followerID);
	void SetNpcOrOtherPlayerInfo(const CFPos &pos, const string &name, EMapRoleType type, SQRButton* btn, uint32 followerID);//遍历主角范围内的npc和玩家，并显示在小地图中
	/// @brief 设置当前场景的信息
	/// @param 场景宽度
	/// @param 场景高度
	/// @param 地图逆时针旋转的角度（常用45）
	/// @param 小地图文件的名称
	/// @param 场景的ID
	void SetSceneData( uint32 nWidthInGrid, uint32 nHeightInGrid, float fCWRotDeg, const TCHAR* szMapFile, uint8 uSceneID );
	/// @brief 绘制主角
	void DrawMainPlayer();
	void SetPlayerDir(float x, float y);//{ m_MainPlayerDirection = CVector2f(x, y); }
	void SetMainPlayerPixelPos(float x, float y);
	CCommpressFile* GetMapFileData(){ return NULL; }
	//绘制NPC和其它玩家
	void DrawFollower();
	void SetMainPlayer(uint32 uMainPlayerID);
	void AddEntity(uint32 uGameEntityID,const char* szCharName, EMapRoleType eRoleType);
	void ChangeEntityImageType(uint32 uGameEntityID,EMapRoleType eRoleType);
	void RemoveEntity(uint32 uObjID); 
	void AddShowRoleType(uint32 uRoleType);
	void RemoveShowRoleType(uint32 uRoleType);
	void InitNpcAndPlayerImageData();
	void OnTick();
	CElementNode* _CreateNode( URegionID ID );
protected:
	CIRect		m_rcPiece;
	MAPROLE		m_aRoleInfo[eMR_Count];		///< 角色信息
	SQRWnd		*m_pWnd;
	SQRWnd		*m_pMainPlayerWnd;
	SQRWnd		*m_pCenterBtn;
	//
	CVector2f   m_MainPlayerDirection;		///< 主角的方向
	CVector2f	m_MainPlayerPosition;		///< 主角位置（像素为单位）
	//ITexture*	m_MapTexture[3][3];			///< 地图背景图片
	CVector2I	m_Test[3][3];
	float       m_fScale;					///< 地图缩放系数
	// 场景信息
	uint32      m_nWidthInPixel;			///< 场景宽度（像素为单位）
	uint32      m_nHeightInPixel;			///< 场景高度（像素为单位）
	float       m_fCWRotRad;				///< 地图逆时针旋转弧度

	CRenderTexture*				m_pBackTexture;
	sqr::CElementCoderEx*		m_pCoderEx;
	//绘制用的变量
 	CFRect		m_wndRect;
 	CVector2f	m_dest[4];
	CVector2f	m_BackTexUV[4];
	uint32		m_XRegion;
	uint32		m_YRegion;
	string		m_SceneName;
	uint32		m_MaxWidth;
	uint32		m_MaxHight;
	CVector2f	m_Rect[3][3][4];
	float		m_fDeltaX;
	float		m_fDeltaY;
	float		m_fCenterX;
	float		m_fCenterY;
	float		m_fBlockWidth;
	float		m_fBlockHeight;
	uint32	m_uDirectorID;
	static UIVector<MapImageData> ms_vecImageType;
	typedef UIMap<uint32, pair< SQRButton*, EMapRoleType> > MapFollowerBtn;
	MapFollowerBtn m_mapFollowerMapBtn;
	UISet<uint32> m_setShowRoleType;
private:
	void SetRoleTex( WND_IMAGE_LIST* pItem, EMapRole eRole );
	void DrawSmallMap();
	void GetMainPlayerBtnPos(int32& xPos, int32& yPos);
	uint32 GetRoleType(CCharacterFollower* pRole);
	bool IsNeedUpdatePlayerPos(SQRButton* pBtn ,const CFPos &Pos);
	SQRButton* CreateBtn(const char* szName);
	void SetBtnImage(SQRButton* pBtn, const char* szImageInfo);
	void SetBtnRect(SQRButton* pBtn, const CFPos Pos,int32 ImageWidth, int32 ImageHeight);
	void GetOtherObjBtnPos(const CFPos pos, int32 &XPos, int32 &YPos);
};