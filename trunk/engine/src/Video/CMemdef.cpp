//------------------------------------------------------------------------------
// File: MemFile.h
//
// Desc: DirectShow sample code - header file for application using async 
//       filter.
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "CGraphic.h"
#include "CWinPkgFile.h"
#include <streams.h>
#include "Casyncio.h"
#include "Casyncrdr.h"
#include "CMemdef.h"

CMemStream::CMemStream(const std::string& name, DWORD dwKBPerSec) :
  m_llPosition(0),
  m_dwKBPerSec(dwKBPerSec)
{
	m_dwTimeStart = timeGetTime();
	m_PkgFile.Open(PATH_ALIAS_AUD.c_str(), name.c_str());
	m_llLength = m_PkgFile.Size();
	m_pbData = new BYTE[1024*1024*1];
}

CMemStream::~CMemStream()
{
	if (m_pbData)
	{
		delete m_pbData;
		m_pbData = NULL;
	}

	m_PkgFile.Close();
}

HRESULT CMemStream::SetPointer(LONGLONG llPos)
{
	if (llPos < 0 || llPos > m_llLength) {
	  return S_FALSE;
	} else {
	  m_llPosition = llPos;
	  return S_OK;
	}
}

void CMemStream::AddBuf(DWORD dwBytesToRead)
{
	m_PkgFile.Seek((LONG)m_llPosition, FILE_BEGIN);
	m_PkgFile.Read((LPVOID)m_pbData,dwBytesToRead);
}

HRESULT CMemStream::Read( PBYTE pbBuffer,
						  DWORD dwBytesToRead,
						  BOOL bAlign,
						  LPDWORD pdwBytesRead)
{
	if (!m_pbData)
		return S_FALSE;

	CAutoLock lck(&m_csLock);
	DWORD dwReadLength;

	/*  Wait until the bytes are here! */
	DWORD dwTime = timeGetTime();
	if (m_llPosition + dwBytesToRead > m_llLength) {
	  dwReadLength = (DWORD)(m_llLength - m_llPosition);
	} else {
	  dwReadLength = dwBytesToRead;
	}

	DWORD dwTimeToArrive =
	  ((DWORD)m_llPosition + dwReadLength) / m_dwKBPerSec;
	if (dwTime - m_dwTimeStart < dwTimeToArrive) {
	  Sleep(dwTimeToArrive - dwTime + m_dwTimeStart);
	}

	AddBuf(dwReadLength);

	CopyMemory((PVOID)pbBuffer, (PVOID)m_pbData,
	  dwReadLength);
	m_llPosition += dwReadLength;
	*pdwBytesRead = dwReadLength;
	return S_OK;
}

LONGLONG CMemStream::Size(LONGLONG *pSizeAvailable)
{
	LONGLONG llCurrentAvailable =
	  Int32x32To64((timeGetTime() - m_dwTimeStart),m_dwKBPerSec);

	*pSizeAvailable = min(m_llLength, llCurrentAvailable);
	return m_llLength;
}

DWORD CMemStream::Alignment()
{
	return 1;
}
void CMemStream::Lock()
{
	m_csLock.Lock();
}
void CMemStream::Unlock()
{
	m_csLock.Unlock();
}



//  We're not going to be CoCreate'd so we don't need registration
//  stuff etc
STDMETHODIMP CMemReader::Register()
{
	return S_OK;
}

STDMETHODIMP CMemReader::Unregister()
{
	return S_OK;
}

CMemReader::CMemReader(CMemStream *pStream, CMediaType *pmt, HRESULT *phr) :
CAsyncReader(NAME("Mem Reader\0"), NULL, pStream, phr)
{
	m_mt = *pmt;
}
