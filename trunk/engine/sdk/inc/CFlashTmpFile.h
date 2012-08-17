#pragma once

namespace sqr
{
	class CPkgFile;
	class CFlashTmpFile
	{
	public:
		CFlashTmpFile(wstring fname);
		~CFlashTmpFile(void);

	private:
		wstring		m_FileName;
		wstring		m_wTmpFileName;
		wstring		m_tmpFolderName;
		CPkgFile	m_PkgFile;
		BOOL		m_IsCreateFileFolder;
	public:
		bool CreateTmpFlash();
		void Clear();
		wstring		GetTmpFileName() const;
	};

}