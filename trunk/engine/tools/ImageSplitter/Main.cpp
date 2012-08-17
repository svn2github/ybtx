#include "stdafx.h"
#include "Win32.h"
#include "DirectX9.h"
#include "ConfigFileParser.h"
#include "ImageSplitter.h"
#include "DefaultIndexGenerator.h"

// ---- 实现部分 ------------------------------------------------------------------------------

	bool	ProcessFile(CConfigFileParser::Node &node)
	{
		// 解析区域信息文件
		string szRgnInfo = node.szSrcPath;
		uint32 uWidth	 = 0;
		uint32 uHeight	 = 0;

		locale loc = locale::global(locale(""));
		ifstream file(szRgnInfo.c_str());
		locale::global(loc);

		if ( !file.good() )
		{
			cout << "区域信息文件 " << szRgnInfo << " 不存在\n";
			return false;
		}

		// 读取区域信息(这里读取的值要除以4)
		file.read((char*)&uWidth, sizeof(uint32));
		file.read((char*)&uHeight, sizeof(uint32));
		file.close();
		if ( uWidth<=0 || uHeight<=0 )
			return false;
		uWidth	>>= 2;
		uHeight >>= 2;

		// 检测原图片文件是否存在
		string szOldPicPath = "./source/" + node.szPicName;
		loc = locale::global(locale(""));
		file.open(szOldPicPath.c_str());
		locale::global(loc);
		if ( !file.good() )
		{
			cout << "原图片 " << node.szPicName << " 不存在\n";
			return false;
		}
		
		CImageSplitter	cutter(new CDefaultIndexGenerator);
		cutter.SetRgnInfo(uWidth, uHeight);
		cutter.SetSourceFile(szOldPicPath);
		cutter.SetTargetPath(node.szTgtPath);
		cutter.Run();
		
		return true;
	}

	bool	ProcessAllFiles()
	{
		CConfigFileParser::Node node;
		while ( CConfigFileParser::GetInst()->GetNextNode(node) )
		{
			if ( !ProcessFile(node) )
			{
				return false;
			}
		}

		return true;
	}

	int _tmain(int argc, _TCHAR* argv[])
	{
		if ( !CreateMainWindow(0, 0, 256, 256) )
		{
			return ERR_CREATE_WINDOW_FAILED;
		}

		if ( !InitGraphicsDeviceDX9() )
		{
			return ERR_INIT_DIRECT3D9_FAILED;
		}

		bool bRet = false;
		while ( true )
		{
			CConfigFileParser::GetInst()->Reset();
			bRet = CConfigFileParser::GetInst()->SetConfigFilePath("./Config.txt");
			if ( !bRet )
			{
				cout << "是否继续?(Y/N)" << endl;
				string szInput;
				cin >> szInput;
				if ( szInput == "Y" || szInput == "y" )
				{
					continue;
				}
				else
				{
					break;
				}
			}

			bRet = ProcessAllFiles();
			if ( !bRet )
			{
				cout << "是否继续?(Y/N)" << endl;
				string szInput;
				cin >> szInput;
				if ( szInput == "Y" || szInput == "y" )
				{
					continue;
				}
				else
				{
					break;
				}
			}

			cout << "转换成功" << endl;
			system("pause");
			return 0;
		}
		
		return ERR_PROCESS_FAILED;
	}

