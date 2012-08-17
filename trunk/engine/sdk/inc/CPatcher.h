

namespace sqr
{
#define  PROCESSOR_NUMBER  2
#define  UPGRADE_PATCH     1
#define  DEGRADE_PATCH     2


class IDiff;
class IAsyncUpdateHandler;


class CPatcher
{
public:
	CPatcher();
	~CPatcher();

	void CheckHeadInfo(sqr::IAsyncUpdateHandler* handler, const wchar_t* szPatchFile);

private:
	char*  m_pContent;
	IDiff* m_szDiff[PROCESSOR_NUMBER];
};

}
