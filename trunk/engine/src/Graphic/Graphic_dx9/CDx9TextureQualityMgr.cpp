#include "stdafx.h"
#include "CDx9TextureQualityMgr.h"
#include "TNedAllocator.inl"


namespace sqr
{
	class CQualityProcess : public CSyncProcess
	{
	public:
		CQualityProcess(CSyncProcess *pProcess,float fProBegin,float fProEnd);
		void Clear(void);
		void DoProcess( float process );
	protected:
		CSyncProcess*	m_pProcess;
		float			m_fProBegin;
		float			m_fProEnd;
	};

	CQualityProcess::CQualityProcess(CSyncProcess *pProcess,float fProBegin,float fProEnd)
		: m_pProcess(pProcess)
		, m_fProBegin(fProBegin)
		, m_fProEnd(fProEnd)
	{
		Ast(m_pProcess&&"CQualityProcess::CQualityProcess() : pProcess²»ÄÜÎª¿Õ");
		Ast(m_fProEnd>m_fProBegin && m_fProBegin>=0.0f && m_fProEnd<=1.0f );
	}

	void CQualityProcess::Clear(void)
	{
	}

	void CQualityProcess::DoProcess( float process )
	{
		float Delta =m_fProEnd - m_fProBegin;
		m_pProcess->DoProcess(m_fProBegin + Delta * process);
	}

	CDx9TextureQualityMgr::CDx9TextureQualityMgr()
		: m_uQualityLevel(0)
		, m_sInCtrlTexSize(0)
		, m_sOutCtrlTexSize(0)
	{

	}

	CDx9TextureQualityMgr::~CDx9TextureQualityMgr()
	{
		m_setTextures.clear();
	}

	bool	CDx9TextureQualityMgr::_AddTexture( CDx9Texture* pText )
	{
		QualityTextureSet::iterator it = m_setTextures.find(pText);
		if(it==m_setTextures.end())
			m_setTextures.insert(pText);
		return true;
	}

	bool	CDx9TextureQualityMgr::_DelTexture( CDx9Texture* pText )
	{
		QualityTextureSet::iterator it = m_setTextures.find(pText);
		if(it!=m_setTextures.end())
		{
			m_setTextures.erase(it);
			return true;
		}
		return false;
	}

	void	CDx9TextureQualityMgr::Update(CSyncProcess* pProcess)
	{
		if(pProcess!=NULL)
		{
			pProcess->Clear();
			CQualityProcess* pQualtyPro = NULL;
			pQualtyPro = new CQualityProcess(pProcess,0.0f,0.1f);
			CSyncSystem::BeginSyncFun( pQualtyPro );
			delete pQualtyPro;
			pQualtyPro = NULL;
		}
		else
			CSyncSystem::BeginSyncFun();
		
		bool isAsync = CPkgFile::IsEnableAsyncRead();
		CPkgFile::EnableAsyncRead(false);
		QualityTextureSet::iterator it,eit = m_setTextures.end();
		float	size = (float)m_setTextures.size();
		float   index = 0.0f;
		for( it = m_setTextures.begin(); it!=eit; ++it, index += 1.0f )
		{
			if( -1 == (*it)->ReLoadPic() )
			{
				string strErr = (*it)->GetName();
				strErr += ": ReLoadPic Failed!\n";
	
				GfkLogErr(GraphicErr::GetErrTypeStr(TextureCreate_Err),strErr);
			}

			if(pProcess)
				pProcess->DoProcess( 0.1f + 0.4f * index / size);
		}

  		if(pProcess!=NULL)
  		{
 			CQualityProcess* pQualtyPro = NULL;
 			pQualtyPro = new CQualityProcess(pProcess,0.5f,1.0f);
  			CSyncSystem::BeginSyncFun( pQualtyPro );
  			delete pQualtyPro;
 			pQualtyPro = NULL;
  		}
  		else
  			CSyncSystem::BeginSyncFun();
		
		CPkgFile::EnableAsyncRead(isAsync);
		if(pProcess)
			pProcess->DoProcess( 1.0f );
	}

	void	CDx9TextureQualityMgr::_ChangeTextureSize(int32 Size, CDx9Texture* pText)
	{
		QualityTextureSet::iterator it = m_setTextures.find(pText);
		if(it==m_setTextures.end())
			m_sOutCtrlTexSize += Size;
		else
			m_sInCtrlTexSize  += Size;
	}

}
