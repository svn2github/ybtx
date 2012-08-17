#include "stdafx.h"
#include "CRowTableFileImp.h"
#include "CRowTableFile.h"
#include "ExpHelper.h"
#include "TBaseTableFile.inl"


//--------------------------------------------------------------

template class TBaseTableFile<CRowTableFileImp>;

CRowTableFile::CRowTableFile() 
{

}

CRowTableFile::~CRowTableFile()
{
	
}

void CRowTableFile::Release()
{
	delete this;
}

CRowTableFileNode* CRowTableFile::GetRowTableNode()
{
	Ast(m_pImp);
	return m_pImp->m_pRowTableRootNode;
}

