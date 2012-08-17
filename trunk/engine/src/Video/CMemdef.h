//------------------------------------------------------------------------------
// File: MemFile.h
//
// Desc: DirectShow sample code - header file for application using async 
//       filter.
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#pragma once
//
//  Define an internal filter that wraps the base CBaseReader stuff
//
namespace sqr
{
	class  CPkgFile;
}

const wstring PATH_ALIAS_AUD = L"res";

class  CMemStream : public CAsyncStream
{
public:
	CMemStream(const std::string& name, DWORD dwKBPerSec = INFINITE);
	 
	~CMemStream();

	HRESULT SetPointer(LONGLONG llPos);

	void AddBuf(DWORD dwBytesToRead);

	HRESULT Read(PBYTE pbBuffer,
		  DWORD dwBytesToRead,
		  BOOL bAlign,
		  LPDWORD pdwBytesRead);

	LONGLONG Size(LONGLONG *pSizeAvailable);

	DWORD Alignment();

	void Lock();

	void Unlock();

private:
	CCritSec       m_csLock;
	PBYTE		   m_pbData;
	LONGLONG	   m_llLength;
	LONGLONG       m_llPosition;
	DWORD          m_dwKBPerSec;
	DWORD          m_dwTimeStart;
	CPkgFile	   m_PkgFile;
};


class  CMemReader : public CAsyncReader
{
public:

	//  We're not going to be CoCreate'd so we don't need registration
	//  stuff etc
	STDMETHODIMP Register();

	STDMETHODIMP Unregister();

	CMemReader(CMemStream *pStream, CMediaType *pmt, HRESULT *phr);
};
