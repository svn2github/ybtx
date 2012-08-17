#pragma once
#include "CommonDefs.h"

#ifndef _WIN32
#ifdef linux
#define SVNopen popen
#define SVNclose pclose
#else
#define SVNopen popen
#define SVNclose pclose
#endif
#else
#define SVNopen _popen
#define SVNclose _pclose
#endif

namespace sqr
{
	enum eSVNCom
	{
		eSVNC_SHOWLOG	= 0,
		eSVNC_CHECKOUT	= 1,
		eSVNC_ADD		= 2,
		eSVNC_MKDIR		= 3,
		eSVNC_STATUS	= 4,			
		eSVNC_UPDATE	= 5,
		eSVNC_COMMITE	= 6,
		eSVNC_GETLOCK	= 7,
		eSVNC_RELEASELOCK = 8,
		eSVNC_INFO		  = 9,
		eSVNC_DELETE	= 10,
		eSVNC_COUNT,
	};

	class	CSvnControlImp
	{
	public:
		static const string st_ComList[];

		string	strWorkUrl;
		string	strSvnUrl;
		string  strSvnRoot;
		string	strInfo;

		string	UserName;
		string	PassWord;

		bool	_SvnLineGet(const string& key, string& line);
		bool	_IsErr(void);
		bool	_Svn(string& com);
	};

	class	CSvnControl
	{
	public:
		typedef std::string				std_string;
		typedef std::set<std_string>	fileset;
	public:
		CSvnControl(const char* svn_path);
		CSvnControl(void);
		~CSvnControl();

		bool	initialize(const char* svn_path);
		bool	svn( eSVNCom sc, const	std_string& filename ,const	std_string& msg );
		bool	svn( eSVNCom sc,		std_string& filename ,const	std_string& msg );
		bool	svn( eSVNCom sc, const	fileset& filenames ,const	std_string& msg ); //ø’÷∏’ÎΩ·Œ≤
		bool	svn( eSVNCom sc,		fileset& filenames ,const	std_string& msg );
		
		const	char*	getSvnInfo(void);
		const	char*	getSvnUrl(void);
		const	char*	getWorlPath(void);
		const	char*	getSvnRoot(void);

				void	setUserName(const	std_string& user);
				void	setPassWord(const	std_string& pwd);
	private:
		CSvnControlImp*	m_pImp;
	};
}
