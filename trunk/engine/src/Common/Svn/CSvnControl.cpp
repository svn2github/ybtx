#include "stdafx.h"
#include "CSvnControl.h"
#include <stdio.h>

using namespace std;
namespace sqr
{
	const string CSvnControlImp::st_ComList[] ={"log","checkout","add","mkdir","status","update","commit","lock","unlock","info","delete"  };

	CSvnControl::CSvnControl(void)
	{
		m_pImp = new CSvnControlImp;
	}

	CSvnControl::CSvnControl(const char* svn_path)
	{
		m_pImp = new CSvnControlImp;
		initialize(svn_path);
	}

	CSvnControl::~CSvnControl()
	{
		delete m_pImp;
		m_pImp = NULL;
	}

	bool CSvnControl::initialize(const char* svn_path)
	{
		m_pImp->strWorkUrl = svn_path;
		if(m_pImp->strWorkUrl[m_pImp->strWorkUrl.size()-1] != '\\' || m_pImp->strWorkUrl[m_pImp->strWorkUrl.size()-1] != '/')
			m_pImp->strWorkUrl.push_back('/');

		if( !svn(eSVNC_INFO,"","") )
			return false;

		if(!m_pImp->_SvnLineGet("URL",m_pImp->strSvnUrl))
			return false;

		if(!m_pImp->_SvnLineGet("°æ±¾¿â¸ù",m_pImp->strSvnRoot))
			return false;
		return true;
	}

	const char*	CSvnControl::getSvnInfo(void)
	{
		return m_pImp->strInfo.c_str();
	}

	const char* CSvnControl::getSvnUrl(void)
	{
		return m_pImp->strSvnUrl.c_str();
	}

	const char* CSvnControl::getWorlPath(void)
	{
		return m_pImp->strWorkUrl.c_str();
	}

	const char* CSvnControl::getSvnRoot(void)
	{
		return m_pImp->strSvnRoot.c_str();
	}

	void 	CSvnControl::setUserName(const	std_string& user)
	{
		m_pImp->UserName = user;
	}

	void 	CSvnControl::setPassWord(const	std_string& pwd)
	{
		m_pImp->PassWord =pwd;
	}

	bool CSvnControl::svn( eSVNCom sc,		fileset& filenames ,const	std_string& msg )
	{
		return svn(sc, (const fileset&)filenames, msg);
	}

	bool CSvnControl::svn( eSVNCom sc, const	fileset& filenames ,const	std_string& msg )
	{
		string com = "svn " + CSvnControlImp::st_ComList[sc];

		fileset::const_iterator itr = filenames.begin();
		fileset::const_iterator eitr = filenames.end();

		if ( itr == eitr )
			com += m_pImp->strWorkUrl;
		else
		{
			while ( itr == eitr )
			{
				com += " " + m_pImp->strWorkUrl + string(*itr);
				++itr;
			}
		}

		switch(sc)
		{
		case eSVNC_COMMITE:
			if(msg.size()!=0)
				com += " -m " + msg;
			break;
		default:
			break;
		}

		return m_pImp->_Svn(com);
	}

	bool CSvnControl::svn( eSVNCom sc,		std_string& filename ,const	std_string& msg )
	{
		return svn(sc, (const std_string&)filename, msg);
	}

	bool	CSvnControl::svn(eSVNCom sc, const	std_string& filename ,const	std_string& msg)
	{
		string com = "svn " + CSvnControlImp::st_ComList[sc] + " "+ m_pImp->strWorkUrl;
		if(filename.size()!=0)
			com += filename;

		switch(sc)
		{
		case eSVNC_COMMITE:
			if(msg.size()!=0)
				com += " -m " + msg;
			break;
		default:
			break;
		}
		return m_pImp->_Svn(com);
	}

	bool	CSvnControlImp::_IsErr(void)
	{
		return strInfo.size() == 0 || strInfo.find("svn:")!=-1;
	}

	bool	CSvnControlImp::_Svn(string& com)
	{
		if(!UserName.empty())
			com += " --username " + UserName;

		if(!PassWord.empty())
			com += " --password " + PassWord;

		com += " --non-interactive --no-auth-cache";

		com += " 2>&1";
		FILE* pIO = SVNopen(com.c_str(),"r");

		strInfo.clear();
		if(pIO!=NULL) 
		{
			const int BufSize = 256;
			char buffer[BufSize + 1];
			buffer[BufSize] = 0;
			int n = 0;
			do{
				n = (int)(fread(buffer,(int)(sizeof(char)),BufSize,pIO));
				if(n<BufSize)
					buffer[n] = 0;
				strInfo += buffer;
			}while(n==BufSize);

		} 
		SVNclose(pIO);
		return !_IsErr();
	}

	bool CSvnControlImp::_SvnLineGet( const string& key, string& line)
	{
		int pos = (int)(strInfo.find(key));
		if(pos == -1)
			return false;
		string imp_key = ": ";
		
		line = strInfo.substr(pos);
		pos = (int)(line.find_first_of(imp_key) + imp_key.size());
		line = line.substr(pos,line.find_first_of("\n")-pos);
		return true;
	}
}
