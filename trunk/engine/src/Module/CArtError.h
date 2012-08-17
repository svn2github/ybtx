#pragma once
#include "CMallocObject.h"

namespace sqr
{
	//对于资源错误的情况，我们只记录一次
	
	class CArtErrorData;

	class CArtError : public CMallocObject
	{
	public:
		static CArtError& Inst();

		void ArtError(const std::string& sErrorType);
		void ArtError(const std::string& sErrorType, const std::string& sErrorMsg);

	private:
		CArtError();
		~CArtError();
	
		CArtErrorData*	m_pData;
	};

}

