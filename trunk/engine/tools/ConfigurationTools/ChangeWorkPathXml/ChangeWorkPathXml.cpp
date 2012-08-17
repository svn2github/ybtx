// ChangeWorkPathXml.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
void ChangeXML(const char * FileName,const char * NodeName,const char * NewNodeText);

int _tmain(int argc, _TCHAR* argv[])
{
	map<string,string> mapArgv;
	const char * FileName_t=argv[1];
	for(int i=0;i<((argc-1)/2);i++)
	{
		mapArgv.insert(make_pair(argv[2*i+2],argv[2*i+3]));
	}
	map<string,string>::iterator iter = mapArgv.begin();
	for(;iter!=mapArgv.end();iter++)
	{
		ChangeXML(FileName_t,(iter->first).c_str(),(iter->second).c_str());
	}
	
	return 0;
}

void ChangeXML(const char * FileName,const char * NodeName,const char * NewNodeText)
{
	TiXmlDocument *pDoc = new TiXmlDocument();
	pDoc->LoadFile(FileName);
	TiXmlNode* pRoot = pDoc->RootElement();
	TiXmlElement *pNode =NULL;
	TiXmlNode* pNodeXX = pRoot->FirstChild("Work_Path");
	TiXmlElement* pXmlElem11 = pNodeXX->FirstChildElement(NodeName);
	pXmlElem11->ReplaceChild(pNodeXX->FirstChild(NodeName)->FirstChild(),TiXmlText(NewNodeText));
	pDoc->SaveFile(FileName);
}