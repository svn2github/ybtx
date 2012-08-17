
#include "CPkgFile.h"


namespace sqr
{

	enum EFileOPType
	{
		eFOP_Read,
		eFOP_Write,
		eFOP_Append
	};

#ifdef _WIN32
	class CWinPkgFile
	{
	public:
		CWinPkgFile();
		~CWinPkgFile();

		HANDLE Open(const wchar_t* alias, const char *szFileName);
		HANDLE Open(const wchar_t* alias, const wchar_t *szFileName); // EFileOPType eFlag
		unsigned int Read(char* szBuf, unsigned int uSize);
		unsigned int Write(char* szBuf, unsigned int uSize);
		unsigned int Seek(long nSize, unsigned int uFlag);
		int Flush();
		int Close();

	private:
		HANDLE m_hFile;
	};
#endif

}
