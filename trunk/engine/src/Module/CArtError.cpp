#include "stdafx.h"
#include "CArtError.h"
#include "CError.h"
#include "ExpHelper.h"
#include "ErrLogHelper.h"

namespace sqr
{
	class CArtErrorData : public CMallocObject
	{
	public:
		typedef basic_string<char,std::char_traits<char>,TMallocAllocator<char> > ArtErr_t;
		typedef set<ArtErr_t, less<ArtErr_t>, 
			TMallocAllocator<ArtErr_t> >	SetError_t;

		void ArtError(const string& sErrorType);
		void ArtError(const string& sErrorType, const string& sErrorMsg);

		SetError_t m_setError;
	};

}

void CArtErrorData::ArtError(const string& sErrorType)
{
	bool bSuc = m_setError.insert(sErrorType.c_str()).second;
	if(bSuc)
	{
		CError error(sErrorType, "");
		error.SetCategory(ART_ERR_MSG);
		LogExp(error, NULL);
	}
	else
	{
		return;		
	}
}

void CArtErrorData::ArtError(const string& sErrorType, const string& sErrorMsg)
{
	string str(sErrorType);
	str.append(sErrorMsg);

	bool bSuc = m_setError.insert(str.c_str()).second;
	if(bSuc)
	{
		CError error(sErrorType, "");
		error.AppendMsg(sErrorMsg);
		error.SetCategory(ART_ERR_MSG);
		LogExp(error, NULL);
	}
	else
	{
		return;		
	}
}

CArtError::CArtError()
{
	m_pData = new CArtErrorData;
}

CArtError::~CArtError()
{
	delete m_pData;
}

CArtError& CArtError::Inst()
{
	static CArtError ls_Inst;
	return ls_Inst;
}

void CArtError::ArtError(const string& sErrorType)
{
	m_pData->ArtError(sErrorType);
}

void CArtError::ArtError(const string& sErrorType, const string& sErrorMsg)
{
	m_pData->ArtError(sErrorType, sErrorMsg);
}

