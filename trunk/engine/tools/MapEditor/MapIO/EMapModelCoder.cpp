#include "stdafx.h"
#include "EMapModelCoder.h"

BEGIN_VERSION_MAP(EMapModelCoder)
	ON_VERSION(1,(CCoder::PFN_DECODE)&EMapModelCoder::DeCode_Ver_1)
END_VERSION_MAP()

const DWORD EMapModelCoder::New_Version = 1;
EMapModelCoder::EMapModelCoder(CTerrainMesh* pTerrainMesh):m_pTerrain(pTerrainMesh){}
EMapModelCoder::~EMapModelCoder(){}

void DebugOutA(const char *format,...)
{

	static char a[50000];
	static char b[50000];

	va_list pz;
	va_start(pz, format);
	vsprintf(a,format,pz);
	USES_CONVERSION;
	sprintf(b,"yhl:%s",a);
	OutputDebugString(b);
}

bool EMapModelCoder::Code()
{
	CDataChunk outData;
	outData.setName(getChunkName());
	std::map< pair<std::string,int>, int> mapModels;
	vector< std::vector<std::string> > vvModelNames;
	vector<std::string> vecModelAnimationNames;
	vector<std::string> vecModelEffectNames;
	vector<std::string> vecModelEffectFileNames;
	vector<BYTE> vecModelShadow;
	vector<BYTE> vecModelTranslucent;
	vector<BYTE> vecModelFirstShadow;
	vector<BYTE> vecModelShadowZTest;
	vector<BYTE> vecModelSelfZTest;
	vector<BYTE> vecModelRenderStyle;

	int j = 0;
	int TSize = m_pTerrain->GetWidth()*m_pTerrain->GetDepth();
	size_t countSize = 0;
	
	for(int i = 0; i < TSize; ++i )
	{
		SGrid& Grid = m_pTerrain->GetGrid(i);

		if ( !Grid.vecCenterObjects.empty() )
		{	
			for ( std::vector< SmartPtr<bly::CTObject> >::iterator iter = Grid.vecCenterObjects.begin(); iter != Grid.vecCenterObjects.end(); ++iter )
			{
				bly::CTObject * p = (*iter);

				//////////////////////////////////////////////////////////////////////////

				if ( !p->GetFace()->BuildRealCenterObjectsForClient(p) )
					continue;

				//////////////////////////////////////////////////////////////////////////

				if ( !p->GetIsClientBrainBound() && !p->GetIsServerBrainBound() && !p->GetFace()->m_LG.m_Hey_IAmALogicObject )
				{
					std::string str = p->GetFace()->GetItemName();

					if ( str == "EFFECTDUMMY" )
						p->GetFace()->GetEffectName(str);

					int nRenderstyle = p->GetFace()->GetRenderStyle();

					std::map< std::pair<std::string,int>, int>::iterator iterModel = mapModels.find(make_pair(str,nRenderstyle));

					if ( iterModel == mapModels.end() )
					{
						mapModels[make_pair(str,nRenderstyle)] = j;

						std::vector<std::string> v;

						// arp
						if ( p->GetFace()->GetItemName() != "EFFECTDUMMY" )
						{
							v = p->GetFace()->GetModelFileNames();
							countSize+=p->GetFace()->GetModelFileNamesSize() + v.size()*sizeof(DWORD)*2;
						}
						else
						{
							v.push_back("");
						}

						vvModelNames.push_back(v);

						// ara						
						vecModelAnimationNames.push_back(p->GetFace()->GetAraFileName());
						countSize+= p->GetFace()->GetAraFileName().length() + sizeof(DWORD)*2*p->GetFace()->GetAraFileName().empty();
						// are
						std::string temp;

						if ( p->GetFace()->GetEffectFileName(temp) )
						{
							vecModelEffectFileNames.push_back(temp);
							countSize+= temp.length() + sizeof(DWORD)*temp.empty()*3;
							p->GetFace()->GetEffectName(temp);
							vecModelEffectNames.push_back(temp);
							countSize+=temp.length();
						}
						else
						{
							vecModelEffectFileNames.push_back("");
							vecModelEffectNames.push_back("");
						}

						// shadow
						if ( p->GetShadow() )
							vecModelShadow.push_back(1);
						else
							vecModelShadow.push_back(0);

						// translucent
						if ( p->GetTranslucent() )
						{
							vecModelTranslucent.push_back(1);
						}
						else
						{
							vecModelTranslucent.push_back(0);
						}

						// first shadow
						if ( p->GetFace()->GetShadowSecondPass() )
							vecModelFirstShadow.push_back(0);
						else
							vecModelFirstShadow.push_back(1);

						// shadow z test
						if ( p->GetZTest() )
							vecModelShadowZTest.push_back(1);
						else
							vecModelShadowZTest.push_back(0);

						// self z test
						if ( p->GetSelfZTest() )
							vecModelSelfZTest.push_back(1);
						else
							vecModelSelfZTest.push_back(0);

						// render style 
						BYTE byRenderStyle = p->GetFace()->GetRenderStyle();

						if ( byRenderStyle > 15 )
						{
							::MessageBox(0,"错误 渲染类型太大","",MB_OK);
						}

						vecModelRenderStyle.push_back(byRenderStyle);

						p->SetResSetIndex(j);


						++j;
					}
					else
					{
						p->SetResSetIndex(iterModel->second);
					}
				}
			}
		}
	}

	//改后缀名
	std::string name = "";
	size_t nPos = 0;

	for( size_t m = 0; m < vvModelNames.size(); ++m )
	{
		vector<std::string> vec = vvModelNames[m];

		for( size_t n = 0; n < vec.size(); ++n )
		{
			name = vec[n];

			// 				nPos = name.rfind(".arp");
			// 				if ( nPos != -1 )
			// 				{
			// 					name = name.substr(0, nPos);
			// 					name += ".mod";
			// 				}
		}

		//DebugOut("model name is %s", name.c_str());
	}

	for( size_t m = 0; m < vecModelAnimationNames.size(); ++m )
	{
		name = vecModelAnimationNames[m];

		// 			nPos = name.rfind(".ara");
		// 			if ( nPos != -1 )
		// 			{
		// 				name = name.substr(0, nPos);
		// 				name += ".ske";
		// 			}

		//DebugOut("ani name is %s", name.c_str());
	}

	for( size_t m = 0; m < vecModelEffectFileNames.size(); ++m )
	{
		name = vecModelEffectFileNames[m];

		// 			nPos = name.rfind(".are");
		// 			if ( nPos != -1 )
		// 			{
		// 				name = name.substr(0, nPos);
		// 				name += ".are";
		// 			}

		//DebugOut("effect name is %s", name.c_str());
	}

	// component cnt
	DWORD ARP = 'ARP';
	DWORD ARA = 'ARA';
	DWORD ARE = 'ARE';

	countSize+=sizeof(DWORD)+vvModelNames.size()*sizeof(BYTE)*2;
	char* pbuf = outData.allocate(countSize);
	DWORD TestL = 0;
	memcpy(pbuf,&j,sizeof(int));
	pbuf += sizeof(int);
	TestL += sizeof(int);
	for(size_t i = 0; i < vvModelNames.size(); ++i )
	{	
		vector<std::string> vec = vvModelNames[i];

		BYTE byIndividualCnt = 0;
		for ( size_t k = 0; k < vec.size(); ++k )
		{
			// arp
			if ( !vec[k].empty() ) 
			{
				++byIndividualCnt;
			}
		}

		// ara
		if ( !vecModelAnimationNames[i].empty() ) ++byIndividualCnt;

		// are
		if ( !vecModelEffectFileNames[i].empty() ) ++byIndividualCnt;

		BYTE by = vecModelRenderStyle[i];

		memcpy(pbuf,&by,sizeof(BYTE));
		pbuf += sizeof(BYTE);
		TestL += sizeof(BYTE);
		memcpy(pbuf,&byIndividualCnt,sizeof(BYTE));
		pbuf += sizeof(BYTE);
		TestL += sizeof(BYTE);

		// write arp
		for ( size_t j2 = 0; j2 < vec.size(); ++j2 )
		{
			if ( !vec[j2].empty() )
			{
				memcpy(pbuf,&ARP,sizeof(DWORD));
				pbuf += sizeof(DWORD);
				TestL += sizeof(DWORD);
				string str = vec[j2];
				//str = GetAlphabetic(str.c_str());
				DWORD n = str.length();
				memcpy(pbuf,&n,sizeof(DWORD));
				pbuf += sizeof(DWORD);
				TestL += sizeof(DWORD);
				memcpy(pbuf,str.c_str(),n);
				pbuf += n;
				TestL += n;
			}
		}

		// write ara
		if ( !vecModelAnimationNames[i].empty() )
		{
			memcpy(pbuf,&ARA,sizeof(DWORD));
			pbuf += sizeof(DWORD);
			TestL += sizeof(DWORD);
			string str = vecModelAnimationNames[i];

			//str = GetAlphabetic(str.c_str());
			int n = str.length();
			memcpy(pbuf,&n,sizeof(DWORD));
			pbuf += sizeof(DWORD);
			TestL += sizeof(DWORD);
			memcpy(pbuf,str.c_str(),n);
			pbuf += n;
			TestL += n;
		}					

		// write are
		if ( !vecModelEffectFileNames[i].empty() )
		{
			memcpy(pbuf,&ARE,sizeof(DWORD));
			pbuf += sizeof(DWORD);
			TestL += sizeof(DWORD);
			string str = vecModelEffectFileNames[i];

			//str = GetAlphabetic(str.c_str());
			int n = str.length();
			memcpy(pbuf,&n,sizeof(DWORD));
			pbuf += sizeof(DWORD);
			TestL += sizeof(DWORD);
			memcpy(pbuf,str.c_str(),n);
			pbuf += n;
			TestL += n;

			str = vecModelEffectNames[i];
			n = str.length();
			memcpy(pbuf,&n,sizeof(DWORD));
			pbuf += sizeof(DWORD);
			TestL += sizeof(DWORD);
			memcpy(pbuf,str.c_str(),n);
			pbuf += n;
			TestL += n;
		}
	}

	m_pMgr->Write(outData);
	return true;
}

bool EMapModelCoder::DeCode_Ver_1(const CDataChunk& inData)
{
	//此模块不能从文件中适配回来
	return false;
}
