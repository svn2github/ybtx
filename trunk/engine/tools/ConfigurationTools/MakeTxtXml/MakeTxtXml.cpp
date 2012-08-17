// MakeTxtXml.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MakeTxtXmlAuto.h"
#include "stdlib.h"
#include "CExpLog.h"
#include "CSplitInfo.h"

int _tmain(int argc, _TCHAR* argv[])
{
	cout<<"输入要转换的文件路径例如../../cfg/data/src/action/ActionSkill_Common.dif"<<endl;
	string strFileName;
	cin>>strFileName;
	CSplitInfo::GetInst()->Init(argc, argv);
	BeginToPrint("MakeTxtXml");
	vector<bool> vec;
    GetColType(strFileName,vec);
	string strXml = strFileName.substr(0,strFileName.size()-4);
	strXml.append(".xml");
	ForceDeleteFile(strXml);
	WriteToXml(strXml,vec,strFileName);
	EndToPrint();
	cout<<"转换完毕" <<endl;
	system("pause");
	return 0;
}

