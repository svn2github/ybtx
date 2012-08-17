
#include "IDiff.h"




namespace sqr
{
struct FileHead;


class CBaseDiff
	: public IDiff
{
public:
	CBaseDiff();
	~CBaseDiff();

	virtual bool ExecuteModify(FileHead* pPatchHead);

private:
};

}

