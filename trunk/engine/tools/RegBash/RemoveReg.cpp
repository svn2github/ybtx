
#include "stdafx.h"

#include "RemoveReg.h"


void RemoveReg(void)
{
	try
	{
		RegDeleteKey(HKEY_CLASSES_ROOT, _T(".sh"));
		RegDeleteKey(HKEY_CLASSES_ROOT, _T("ybtx_bash\\shell\\open\\command"));
		RegDeleteKey(HKEY_CLASSES_ROOT, _T("ybtx_bash\\shell\\open"));
		RegDeleteKey(HKEY_CLASSES_ROOT, _T("ybtx_bash\\shell"));
		RegDeleteKey(HKEY_CLASSES_ROOT, _T("ybtx_bash\\DefaultIcon"));
		RegDeleteKey(HKEY_CLASSES_ROOT, _T("ybtx_bash"));
	}
	catch(...)
	{
		printf("Delete key error!");
	}

	printf("成功移除文件关联！");
}

