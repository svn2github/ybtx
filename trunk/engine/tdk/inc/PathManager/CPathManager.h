#pragma once
#include "CCtrlBase.h"
#include <string>
#include <vector>
#include "TSingleton.h"

namespace sqr_tools
{
	class CPathSetImp :public Singleton<CPathSetImp>,public CCtrlBase
	{
		REG_UI_IMP(CPathSetImp);
	public:
		CPathSetImp(void);
		~CPathSetImp(void);

	private:
		typedef std::vector<std::string> PathList;
		PathList			m_ConfigPath;
		static std::string	st_szCtrlType;
	public:
		virtual bool GetResFileName(std::string& d_str,std::string& r_str);
		virtual bool GetFullFileName(std::string filename,std::string& value);
	
	public:
		virtual bool SaveConfigPath(void);
		virtual bool HasSetConfigPath(void);
		virtual void			Clear(void);
		virtual unsigned int	Count(void);
		virtual std::string		GetPath(unsigned int index);
		virtual void			AddPath(const std::string& work_dir);
	};
}
