#include "stdafx.h"
#include "CEditSkeletal.h"
#include "BaseHelper.h"
#include <fstream>
#include "CDataSources.h"

const CEditSkeletalUnit& CEditSkeletalUnit::operator = ( const CEditSkeletalUnit& fEsu )
{
	if( this != &fEsu )
	{
		memcpy(this,&fEsu,sizeof(CEditSkeletalUnit));
	}
	return *this;
}

int CEditSkeletalUnit::BinSave( ofstream& File )
{
	int i;
	CMatrix tMatrix;
	m_InverseMatrix.GetMATRIX(tMatrix);
	File.write( (TCHAR*)&tMatrix, sizeof( CMatrix ) );

	int Size = (int)m_SocketList.size();
	File.write( (TCHAR*)&Size, sizeof( Size ) );
	for( i = 0; i < (int)m_SocketList.size(); i++ )
	{
		File.write( m_SocketList[i].m_SocketName, sizeof( sqr::IDNAME ) );
		File.write( (TCHAR*)&m_SocketList[i].m_Socket, sizeof( CMatrix ) );
	}

	return 0;
}
const	CEditSkeletal& CEditSkeletal::operator = ( const CEditSkeletal& fES )
{
	if( this != &fES )
	{
		m_ParentSkeletalID	= fES.m_ParentSkeletalID;		//¸¸¹Ç÷ÀID
		m_SkeletalID		= fES.m_SkeletalID;           //¹ÇÍ·ID
		m_SkeletalName		= fES.m_SkeletalName;			//¹ÇÍ·Ãû×Ö
		GVector<CSkeletal*>::iterator it,eit = m_Children.end();
		for( it = m_Children.begin(); it!=eit; ++it )
			SafeDelete(*it);
		m_Children.resize( fES.m_Children.size(),NULL );

		for( size_t i=0;i<m_Children.size(); ++i )
		{
			m_Children[i] = CDataSources::GetInst()->NewSkeletal();
			*(CEditSkeletal*)m_Children[i] = *(CEditSkeletal*)fES.m_Children[i];
		}
	}
	return *this;
}

void	CEditSkeletal::RefreshSkeletalUnit(GVector<CSkeletalUnit*>& VecUnit)
{
	VecUnit[m_SkeletalID]->m_pSkeletal = this;
	for( size_t i=0;i<m_Children.size(); ++i )
		((CEditSkeletal*)m_Children[i])->RefreshSkeletalUnit(VecUnit);
}

bool CEditSkeletal::IsFitForMe( const CEditSkeletal* Skeletal, string& ErrSkeletal )
{
	if( m_Children.size() == Skeletal->m_Children.size() )
	{
		for( int i = 0; i< (int)m_Children.size(); i++ )
		{
			if( !((CEditSkeletal*)m_Children[i])->IsFitForMe( (CEditSkeletal*)Skeletal->m_Children[i], ErrSkeletal ) )
				return FALSE;
		}
		return TRUE;
	}
	string Src = "Ô­À´¹Ç÷À",Dest = "ÐÂ¼Ó¹Ç÷À";
	Src += m_SkeletalName.c_str();
	Src += "×Ó¹Ç÷À:";
	Dest+= Skeletal->m_SkeletalName.c_str();
	Dest+= "×Ó¹Ç÷À:";
	for( int i = 0; i < (int)m_Children.size(); i++ )
	{
		Src += ((CEditSkeletal*)m_Children[i])->m_SkeletalName.c_str();
		Src += ",";
	}
	for( int i = 0; i < (int)Skeletal->m_Children.size(); i++ )
	{
		Dest += ((CEditSkeletal*)(Skeletal->m_Children[i]))->m_SkeletalName.c_str();
		Dest += ",";
	}
	ErrSkeletal = Src + "<==>" + Dest;
	return FALSE;
}

int CEditSkeletal::BinSave( ofstream& File )
{
	int Size = (int)m_SkeletalName.size();
	File.write( (TCHAR*)&Size, sizeof(Size) );
	File.write( (TCHAR*)m_SkeletalName.data(), Size );
	File.write( (TCHAR*)&m_SkeletalID, sizeof(m_SkeletalID) );

	Size = (int)m_Children.size();
	File.write( (TCHAR*)&Size, sizeof(Size) );
	for( int i = 0; i < (int)m_Children.size(); i++ )
		((CEditSkeletal*)m_Children[i])->BinSave( File );

	return 0;
}