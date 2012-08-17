#include "stdafx.h"
#include "CComputerInfo.h"
#include "BaseHelper.h"
#include <Psapi.h>
#include "MMSystem.h"
#include "CodeCvs.h"
#include "GraphicBase.h"
/*
默认是所有功能都磨认为最弱功能
*/

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4996)
#pragma warning(disable:4819)
#endif

#include <comdef.h>
#include <Wbemidl.h>
#pragma comment(lib,"WbemUuid.lib")

#define  DirectDrawKey			"Software\\Microsoft\\DirectDraw"
#define  DirectDrawValue		"EmulationOnly"
#define  Direct3DKey			"Software\\Microsoft\\Direct3D\\Drivers"
#define  Direct3DValue			"SoftwareOnly"
#define  DirectAGPKey			"Software\\Microsoft\\DirectDraw"
#define  DirectAGPValue			"DisableAGPSupport"
#define  DirectVersionKey		"Software\\Microsoft\\DirectX"
#define  DirectVersionValue		"Version"
#define  OSKey					"System\\CurrentControlSet\\Control\\ProductOptions"
#define  OSValue				"ProductType"
#define  CPUKey					"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"
#define  CPUValue				"ProcessorNameString"
#define  VideoServicesKey		"SYSTEM\\CurrentControlSet\\Services"
#define  VideoEnumKey			"SYSTEM\\CurrentControlSet\\Enum"



LPCTSTR GetRegistKeyStr( LPCTSTR Path, LPCTSTR Name )
{
	HKEY hKey = NULL;
	DWORD Len = 1024;
	DWORD Type = REG_SZ;
	static BYTE  Buf[1024];
	RegCreateKey( HKEY_LOCAL_MACHINE, Path, &hKey );
	if( ERROR_SUCCESS != RegQueryValueEx( hKey, Name, NULL, &Type, Buf, &Len ) )
		Buf[0] = 0;
	RegCloseKey(hKey);

	return (TCHAR*)Buf;
}

DWORD GetRegistKeyDWORD( LPCTSTR Path, LPCTSTR Name )
{
	HKEY hKey = NULL;
	DWORD Len = 1024;
	DWORD Type = REG_DWORD;

	DWORD dwRet = 0;

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,Path,0,KEY_QUERY_VALUE,&hKey)==ERROR_SUCCESS)
	{
		DWORD dwValue,dwType;
		DWORD dwBufLen = 255;
		if(RegQueryValueEx(hKey,Name,NULL,&dwType,(LPBYTE)&dwValue,&dwBufLen)==ERROR_SUCCESS)
		{
			if(dwType==REG_DWORD)
				dwRet = dwValue;
			else
				dwRet = -1;
		}
		else
			dwRet = -1;
		RegCloseKey(hKey);
	}
	else
		dwRet = -1;
	return dwRet;
}

string CComputerInfo::GetDirectInfo()
{
	string DirectVersion = GetRegistKeyStr(DirectVersionKey,DirectVersionValue);
	return DirectVersion;
}

string CComputerInfo::GetAudioCardInfo()
{
	//HKEY         keyServ   =   NULL; 
	//HKEY         keyEnum   =   NULL; 
	//HKEY         key       =   NULL; 
	//HKEY         key2      =   NULL; 
	//DWORD        error     =   0; 
	//LPCTSTR		 api       =   NULL; 
	//DWORD        type,   count,   size,   i,   iServ; 
	//list<string> save; 
	//string err = "Get videocard info faild!";

	//if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,VideoServicesKey,0,KEY_READ,&keyServ)!=ERROR_SUCCESS)
	//{
	//	RegCloseKey(keyServ);
	//	err = err + "VideoServicesKey";
	//	return err;
	//}

	//if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,VideoEnumKey,0,KEY_READ,&keyEnum)!=ERROR_SUCCESS)
	//{
	//	if(keyServ)
	//		RegCloseKey(keyServ);
	//	RegCloseKey(keyEnum);
	//	err = err+"VideoEnumKey";
	//	return err;
	//}

	//i   =   0; 
	//for   (iServ=0;;++iServ) 
	//{ 
	//	TCHAR   name   [0x200]; 
	//	size   =   0x200; 
	//	error   =   RegEnumKeyEx   (keyServ,   iServ,   name,   &size,   
	//		NULL,   NULL,   NULL,   NULL); 

	//	if   (error   ==   ERROR_NO_MORE_ITEMS) 
	//		break; 

	//	error   =   RegOpenKeyEx   (keyServ,   name,   0,   KEY_READ,   &key); 

	//	if(error!=ERROR_SUCCESS)
	//		break;

	//	size   =   0x200; 
	//	type = REG_DWORD;
	//	error   =   RegQueryValueEx   (key,   "Group",   NULL,   &type,   (LPBYTE)name,   &size); 

	//	if   (error   ==   ERROR_FILE_NOT_FOUND) 
	//	{       //     键不存在 
	//		RegCloseKey   (key); 
	//		continue; 
	//	}; 

	//	if(error!=ERROR_SUCCESS)
	//	{
	//		RegCloseKey   (key); 
	//		break;
	//	}

	//	string videoword = name;
	//	if   (videoword!="Video") 
	//	{       //     不是显卡的内核驱动 
	//		RegCloseKey   (key); 
	//		continue; 
	//	}; 


	//	error   =   RegOpenKeyEx(key,   "Enum",   0,   KEY_READ,   &key2); 
	//	if   (error   ==   ERROR_FILE_NOT_FOUND) 
	//	{       //     键不存在 
	//		RegCloseKey   (key); 
	//		continue; 
	//	}; 
	//	RegCloseKey   (key); 
	//	key   =   key2; 

	//	size   =   sizeof   (count); 
	//	error   =   RegQueryValueEx   (key,   "Count",   
	//		0,   &type,   (LPBYTE)&count,   &size); 

	//	if(error!=ERROR_SUCCESS)
	//	{
	//		RegCloseKey   (key); 
	//		break;
	//	}

	//	for   (i=0;i <count;++i) 
	//	{ 
	//		TCHAR   sz   [0x200]; 
	//		TCHAR   name   [0x40]; 

	//		wsprintf   (name,   "%d",   i); 
	//		size   =   sizeof   (sz); 
	//		error   =   RegQueryValueEx   (key,   name,   
	//			0,   &type,   (LPBYTE)sz,   &size); 

	//		if(error!=ERROR_SUCCESS)
	//			break;

	//		error   =   RegOpenKeyEx   (keyEnum,   sz,   
	//			0,   KEY_READ,   &key2); 

	//		if(error!=ERROR_SUCCESS)
	//			break;
	//		size   =   sizeof   (sz); 
	//		error   =   RegQueryValueEx   (key2,   "FriendlyName",   
	//			0,   &type,   (LPBYTE)sz,   &size); 

	//		if   (error   ==   ERROR_FILE_NOT_FOUND) 
	//		{ 
	//			size   =   sizeof   (sz); 
	//			error   =   RegQueryValueEx   (key2,  "DeviceDesc",   
	//				0,   &type,   (LPBYTE)sz,   &size); 
	//		}; 
	//		if(error!=ERROR_SUCCESS)
	//			break;

	//		save.push_back   (sz); 
	//		RegCloseKey   (key2); 
	//		key2   =   NULL; 
	//	}; 
	//	if(key)
	//		RegCloseKey   (key); 
	//	key   =   NULL; 
	//	if(key2)
	//		RegCloseKey   (key2); 
	//	key2 = NULL;
	//}; 

	//if(keyServ) 
	//	RegCloseKey(keyServ); 
	//if(keyEnum) 
	//	RegCloseKey(keyEnum); 

	//string info;
	//if(!save.empty()) 
	//{ 
	//	std::list   <string> ::iterator   it,   end; 
	//	it     =   save.begin   (); 
	//	end   =   save.end   (); 

	//	for(i=0;it!=end;++it) 
	//		info+= it->c_str();
	//} 
	//if(save.empty())
	//	return err; 
	//else
	//	return info;

	int nCount = waveOutGetNumDevs();
	vector<string> vecStrs;
	WAVEOUTCAPS woc;
	for (int i = 0; i < nCount; ++i)
	{
		MMRESULT hr = waveOutGetDevCaps(i, &woc, sizeof(WAVEOUTCAPS));
		if( hr == MMSYSERR_NOERROR )
		{				
			vecStrs.push_back(woc.szPname);
		}
	}
	string Info = "";
	for(int i = 0;i<(int)vecStrs.size();i++)
	{
		Info += vecStrs[i];
		Info += ";";
	}
	return Info;
}

string CComputerInfo::GetCpuInfo()
{
	string cpuinfo = GetRegistKeyStr(CPUKey,CPUValue);
	return cpuinfo;
}

string CComputerInfo::GetHDInfo()
{
	_ULARGE_INTEGER result_freespace,result_totalspace,result_g;

	int error = 0;
	ULONGLONG free_space,total_space;

	string strdir;
	ULONGLONG totalbulk=0;
	ULONGLONG freebulk=0;
	char buf[80];

	for(char cc='A';cc<='Z';cc++)
	{
		strdir = cc;
		strdir+=":";
		if(GetDriveType((LPCTSTR)strdir.c_str())==DRIVE_FIXED)
		{
			GetDiskFreeSpaceEx(strdir.c_str(),&result_freespace,&result_totalspace,&result_g);
			free_space = result_freespace.QuadPart/1024/1024;
			total_space = result_totalspace.QuadPart/1024/1024;
			totalbulk+=total_space;
			freebulk+=free_space;
		}
	}

	sprintf(buf,"%dG",totalbulk/1024);
	strdir = buf;
	sprintf(buf," %dG",freebulk/1024);
	strdir += buf;

	wchar_t szCurPath[1024];
	_wgetcwd(szCurPath, _countof(szCurPath));
	strdir += string("\ninstall path: ") + utf16_to_utf8(szCurPath);

	return strdir;
}

//string CComputerInfo::GetOSInfo()
//{
//	string m_stInfo;
//	string m_stMinorVer;
//	string m_stServPack;
//	string m_stBuildNumber;
//
//	char buffer[20];
//	string data;
//	DWORD win95Info;
//	OSVERSIONINFOEX versionInfo;
//	// set the size of OSVERSIONINFO, before calling the function
//	ZeroMemory(&versionInfo, sizeof(OSVERSIONINFOEX));
//	versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
//	// Get the version information
//
//	if (!GetVersionEx ((OSVERSIONINFO *) &versionInfo)) 
//	{
//		return "OSInfo Get fail!";
//	}
//
//	// If NT ,then check if its server or workstation.
//
//	if (versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) {
//		m_stInfo =  "Windows NT";
//
//		data = GetRegistKeyStr(OSKey,OSValue);
//
//		if (data=="WinNT") {
//			m_stInfo = "Windows NT Workstation";
//			if ( versionInfo.dwMajorVersion == 6 && versionInfo.dwMinorVersion == 0 && versionInfo.wProductType == VER_NT_WORKSTATION )
//				m_stInfo = "WindowsVista";
//			else if ( versionInfo.dwMajorVersion == 6 && versionInfo.dwMinorVersion == 1 )
//				m_stInfo = "Win7";
//			else if ( versionInfo.dwMajorVersion == 5 && versionInfo.dwMinorVersion == 1 )
//				m_stInfo = "WindowsXP";
//		}
//		else if (data=="ServerNT") {
//			m_stInfo = "Windows NT Server";
//		}
//		else {
//			m_stInfo = "Windows NT Server - Domain Controller";
//		}
//
//		// Check the version number
//
//		if (versionInfo.dwMajorVersion == 3 || versionInfo.dwMinorVersion == 51) {
//			m_stMinorVer = "3.51";
//		}
//		else {
//			m_stMinorVer = "4.0";
//		}
//
//		// Get the build number.
//
//		_itoa( versionInfo.dwBuildNumber, buffer, 10 );
//		m_stBuildNumber = buffer;
//	}
//	else if (versionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
//		m_stInfo = "Windows 95";
//
//		if ((versionInfo.dwMajorVersion > 4) || ((versionInfo.dwMajorVersion == 4)
//			&& (versionInfo.dwMinorVersion > 0))) {
//				m_stInfo = "Windows 98";
//		}
//
//		// For win95, build number is low order word
//
//		win95Info = (DWORD)(LOBYTE(LOWORD(versionInfo.dwBuildNumber)));
//
//		// Get the build number.
//
//		_itoa( win95Info, buffer, 10 );
//		m_stBuildNumber = buffer;
//	}
//	else {
//		m_stInfo = "Windows 3.1";
//	}
//
//	// Get service pack information.
//	m_OS = m_stInfo;
//
//	m_stServPack = versionInfo.szCSDVersion;
//
//	m_stInfo += " ";
//	m_stInfo += m_stServPack;
//	return m_stInfo;
//}


string CComputerInfo::GetOSInfo()
{
	string OSName;
	string OSServPack;
	string OSCode;

   HRESULT hres;

    // Step 1: --------------------------------------------------
    // Initialize COM. ------------------------------------------

    hres =  CoInitializeEx(0, COINIT_APARTMENTTHREADED); 
    if (FAILED(hres))
    {
		ostringstream err;
        err<< "Failed to initialize COM library. Error code = 0x" 
            << hex << hres;
        return err.str();                  // Program has failed.
    }

    // Step 2: --------------------------------------------------
    // Set general COM security levels --------------------------
    // Note: If you are using Windows 2000, you need to specify -
    // the default authentication credentials for a user by using
    // a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----
    // parameter of CoInitializeSecurity ------------------------

    hres =  CoInitializeSecurity(
        NULL, 
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
        );

                      
    if (FAILED(hres))
    {
		ostringstream err;
        err<< "Failed to initialize security. Error code = 0x" 
            << hex << hres ;
        CoUninitialize();
        return err.str();                    // Program has failed.
    }
    
    // Step 3: ---------------------------------------------------
    // Obtain the initial locator to WMI -------------------------

    IWbemLocator *pLoc = NULL;

    hres = CoCreateInstance(
        CLSID_WbemLocator,             
        0, 
        CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, (LPVOID *) &pLoc);
 
    if (FAILED(hres))
    {
		ostringstream err;
        err<< "Failed to create IWbemLocator object."
            << " Err code = 0x"
            << hex << hres ;
        CoUninitialize();
        return err.str();                 // Program has failed.
    }

    // Step 4: -----------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method

    IWbemServices *pSvc = NULL;
	
    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.
    hres = pLoc->ConnectServer(
         _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
         NULL,                    // User name. NULL = current user
         NULL,                    // User password. NULL = current
         0,                       // Locale. NULL indicates current
         NULL,                    // Security flags.
         0,                       // Authority (e.g. Kerberos)
         0,                       // Context object 
         &pSvc                    // pointer to IWbemServices proxy
         );
    
    if (FAILED(hres))
    {
		ostringstream err;
        err<< "Could not connect. Error code = 0x" 
             << hex << hres ;
        pLoc->Release();     
        CoUninitialize();
        return err.str();                // Program has failed.
    }

    // Step 5: --------------------------------------------------
    // Set security levels on the proxy -------------------------

    hres = CoSetProxyBlanket(
       pSvc,                        // Indicates the proxy to set
       RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
       RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
       NULL,                        // Server principal name 
       RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
       RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
       NULL,                        // client identity
       EOAC_NONE                    // proxy capabilities 
    );

    if (FAILED(hres))
    {
		ostringstream err;
        err<< "Could not set proxy blanket. Error code = 0x" 
            << hex << hres ;
        pSvc->Release();
        pLoc->Release();     
        CoUninitialize();
        return err.str();               // Program has failed.
    }

    // Step 6: --------------------------------------------------
    // Use the IWbemServices pointer to make requests of WMI ----

    // For example, get the name of the operating system
    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
        bstr_t("WQL"), 
        bstr_t("SELECT * FROM Win32_OperatingSystem"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
        NULL,
        &pEnumerator);
    
    if (FAILED(hres))
    {
		ostringstream err;
        err<< "Query for operating system name failed."
            << " Error code = 0x" 
            << hex << hres ;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return err.str();               // Program has failed.
    }

    // Step 7: -------------------------------------------------
    // Get the data from the query in step 6 -------------------
 
    IWbemClassObject *pclsObj;
    ULONG uReturn = 0;
   
	if (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, 
			&pclsObj, &uReturn);

		if(0 != uReturn)
		{
			VARIANT vtProp;
			char* tcode;
			// Get the value of the Name property
			hr = pclsObj->Get(L"Caption", 0, &vtProp, 0, 0);
			tcode = _com_util::ConvertBSTRToString(vtProp.bstrVal) ;
			OSName = tcode;
			GfkSafeDelete(tcode);
			VariantClear(&vtProp);

			hr = pclsObj->Get(L"CSDVersion", 0, &vtProp, 0, 0);
			tcode = _com_util::ConvertBSTRToString(vtProp.bstrVal);
			OSServPack = tcode;
			GfkSafeDelete(tcode);
			VariantClear(&vtProp);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"), 
		bstr_t("SELECT * FROM Win32_ComputerSystem"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
		NULL,
		&pEnumerator);

	if (FAILED(hres))
	{
		ostringstream err;
		err<< "Query for processor name failed."
			<< " Error code = 0x" 
			<< hex << hres ;
		return err.str();
	}

	// Get the data from the query
	uReturn = 0;

	if(pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, 
			&pclsObj, &uReturn);

		if ( SUCCEEDED( hres ) )
		{
			if(uReturn > 0)
			{
				VARIANT vtProp;
				VariantInit(&vtProp);
				// Get the value of the Name property
				hr = pclsObj->Get(L"SystemType", 0, &vtProp, 0, 0);
				char* tcode = _com_util::ConvertBSTRToString(vtProp.bstrVal) ;
				OSCode = tcode;
				GfkSafeDelete(tcode);
				VariantClear(&vtProp);
			}
		}
	}

	// successfully completed.
	pEnumerator->Release();
	//////////////////////////////////////////////////////////////////////////
    // Cleanup
    // ========
    
    pSvc->Release();
    pLoc->Release();
    pclsObj->Release();
    CoUninitialize();

	string OsInfo;
	OsInfo += OSName +":"+ OSServPack +":"+OSCode;
	return OsInfo;
}

string CComputerInfo::GetMemInfo()
{
	MEMORYSTATUS memStatus;
	char buf[80];
	string memsta="";
	memStatus.dwLength=sizeof(MEMORYSTATUS);

	GlobalMemoryStatus(&memStatus);

	DWORD tom=memStatus.dwTotalPhys;
	DWORD mem=memStatus.dwAvailPhys;
	DWORD res=memStatus.dwAvailVirtual;

	tom=(tom/1024)/1024;
	mem=(mem/1024)/1024;
	res=(res/1024)/1024;
	sprintf(buf, "%ld", tom);
	memsta = buf;
	return memsta;
}

#ifdef _WIN32
#pragma warning(pop)
#endif