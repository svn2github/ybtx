#pragma once
#include "IDatabase.h"

namespace sqr
{
	class CDbStatement;

	class CDbResultBinder
		:public IDbResultBinder
	{
	public:
		CDbResultBinder(const CDbStatement* pStmt);
		~CDbResultBinder(void);

		void SetResultInt8(int8* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex);
		void SetResultUint8(uint8* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex);
		void SetResultInt16(int16* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex);
		void SetResultUint16(uint16* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex);
		void SetResultInt32(int32* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex);
		void SetResultUint32(uint32* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex);
		void SetResultInt64(int64* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex);
		void SetResultUint64(uint64* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex);
		void SetResultFloat(float* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex);
		void SetResultDouble(double* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex);
		void SetResultText(void* pBuffer,size_t uBufferSize
			,size_t* uDataSize,bool* pIsNull,bool* pError,size_t uColIndex);
		void SetResultBinary(void* pBuffer,size_t uBufferSize
			,size_t* uDataSize,bool* pIsNull,bool* pError,size_t uColIndex);

		IDbStatement* GetStatement()const;

		void Bind();

		void Release();
		
		void Reset();

	private:
		const CDbStatement*	m_pStmt;
		MYSQL_BIND*				m_aryBind;

		void SetResultIntegerSeries(void* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex,
			enum_field_types eBufferType,bool bIsUnsigned);
		void SetResultRealSeries(void* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex,
			enum_field_types eBufferType);
		void SetResultBufferSeries(void* pBuffer,size_t uBufferSize,size_t* uDataSize,bool* pIsNull,bool* pError,size_t uColIndex,
			enum_field_types eBufferType);

		void ConfirmResultIndexValid( size_t stIndex );
	};


}
