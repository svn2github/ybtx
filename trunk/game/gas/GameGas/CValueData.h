#pragma once
#include "CFighterMallocObject.h"

class CValueData
	:public CFighterMallocObject
{
public:
	CValueData()										{}
	CValueData(int32 iVal) : m_iVal(iVal)				{}
	CValueData(float fVal) : m_fVal(fVal)				{}
	CValueData(uint32 uVal) : m_uVal(uVal)				{}
	const CValueData&	operator= (int32 iVal)			
	{
		m_iVal = iVal;
		return *this;
	}
	const CValueData&	operator= (float fVal)			
	{
		m_fVal = fVal;
		return *this;
	}
	const CValueData&	operator= (uint32 uVal)			
	{
		m_uVal = uVal;
		return *this;
	}
	const int32			GetInt32() const				{return m_iVal;}
	const float			GetFloat() const				{return m_fVal;}
	const uint32		GetUInt32() const				{return m_uVal;}
	void Get(int32& Out)
	{
		Out=m_iVal;
	}
	void Get(float& fOut)
	{
		fOut=m_fVal;
	}
	void Get(uint32& uOut)
	{
		uOut=m_uVal;
	}
	
	template <typename ValueType>
	void SetBy(ValueType Val);

	template <typename ValueType>
	const ValueType		GetBy() const;
private:
	union
	{
		int32	m_iVal;
		float	m_fVal;
		uint32	m_uVal;
	};

};







