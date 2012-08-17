#pragma once
#include "TServantMgr.h"

class CCharacterDictator;

/************************************************************************/
/*					服务端召唤兽管理器的职责：
*	1：存储主人召唤出来的所有类型召唤兽
*	2：负责管理召唤兽的和主人的存在关系，添加删除等操作
*	3：负责同步服务端和客户端之间关系（入：主人和召唤兽图标何时显示等等）
*	4：主人死亡或者下线后通知召唤兽做相关动作，以及控制是否删除该召唤兽
*/
/************************************************************************/

class CServantServerMgr : public TServantMgr<CCharacterDictator>
{
public:
	CServantServerMgr(CCharacterDictator* pOwner);
	void OnRemoveServant( CCharacterDictator* pServant );
	void ClearAllServantByMasterDie();
	void ClearAllServant();

	//主要用来执行主角死亡后对自己的召唤兽的操作（比如：干掉召唤兽）
	void ServantResByMasterDie(CCharacterDictator* pServant);

	//主角切场景或者下线先保存数据库再清除所有召唤兽
	void SaveAndClearAllServant(bool bSaveServant);
	void SaveAllServantInfo();

	virtual void OnServantChanged();
public:
	~CServantServerMgr();
};
