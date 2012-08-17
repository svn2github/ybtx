
#include "stdafx.h"

#include "BaseTypes.h"
#include "CPatcher.h"
#include "CPatcherUpdateHandler.h"


using namespace sqr;

#define  PATCH_FILE  L"download/patch_0.0.13.ybtx"


//DEFINITION_OF_OPERATOR_NEW

void StartPatch()
{
	sqr::CPatcher patch;
	CPatcherUpdateHandler handler;
	// 启动器的升降机由选定服务器版本号与当前版本号对比决定
	patch.CheckHeadInfo(&handler, PATCH_FILE); // MemoryBoundCheck(1000);
}


int _tmain(int argc, _TCHAR* argv[])
{
	// 内存越界检查
	//SetBoundCheckEnabled(true);

	// 开始内存泄露查找
	//SetFindLeakageEnabled(true);

	//if (IsFindLeakageEnabled())
	//	BeginWatchingLeakage();

	//CMapFileMgr::Inst().HandleMapFile("F:/ybtx/programmer/trunk/bin/Debug/ApplyPatch.map");
	//SetMemFileName("leak", "F:/ybtx/programmer/tools/dbti/var/Client/ver/ybtx_69179_c/0.6.1/bin/Release");

	// 开始上补丁
	StartPatch();

	// 结束内存泄露查找
	//if (IsFindLeakageEnabled())
	//	EndWatchingLeakage();
	//
	//SetFindLeakageEnabled(false);

	return 0;
}

