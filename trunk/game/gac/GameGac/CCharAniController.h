#pragma once
//--------------------------------------------------
// 这些类是C++写的纯游戏逻辑方面的代码
// 尝试将 游戏逻辑 和 骨骼动画合并起来
// EndDuke 09/8/14 -> 传的参数还没有经过推敲
// 以人物转头作为试验
// 希望这几个类以后可以成为 布娃娃系统核心的基础结构
//--------------------------------------------------
namespace sqr
{
	class CRenderObject;
}

class CCharacterFollower;


// 人物骨骼IK控制 -> 兴趣点转向控制器
class CCharacterController 
{
	friend class CCharPartCtrl;
	friend class CHeadController;
	//friend class CSternumController;
	//friend class CCoxalController;
public:
	CCharacterController(CCharacterFollower* pCharacter);
	~CCharacterController();

	void	SetInterestedObj(CCharacterFollower* pTargetObj);	//设置兴趣对象
	bool	GetInterestedPos( CVector3f& outVec );			//获得兴趣点
	void	ClearInterested(bool Imme);							//清除兴趣
	void	SetCtrlAll(bool isCtrl ) { m_isCtrlAll = isCtrl; };
	bool	IsCtrlAll(void);
	bool	IsHasInterested(void);

protected:
	CCharacterFollower* m_pCharacter;
	CCharPartCtrl*	m_pHeadCtrl;		//头部控制器
	CCharPartCtrl*	m_pSternumCtrl;		//胸部控制器
	CCharPartCtrl*	m_pCoxalCtrl;		//腰部控制器

	CVector3f		m_vecInterestedPos;				//兴趣点
	bool			m_isHasInterested;				//是否存在兴趣点
	bool			m_isCtrlAll;	

	uint32 m_uTargetID;

private: //静态参数
	static string st_HeadBoneName;		//头
	static string st_SternumBoneName;	//胸
	static string st_CoxalBoneName;		//腰
	static string st_TargetBoneName;

	static const float	st_MaxDegree;
	static const float	st_MaxLookDegree;
};

inline bool CCharacterController::IsCtrlAll(void)
{
	return m_isCtrlAll;
}

inline bool CCharacterController::IsHasInterested(void)
{
	return m_isHasInterested;
}