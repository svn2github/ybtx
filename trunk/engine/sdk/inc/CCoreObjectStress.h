#pragma once

/*
CCoreObjectStress
简单的对象系统，里面保存了每一个创建director的GlobalId，与对应的Connection关联
在lua里面通过调用SetPostion强制改变对象位置，测试移动时候aoi压力用
*/

namespace sqr
{
	class CConnStress;
	class CGas2GacOC_Create_Still_Object;
	class CGas2GacOC_Create_Moving_Object;
	class CGas2GacOC_Destroy_Object;
	class CGas2GacOC_Set_Time_Distorted_Ratio;

	class CCoreObjectStress
	{
	public:
		static CCoreObjectStress *& MainObject();
		void OnServerCommand(CConnStress *pConn, const CGas2GacOC_Create_Still_Object* pCmd);
		void OnServerCommand(CConnStress *pConn, const CGas2GacOC_Create_Moving_Object *pCmd) {}
		void OnServerCommand(CConnStress *pConn, const CGas2GacOC_Destroy_Object *pCmd) {}
		void OnServerCommand(CConnStress *pConn, const CGas2GacOC_Set_Time_Distorted_Ratio *pCmd) {}

		static void SetBeginPosition(CConnStress* pConn, float x, float y);
		static void MoveTo(CConnStress* pConn, float x, float y);
		static void SetEndPosition(CConnStress* pConn, float x, float y);
	private:
		CCoreObjectStress();
		~CCoreObjectStress();
	};
}

