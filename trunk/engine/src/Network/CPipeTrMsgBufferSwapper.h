#include "CTrMsgBufferSwapper.h"


class CPipeTrMsgBufferSwapper : public CTrMsgBufferSwapper
{
public:
	CPipeTrMsgBufferSwapper();
	~CPipeTrMsgBufferSwapper();

	static CPipeTrMsgBufferSwapper*& Inst();

	void OnWakeLeft();
};



