#include "stdafx.h"
#include <sstream>
#include "global.h"
#include <sys/stat.h>

#define INI_FILE_NAME  L"../../etc/gac/Update.ini"


NAMESPACE_BEGIN(sqr)

void CreatePatchList(PatchList& pl, const string& buf)
{
	int ret = 0;
	// 读取本机的版本
	FILE *fp = _wfopen(INI_FILE_NAME, L"rb");
	fseek(fp, 0, SEEK_END);
	int iFileLen = ftell(fp);
	string sCusContent;
	sCusContent.resize(iFileLen);
	fseek(fp, 0, SEEK_SET);
	fread(&sCusContent[0], iFileLen, 1, fp);
	fclose(fp);

	int OldHi, OldLo, OldBuild;
	string sVerion(sCusContent.substr(sCusContent.find_first_of("=")+1, iFileLen));
	ret = sscanf(sVerion.c_str(), "%d.%d.%d", &OldHi, &OldLo, &OldBuild);
	//if (ret != 3)
	//	::MessageBox(NULL, "本地版本号格式不正确，请检查Update.ini配置文件", "错误", MB_ICONSTOP|MB_OK);

	// 选出存在的比本机版本新的版本种子
	string sContent(buf);
	string sTorrentUrl;
	string sVer;
	int hi, lo, build;
	int start = 0;
	int end = 0;
	int count = 0;
	bool bInsert = true;
	bool bSingle = true; // true代表是奇数行，显示版本号，偶数行则代表是种子地址
	UpdatePacket packet;

	unsigned int i = 0;
	for (i=0; i<sContent.size(); ++i)
	{
		if (sContent.at(i) == '\n')
		{
			bInsert = false;
			end = count;
			if (bSingle) // 版本号
			{
				sVer = sContent.substr(start, end-start);
				ret = sscanf(sVer.c_str(), "%d.%d.%d", &hi, &lo, &build);

				if (ret == 3 && build > OldBuild)
				{
					if (hi != OldHi || lo != OldLo)
					{
						//::MessageBox(NULL, "游戏已出新的资料片，请访问www.linekong.com获取最新信息", "错误", MB_ICONSTOP|MB_OK);
						break;
					}
					else
					{
						packet.ver.byHiVersion   = hi;
						packet.ver.byLoVersion   = lo;
						packet.ver.wBuildVersion = build;
					}
				}
			}
			else // 种子地址
			{
				sTorrentUrl = sContent.substr(start, end-start); // 该处可能会导致错误，因为还没处理\r

				if (ret == 3 && build > OldBuild)
				{
					packet.url = sTorrentUrl;
					pl.push_back(packet);
				}
			}
			start = count + 1;

			if (bSingle)
				bSingle = false;
			else
				bSingle = true;
		}

		++count;
	}
}

NAMESPACE_END
