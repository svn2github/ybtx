#if !defined _I_RESULT_HOLDER_H
#define _I_RESULT_HOLDER_H

class IResultHolder
{
public:
	virtual void Write(const void* pData,unsigned int uSize)=0;
	virtual void Segment()=0;
};

#endif

