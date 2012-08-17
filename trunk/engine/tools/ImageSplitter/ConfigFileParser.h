#pragma once
#include "BaseDefs.h"

	class CConfigFileParser
	{
	public:
		struct	Node
		{
			string szPicName;	// 待切分图片的名称
			string szSrcPath;	// 地图区域信息文件路径
			string szTgtPath;	// 存储目标路径	
		};

	private:
		typedef vector<string> SourcePathVector;
		typedef vector<string> PictureNameVector;
		typedef vector<string> TargetPathVector;
		
		PictureNameVector	m_vecPicName;
		SourcePathVector	m_vecSourcePath;
		TargetPathVector	m_vecTargetPath;
		
		uint32				m_uCurrIndex;
		uint32				m_uNodeCount;
		string				m_szConfigFile;

	private:
		static CConfigFileParser *ms_Inst;
		CConfigFileParser();
		CConfigFileParser( const CConfigFileParser & );
		CConfigFileParser& operator = (const CConfigFileParser &);

		// ---- 删除字符串两端的空格
		string	_Trim(string &str);

		// ---- 解析文件中下一个节点
		bool	_ParseNode( ifstream &file );

	public:
		~CConfigFileParser();

		static CConfigFileParser* GetInst();

		// ---- 设置配置文件路径, 根据配置文件
		bool	SetConfigFilePath( const string &szConfigFile );

		// ---- 重置路径信息，以备下次使用
		void	Reset();

		// ---- 取得下一个切分配置文件节点, 若解析失败则返回false
		bool	GetNextNode( Node &sNextNode );
	};