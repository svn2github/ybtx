#pragma once

class CLongestCommonSequence
{
public:
	CLongestCommonSequence( const wchar_t* szSeq1, const wchar_t* szSeq2 );
	~CLongestCommonSequence();

	void CommSeq();
	void Wildcard();
	wstring GetWildSeq() const;
	wstring GetWildArg() const;
	wstring GetCommSeq();

private:
	void Init( const wchar_t* szSeq1, const wchar_t* szSeq2 );
	void LCS( int nSeq1CurLen, int nSeq2CurLen );
	void WildcardReslove( int nSeq1CurLen, int nSeq2CurLen );
	void LCSLength();

private:
	wchar_t* m_szSeq1;
	wchar_t* m_szSeq2;
	wchar_t* m_szCommSeq;
	int* m_pnFlag;
	int* m_pnWildFlag;
	int m_nSeq1Len;
	int m_nSeq2Len;
	int m_nCommSeqCurPos;
	wstring m_wstrCommSeq;
	wstring m_wstrWildSeq;
	wstring m_wstrWildArg;
	int m_nCommSeqLen;
};
