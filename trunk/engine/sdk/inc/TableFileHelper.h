#pragma once 

namespace sqr
{
	enum ETableFileType
	{
		eTFT_Txt = 0,
		eTFT_Row,
		eTFT_None,
		eTFT_TypeError,
	};

	enum ETableFileFieldType
	{
		eTFFT_String = 0,
		eTFFT_Number,
		eTFFT_Empty,
	};

	enum ERowTableNodeType
	{
		eRTNT_Tree = 0,
		eRTNT_LeafNoData,	//leaf节点，没有任何数据
		eRTNT_LeafOneData,  //leaf节点，只有一个数据
		eRTNT_LeafMoreData,	//leaf节点，有多个数据
	};

	//获取tablefile的类型，为txt或者row
	ETableFileType GetTableFileType(const char* alias, const char* szFileName);

	void SetOriLangPath(const string& strOriLangPath);
	const string& GetOriLangPath();
	void SetCurLangPath(const string& strOriLangPath);
	const string& GetCurLangPath();
}
