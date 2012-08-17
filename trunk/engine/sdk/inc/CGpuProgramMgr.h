#pragma once
#include "CShaderConstParams.h"
#include "CGpuProgram.h"
#include "TSingleton.h"
class TiXmlAttribute;
class TiXmlNode;
namespace sqr
{
#define NONE_GPU_HANDLE = 0;

	class CGpuProgramMgr 
		: public Singleton<CGpuProgramMgr>
		, public CGraphicMallocObject
	{
	public:
		CGpuProgramMgr();
		virtual ~CGpuProgramMgr();
	public:
		CGpuProgram* create(const string& str,DWORD handle,GpuProgramType gpt,const string& fun = "main");
		CGpuProgram* create(const char* buf,size_t fsize,DWORD handle,GpuProgramType gpt,const string& fun = "main");

		void destroy( DWORD handle);
		CGpuProgram* bind( const SHADER_HANDLE& handle );
		void unbind(void);
		CGpuProgram* getGpuProgram(DWORD handle);

		void lockGpuProgram(bool isLock);
		bool load(const string& path,const string& file, ShaderVer sVer );

		bool LoadGroupByXml(TiXmlNode *pDoc);
		bool loadShaderByXML(TiXmlAttribute* XmlAtt,DWORD GroupId,DWORD Mask );

		bool loadIncludeByXML(TiXmlAttribute* XmlAtt);
		bool loadMacroByXML(TiXmlAttribute* XmlAtt);

		bool isLoaded() const;
		const char*	GetShaderPath()
		{
			return m_ShaderPath.c_str();
		}
		ShaderVer GetShaderVer(void);
	protected:
		void	_bind(DWORD SysID);
		typedef GMap<DWORD,CGpuProgram*> GpuProList;
		GpuProList				m_ProgramList;

		CGpuProgram*			m_ActivePro;
		CGpuProgram*			m_SysPro;
		bool					m_isLock;
		bool					m_isLoaded;
		GString					m_ShaderPath;
		ShaderMacroMgr*			m_pMacroMgr;
		ShaderInclude*			m_pShaderInc;
		ShaderVer				m_eShaderVer;
		static const GString	st_ShaderVerPath[ShaderVerCount];
	protected:
		virtual CGpuProgram* _create_vertex_pro(DWORD handle,const string& fun) = 0;
		virtual CGpuProgram* _create_fragment_pro(DWORD handle,const string& fun) = 0;
		virtual CGpuProgram* _create_effects_pro(DWORD handle,const string& fun,const string& params = "") = 0;
	};

	//------------------------------------------------------------------------------
	inline bool CGpuProgramMgr::isLoaded() const
	{
		return this->m_isLoaded;
	}

	inline ShaderVer CGpuProgramMgr::GetShaderVer(void)
	{
		return m_eShaderVer;
	}
}