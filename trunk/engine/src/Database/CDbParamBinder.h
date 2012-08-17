#pragma once
#include "IDatabase.h"

namespace sqr
{
	class CDbStatement;
	class CDbBind;

	class CDbParamBinder
		:public IDbParamBinder
	{
	public:
		CDbParamBinder(const CDbStatement* pStmt);
		~CDbParamBinder(void);

		void SetParamNull(const bool* pBuffer,size_t uIndex);
		void SetParamInt8(const int8* pBuffer,size_t uIndex);
		void SetParamUint8(const uint8* pBuffer,size_t uIndex);
		void SetParamInt16(const int16* pBuffer,size_t uIndex);
		void SetParamUint16(const uint16* pBuffer,size_t uIndex);
		void SetParamInt32(const int32* pBuffer,size_t uIndex);
		void SetParamUint32(const uint32* pBuffer,size_t uIndex);
		void SetParamInt64(const int64* pBuffer,size_t uIndex);
		void SetParamUint64(const uint64* pBuffer,size_t uIndex);
		void SetParamFloat(const float* pBuffer,size_t uIndex);
		void SetParamDouble(const double* pBuffer,size_t uIndex);
		void SetParamText(const void* pBuffer,size_t uMaxSize,size_t* pActualSize,size_t uIndex);
		void SetParamBinary(const void* pBuffer,size_t uMaxSize,size_t* pActualSize,size_t uIndex);

		IDbStatement* GetStatement()const;

		void Bind();

		void Release();

		const CDbStatement*	m_pStmt;
	private:
		CDbBind*		m_aryBind;


		void SetParamIntegerSeries(const void* pBuffer,size_t uIndex,enum_field_types eBufferType,bool bIsUnsigned);
		void SetParamRealSeries(const void* pBuffer,size_t uIndex,enum_field_types eBufferType);
		//void SetParamBufferSeries(const void* pBuffer,size_t uBufferSize,bool* pIsNull,size_t uIndex,enum_field_types eBufferType);
		void SetParamBufferSeries(const void* pBuffer,size_t uMaxSize,size_t* pActualSize,size_t uIndex,enum_field_types eBufferType);

		void ConfirmParamIndexValid(size_t uIndex);
	};


}
