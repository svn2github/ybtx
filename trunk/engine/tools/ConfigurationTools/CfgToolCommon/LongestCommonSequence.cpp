#include "stdafx.h"
#include "LongestCommonSequence.h"

CLongestCommonSequence::CLongestCommonSequence( const wchar_t* szSeq1, const wchar_t* szSeq2 )
{
	Init(szSeq1, szSeq2);
}

CLongestCommonSequence::~CLongestCommonSequence(void)
{
	delete[] m_szSeq1;
	m_szSeq1 = NULL;
	delete[] m_szSeq2;
	m_szSeq2 = NULL;
	delete[] m_pnFlag;
	m_pnFlag = NULL;
	delete[] m_pnWildFlag;
	m_pnWildFlag = NULL;
}

void CLongestCommonSequence::Init( const wchar_t* szSeq1, const wchar_t* szSeq2 )
{
	m_nSeq1Len = wcslen(szSeq1);
	m_nSeq2Len = wcslen(szSeq2);

	m_szSeq1 = new wchar_t[m_nSeq1Len + 2];
	for (int i=1; i<=m_nSeq1Len; i++)
	{
		m_szSeq1[i] = szSeq1[i-1];
	}
	m_szSeq1[m_nSeq1Len + 1] = '\0';

	m_szSeq2 = new wchar_t[m_nSeq2Len + 2];
	for (int i=1; i<=m_nSeq2Len; i++)
	{
		m_szSeq2[i] = szSeq2[i-1];
	}
	m_szSeq2[m_nSeq2Len + 1] = '\0';

	m_pnFlag = new int[(m_nSeq1Len+1) * (m_nSeq2Len+1)];
	m_pnWildFlag = new int[m_nSeq1Len + 1];
	m_pnWildFlag[0] = 0;
	for (int i=1; i<m_nSeq1Len+1; i++)
	{
		m_pnWildFlag[i] = 1;
	}

	m_nCommSeqCurPos = 0;
}

void CLongestCommonSequence::LCSLength()
{
	int* pLength = new int[(m_nSeq1Len+1) * (m_nSeq2Len+1)];
	for (int i=0; i<=m_nSeq1Len ; i++)
	{
		pLength[i * (m_nSeq2Len + 1)] = 0;
	}
	for (int i=1; i<=m_nSeq2Len; i++)
	{
		pLength[i] = 0;
	}

	for (int i=1; i<=m_nSeq1Len; i++)
	{
		for (int j=1; j<=m_nSeq2Len; j++)
		{
			if (m_szSeq1[i] == m_szSeq2[j])
			{
				pLength[i*(m_nSeq2Len+1)+j] = pLength[(i-1)*(m_nSeq2Len+1)+j-1] + 1;
				m_pnFlag[i*(m_nSeq2Len+1)+j] = 0;
			}
			else if (pLength[(i-1)*(m_nSeq2Len+1)+j] >= pLength[i*(m_nSeq2Len+1)+j-1])
			{
				pLength[i*(m_nSeq2Len+1)+j] = pLength[(i-1)*(m_nSeq2Len+1)+j];
				m_pnFlag[i*(m_nSeq2Len+1)+j] = -1;
			}
			else
			{
				pLength[i*(m_nSeq2Len+1)+j] = pLength[i*(m_nSeq2Len+1)+j-1];
				m_pnFlag[i*(m_nSeq2Len+1)+j] = 1;
			}
		}
	}
	m_nCommSeqLen = pLength[(m_nSeq1Len+1) * (m_nSeq2Len+1) - 1];
	delete[] pLength;
	pLength = NULL;
}

void CLongestCommonSequence::LCS( int nSeq1CurLen, int nSeq2CurLen )
{
	if (nSeq1CurLen==0 || nSeq2CurLen==0)
	{
		return;
	}
	if (m_pnFlag[nSeq1CurLen*(m_nSeq2Len+1)+nSeq2CurLen] == 0)
	{
		LCS(nSeq1CurLen - 1, nSeq2CurLen - 1);
		m_wstrCommSeq += m_szSeq1[nSeq1CurLen];
	}
	else if (m_pnFlag[nSeq1CurLen*(m_nSeq2Len+1)+nSeq2CurLen] == -1)
	{
		LCS(nSeq1CurLen - 1, nSeq2CurLen);
	}
	else
	{
		LCS(nSeq1CurLen, nSeq2CurLen - 1);
	}
}

void CLongestCommonSequence::WildcardReslove( int nSeq1CurLen, int nSeq2CurLen )
{
	if (nSeq1CurLen==0 || nSeq2CurLen==0)
	{
		return;
	}
	if (m_pnFlag[nSeq1CurLen*(m_nSeq2Len+1)+nSeq2CurLen] == 0)
	{
		WildcardReslove(nSeq1CurLen - 1, nSeq2CurLen - 1);
		m_wstrCommSeq += m_szSeq1[nSeq1CurLen];
		m_pnWildFlag[nSeq1CurLen] = 0;
	}
	else if (m_pnFlag[nSeq1CurLen*(m_nSeq2Len+1)+nSeq2CurLen] == -1)
	{
		WildcardReslove(nSeq1CurLen - 1, nSeq2CurLen);
	}
	else
	{
		WildcardReslove(nSeq1CurLen, nSeq2CurLen - 1);
	}
}

void CLongestCommonSequence::CommSeq()
{
	LCSLength();
	LCS(m_nSeq1Len, m_nSeq2Len);
}

wstring CLongestCommonSequence::GetCommSeq()
{
	return m_wstrCommSeq;
}

void CLongestCommonSequence::Wildcard()
{
	LCSLength();
	WildcardReslove(m_nSeq1Len, m_nSeq2Len);
	for (int i=1; i<=m_nSeq1Len; i++)
	{
		if (m_pnWildFlag[i] == 0)
		{
			m_wstrWildSeq += m_szSeq1[i];
		}
		else
		{
			if (m_pnWildFlag[i - 1] == 0)
			{
				m_wstrWildSeq += L"%s";
			}
		}
	}
	for (int i=1; i<m_nSeq1Len; i++)
	{
		if (m_pnWildFlag[i] == 1)
		{
			m_wstrWildArg += m_szSeq1[i];
			if (m_pnWildFlag[i+1] == 0)
			{
				m_wstrWildArg += L',';
			}
		}
	}
	if (m_pnWildFlag[m_nSeq1Len] == 1)
	{
		m_wstrWildArg += m_szSeq1[m_nSeq1Len];
	}
}

wstring CLongestCommonSequence::GetWildSeq() const
{
	return m_wstrWildSeq;
}

wstring CLongestCommonSequence::GetWildArg() const
{
	return m_wstrWildArg;
}
