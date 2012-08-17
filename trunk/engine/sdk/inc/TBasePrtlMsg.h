#pragma once


namespace sqr
{

#pragma pack(push,1)

template<typename IdType>
class TBasePrtlMsgHeader
{
private:
	typedef TBasePrtlMsgHeader<IdType>	Self_t;
	IdType	m_uId;
	uint8	m_uCheckSum;
protected:
	TBasePrtlMsgHeader(IdType Id)
			:m_uId(Id),m_uCheckSum(~uint8(Id))
	{
	}
public:
	bool ChecksumCorrect()const
	{
		uint8 nCheckSum = ~m_uCheckSum;
		return uint8(m_uId)==uint8(nCheckSum);
	}
};


template<typename ImpClass,typename IdType,IdType uId>
class TBasePrtlMsg
			:public TBasePrtlMsgHeader<IdType>
{
private:
	typedef TBasePrtlMsgHeader<IdType>	ParentHeader_t;
	uint32 GetPayloadSizeDerived()const
	{
		return 0;
	}
public:
	TBasePrtlMsg()
			:ParentHeader_t(uId)
	{
	}
	static uint32 GetHeaderSize()
	{
		return sizeof(ImpClass);
	}
	static IdType GetId()
	{
		return uId;
	}
	uint32 GetPayloadSize()const
	{
		return static_cast<const ImpClass*>(this)->GetPayloadSizeDerived();
	}
};

#pragma pack(pop)

}

