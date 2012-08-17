#include "stdafx.h"
#include "ConfigFileParser.h"

	CConfigFileParser* CConfigFileParser::ms_Inst = 0;

	CConfigFileParser::CConfigFileParser()
		: m_uCurrIndex(0)
		, m_uNodeCount(0)
	{
		
	}

	CConfigFileParser::~CConfigFileParser()
	{
		m_vecSourcePath.clear();
	}

	void CConfigFileParser::Reset()
	{
		m_szConfigFile	= "";
		m_uCurrIndex	= 0;
		m_uNodeCount	= 0;
		m_vecPicName.clear();
		m_vecSourcePath.clear();
		m_vecTargetPath.clear();
	}

	CConfigFileParser* CConfigFileParser::GetInst()
	{
		if ( ms_Inst == 0 )
		{
			ms_Inst = new CConfigFileParser();
		}
		return ms_Inst;
	}

	bool CConfigFileParser::SetConfigFilePath( const string &szConfigFile )
	{
		ifstream file;
		file.open( szConfigFile.c_str() );
		if ( !file.good() )
		{
			cout << "配置文件: " << szConfigFile << " 不存在\n";
			Reset();
			return false;
		}

		string szCurrLine;
		getline(file, szCurrLine);
		_Trim(szCurrLine);

		uint32		 uNodeCount;
		stringstream ss;
		ss << szCurrLine;
		ss >> uNodeCount;
		m_uNodeCount = uNodeCount;

		while ( uNodeCount-- != 0 )
		{
			if ( !_ParseNode(file) )
			{
				cout << "解析节点: " << m_uNodeCount-uNodeCount+1 << "失败\n";
				return false;
			}
		}	
		
		return true;
	}

	bool CConfigFileParser::_ParseNode( ifstream &file )
	{
		const uint32 NODE_INFO_COUNT = 3;
		
		// 解析节点时不能出现文件读完情况
		if ( file.eof() )
		{
			return false;
		}

		string szCurrLine;
		string szTemp[NODE_INFO_COUNT];
		for ( int i=0; i<NODE_INFO_COUNT; ++i )
		{
			// 解析节点时不能出现文件读完情况
			if ( file.eof() )
			{
				return false;
			}

			getline(file, szCurrLine);
			_Trim(szCurrLine);

			// 删除空行
			while ( szCurrLine == "" )
			{
				// 解析节点时不能出现文件读完情况
				if ( file.eof() )
				{
					return false;
				}

				getline(file, szCurrLine);
				_Trim(szCurrLine);
			}

			szTemp[i] = szCurrLine;
		}

		m_vecPicName.push_back(szTemp[0]);
		m_vecSourcePath.push_back(szTemp[1]);
		m_vecTargetPath.push_back(szTemp[2]);

		return true;
	}

	bool CConfigFileParser::GetNextNode( Node &sNextNode )
	{
		if ( m_uCurrIndex == m_uNodeCount )
		{
			return false;
		}

		sNextNode.szPicName = m_vecPicName[m_uCurrIndex];
		sNextNode.szSrcPath = m_vecSourcePath[m_uCurrIndex];
		sNextNode.szTgtPath = m_vecTargetPath[m_uCurrIndex];

		++m_uCurrIndex;
		return true;
	}

	string CConfigFileParser::_Trim( string &str )
	{
		const string WHITE_SPACE = " \t\r\n\f\v";
		str.erase(str.find_last_not_of(WHITE_SPACE)+1);
		return str.erase(0,str.find_first_not_of(WHITE_SPACE));
	}