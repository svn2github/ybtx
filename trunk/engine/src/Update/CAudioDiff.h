
#include "IDiff.h"
#include "PatchCommonDef.h"

namespace sqr
{

struct FileHead;

class CAudioDiff
	: public IDiff
{
public:
	CAudioDiff();
	~CAudioDiff();

	virtual bool ExecuteModify(FileHead* pPatchHead);

	void LoadHead(const char* szFileName);
	void LoadData(const char* szFileName);
	void LoadStruct(const char* szFileName);

	void LoadPatchInfo(FileHead* pPatchHead);
	void StartPatch(FileHead* pPatchHead);

	unsigned long GetCorrespond(char* szFileName);

private:
	sFileData*      m_SrcWaveData;
	WAVEBANKHEADER  m_SrcHead;
	WAVEBANKDATA    m_SrcBankData;
	WAVEBANKENTRY*  m_SrcMetaDataList;
	char*           m_SrcNameBuffer;
	uint8*          m_FileData;

	WAVEBANKHEADER  m_DstHead;
	WAVEBANKDATA    m_DstBankData;
	WAVEBANKENTRY*  m_DstMetaDataList;

	unsigned int m_uFileSize; // origin file
};

}
