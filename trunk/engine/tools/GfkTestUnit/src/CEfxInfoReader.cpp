#include "stdafx.h"
#include "CEfxInfoReader.h"
#include "CPkgFile.h"
#include "CEffectGroup.h"
#include "CBufFile.h"
#include "CEffectManager.h"

CEfxInfoReader::CEfxInfoReader(void)
{

}

CEfxInfoReader::~CEfxInfoReader(void)
{

}

uint32	CEfxInfoReader::GetCount(void)
{
	return (uint32)m_efxPool.size();
}

string	CEfxInfoReader::GetEfxName(uint32 idx)
{
	return m_efxPool[idx];
}

void	CEfxInfoReader::Load(const string& filename)
{
	CPkgFile epkgFile;
	epkgFile.Open(  L"res",filename.c_str() );
	char* pBuffer = new char[epkgFile.Size()];
	epkgFile.Read(pBuffer,epkgFile.Size());

	CBufFile bufFile(pBuffer,epkgFile.Size(),0);
	CEffectFile efxFile(bufFile);
	
	class CInfoEfxMgr : public CEffectGroup
	{
	public:
		CInfoEfxMgr( IEffectManager* pEffectMgr, const char* szFileName )
			: CEffectGroup( pEffectMgr,szFileName ){}
	};

	CInfoEfxMgr tmpEfx(IEffectManager::GetInst(),filename.c_str());
	tmpEfx.BinLoad(efxFile);

	m_efxPool.resize(tmpEfx.GetEffectNum());
	for(uint32 i = 0; i<tmpEfx.GetEffectNum(); ++i)
	{
		m_efxPool[i] = tmpEfx.GetEffectName(i);	
	}
	SafeDelAry(pBuffer);
}
