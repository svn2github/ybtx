#include "stdafx.h"
#include "Package.h"
#include "FixOverflowPkg.h"
void ProcessPackageFile(const string &full_path)
{
	CFixOverflowPkg pkg(full_path);
	pkg.DoFix();
}

void ProcessAllPackage( const string &path )
{
	const string CURRENT_PATH = ".";
	const string PARENT_PATH  = "..";

	WIN32_FIND_DATA	fd;
	HANDLE			hFind;
	string			full_path;

	full_path = path + "\\*.*"; 
	hFind	  = FindFirstFile(full_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		BOOL bHasNext = true;
		while ( bHasNext )
		{
			if ( fd.cFileName != CURRENT_PATH && fd.cFileName != PARENT_PATH )
			{
				if (fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
				{
					if (fd.cFileName[0] != '.')
					{
						ProcessAllPackage(path + "\\" + fd.cFileName);
					}
				}
				else
				{
					full_path = path + "\\" + fd.cFileName;
					if (full_path.find(".pkg") != string::npos)
					{
						ProcessPackageFile(full_path);
					}
				}
			}

			bHasNext = FindNextFile(hFind, &fd);
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	if ( argc != 2 )
	{
		cout << "Usage: FixPackage.exe package_files_path\n\n";
		system("pause");
		return 1;
	}

	ProcessAllPackage(argv[1]);
	
	cout << "Process finished.\n";

	system("pause");
	return 0;
}

