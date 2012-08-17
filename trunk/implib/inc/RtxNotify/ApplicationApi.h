#ifndef APPLICATIONAPI_H
#define APPLICATIONAPI_H

#include <time.h>
#include <wtypes.h>
#include <winnt.h>

#ifdef APPLICATIONAPI_EXPORTS
#define APPSDKCAPI_(type) extern "C" __declspec(dllexport) type WINAPI
#else
#ifdef NDEBUG
#pragma comment( lib, "ApplicationApi.lib" )
#else
#pragma comment( lib, "ApplicationApi.lib" )
#endif
#define APPSDKCAPI_(type) extern "C" __declspec(dllimport) type WINAPI
#endif


#define ERRORBUFFERLEN									512
#define ONE_APP_THREAD									4
#define ENTERPRISEINFO_MAXLENGTH						2048
#define STATE_MAXLENGTH									32
#define RIGHTINFO_MAXLENGTH                             2048
#define RIGHTLIST_MAXLENGTH                             2048
#define ROLEINFO_MAXLENGTH								1024
#define ROLELIST_MAXLENGTH								2048
#define EXSTATELIST_MAXLENGTH							256


//下面的定义是ONAPPSTOP值
enum API_ERRCODE
{
	ERR_OK=0,
	ERR_RECONN=1,   //连接断开，正在重连
	ERR_DISCONN=2,  //重连失败，连接断开
	ERR_KICKOUT=3,	//被踢,应用停止
	ERR_OTHER=4     //其他未知原因导致应用停止
};


typedef VOID (CALLBACK* ONAPPSTOPA)(LPCSTR lpszguidApp, int iCode);
typedef VOID (CALLBACK* ONAPPSTOPW)(LPCWSTR lpszguidApp, int iCode);


typedef VOID (CALLBACK* ONRECVMSGA)(LPCSTR lpszguidApp, LPCSTR lpszAppName,LPCSTR lpszMsgType,LPCSTR lpszSender,LPCSTR lpszReceivers,LPCSTR lpszOfflineReceivers,LPCSTR lpszMsg,time_t tSendTime,DWORD dwFlag);
typedef VOID (CALLBACK* ONRECVMSGW)(LPCWSTR lpszguidApp, LPCWSTR lpszAppName,LPCWSTR lpszMsgType,LPCWSTR lpszSender,LPCWSTR lpszReceivers,LPCWSTR lpszOfflineReceivers,LPCWSTR lpszMsg,time_t tSendTime,DWORD dwFlag);

//事件处理器定义
typedef struct tagEVENTHANDLERA
{
	ONAPPSTOPA		AppStopHandler;			//离线处理器
	ONRECVMSGA	    MsgHandler;				//收到Msg的处理器
}EVENTHANDLERA,*PEVENTHANDLERA;

//事件处理器定义
typedef struct tagEVENTHANDLERW
{
	ONAPPSTOPW		AppStopHandler;			//离线处理器
	ONRECVMSGW	    MsgHandler;				//收到Msg处理器
}EVENTHANDLERW,*PEVENTHANDLERW;


//获取错误描述字符串
//lpszBuffer为输出参数,返回的错误描述字符串
//iBufferMax为用户为lpszResult准备的字符数，输入参数

APPSDKCAPI_(LPSTR) GetErrorStringA(int iErrorCode,LPSTR lpszBuffer,int iBufferMax);

APPSDKCAPI_(LPWSTR) GetErrorStringW(int iErrorCode,LPWSTR lpszBuffer,int iBufferMax);

//登录SDk Server
//lpszAddr为SDK Server地址，如果为NULL，则地址为本机
//wPort为SDK Server端口
//lpszAppGuid为应用GUID字符串
//lpszPwd为应用密码密码，如果没有设置密码，则lpszPwd应该为NULL
//lpszResult为输出参数，结果描述，XML格式，如，
//<RTXIMResult>
//<ResultCode> 0 </ResultCode>
//<ResultString>登录成功</ResultString>
//</RTXIMResult>

//iResultSize为用户为lpszResult准备的字符数，输入参数
APPSDKCAPI_(int) StartAppW(LPCWSTR lpszAddr,WORD wPort,LPCWSTR lpszAppGuid,LPCWSTR lpszPwd,PEVENTHANDLERW pEventHandlerW,int nNumberThread=ONE_APP_THREAD,LPWSTR lpszResult=NULL,int iResultSize=0);

APPSDKCAPI_(int) StartAppA(LPCSTR lpszAddr,WORD wPort,LPCSTR lpszAppGuid,LPCSTR lpszPwd,PEVENTHANDLERA pEventHandlerA,int nNumberThread=ONE_APP_THREAD,LPSTR lpszResult=NULL,int iResultSize=0);


//iResultSize为用户为lpszResult准备的字符数，输入参数
APPSDKCAPI_(int) StopAppW(LPCWSTR lpszguidApp);

APPSDKCAPI_(int) StopAppA(LPCSTR lpszguidApp);


APPSDKCAPI_(int) SendMsgA(LPCSTR lpszguidApp,LPCSTR lpszAppName,LPCSTR lpszMsgType,LPCSTR lpszSender,LPCSTR lpszReceivers,LPCSTR lpszMsg,DWORD dwFlag=0,BOOL bNeedSendResult=TRUE,LPSTR lpszResult=NULL,int iResultSize=0);
APPSDKCAPI_(int) SendMsgW(LPCWSTR lpszguidApp,LPCWSTR lpszAppName,LPCWSTR lpszMsgType,LPCWSTR lpszSender,LPCWSTR lpszReceivers,LPCWSTR lpszMsg,DWORD dwFlag=0,BOOL bNeedSendResult=TRUE,LPWSTR lpszResult=NULL,int iResultSize=0);


APPSDKCAPI_(int) RegisterAppA(LPCSTR lpszAddr,WORD wPort,LPCSTR lpszguidApp,LPCSTR lpszAppName,int  nAppPriority,LPCSTR lpszAppXmlDesc,LPSTR lpszResult=NULL,int iResultSize=0);
APPSDKCAPI_(int) RegisterAppW(LPCWSTR lpszAddr,WORD wPort,LPCWSTR lpszguidApp,LPCWSTR lpszAppName,int  nAppPriority,LPCWSTR lpszAppXmlDesc,LPWSTR lpszResult=NULL,int iResultSize=0);

APPSDKCAPI_(int) UnRegisterAppA(LPCSTR lpszAddr,WORD wPort,LPCSTR lpszguidApp, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) UnRegisterAppW(LPCWSTR lpszAddr,WORD wPort,LPCWSTR lpszguidApp, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) GetVersionA(LPCSTR lpszAddr,WORD wPort,LPSTR lpszVer,int nCount,LPSTR lpszResult=NULL,int iResultSize=0);
APPSDKCAPI_(int) GetVersionW(LPCWSTR lpszAddr,WORD wPort,LPWSTR lpszVer,int nCount,LPWSTR lpszResult=NULL,int iResultSize=0);

APPSDKCAPI_(int) GetEnterpriseInfoA(LPCSTR lpszAddr,WORD wPort,LPSTR lpszEnterpriseInfo,int nCount,LPSTR lpszResult=NULL,int iResultSize=0);
APPSDKCAPI_(int) GetEnterpriseInfoW(LPCWSTR lpszAddr,WORD wPort,LPWSTR lpszEnterpriseInfo,int nCount,LPWSTR lpszResult=NULL,int iResultSize=0);

APPSDKCAPI_(int) QueryUserStateA(LPCSTR lpszAddr,WORD wPort,LPCSTR lpszUser,LPSTR lpszState,int nCount,LPSTR lpszResult=NULL,int iResultSize=0);
APPSDKCAPI_(int) QueryUserStateW(LPCWSTR lpszAddr,WORD wPort,LPCWSTR lpszUser,LPWSTR lpszState,int nCount,LPWSTR lpszResult=NULL,int iResultSize=0);

APPSDKCAPI_(int) QueryUsersByStateA(LPCSTR lpszAddr,WORD wPort,LPCSTR lpszSTate,LPCSTR* ppUsers,LPSTR lpszResult=NULL,int iResultSize=0);
APPSDKCAPI_(int) QueryUsersByStateW(LPCWSTR lpszAddr,WORD wPort,LPCWSTR lpszSTate,LPCWSTR* ppUsers,LPWSTR lpszResult=NULL,int iResultSize=0);

APPSDKCAPI_(int) GetRightInfoA(LPCSTR lpszAddr,WORD wPort, LPCSTR lpszRight, LPSTR lpszRightInfo,int nCount,LPSTR lpszResult=NULL,int iResultSize=0);
APPSDKCAPI_(int) GetRightInfoW(LPCWSTR lpszAddr,WORD wPort, LPCWSTR lpszRight, LPWSTR lpszRightInfo,int nCount,LPWSTR lpszResult=NULL,int iResultSize=0);

APPSDKCAPI_(int) GetUserRightListA(LPCSTR lpszAddr,WORD wPort, LPCSTR lpszUser, LPSTR lpszRightList,int nCount,LPSTR lpszResult=NULL,int iResultSize=0);
APPSDKCAPI_(int) GetUserRightListW(LPCWSTR lpszAddr,WORD wPort, LPCWSTR lpszUser, LPWSTR lpszRightList,int nCount,LPWSTR lpszResult=NULL,int iResultSize=0);

APPSDKCAPI_(int) CheckRightAllowA(LPCSTR lpszAddr,WORD wPort, LPCSTR lpszUser, LPCSTR lpszRight, BOOL* pbAllow,LPSTR lpszResult=NULL,int iResultSize=0);
APPSDKCAPI_(int) CheckRightAllowW(LPCWSTR lpszAddr,WORD wPort, LPCWSTR lpszUser, LPCWSTR lpszRight, BOOL* pbAllow,LPWSTR lpszResult=NULL,int iResultSize=0);

APPSDKCAPI_(int) GetRoleInfoA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszRole, LPSTR lpszRoleInfo, int nCount, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) GetRoleInfoW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszRole, LPWSTR lpszRoleInfo, int nCount, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) AddRoleA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszRoleName, LPCSTR lpszRoleDescribe = NULL, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) AddRoleW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszRoleName, LPCWSTR lpszRoleDescribe = NULL, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) DeleteRoleA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszRoleName, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) DeleteRoleW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszRoleName, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) ModifyRoleInfoA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszOldRoleName, LPCSTR lpszNewRoleName, LPCSTR lpszNewRoleDescibe, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) ModifyRoleInfoW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszOldRoleName, LPCWSTR lpszNewRoleName, LPCWSTR lpszNewRoleDescibe, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) AddUserToRoleA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszUserName, LPCSTR lpszRoleName, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) AddUserToRoleW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszUserName, LPCWSTR lpszRoleName, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) DeleteUserFromRoleA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszUserName, LPCSTR lpszRoleName, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) DeleteUserFromRoleW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszUserName, LPCWSTR lpszRoleName, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) AddRightToRoleA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszRoleName, LPCSTR lpszRightIdentity, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) AddRightToRoleW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszRoleName, LPCWSTR lpszRightIdentity, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) DeleteRightFromRoleA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszRoleName, LPCSTR lpszRightIdentity, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) DeleteRightFromRoleW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszRoleName, LPCWSTR lpszRightIdentity, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) GetRoleRightListA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszRoleName, LPSTR pRightList, int nCount, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) GetRoleRightListW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszRoleName, LPWSTR pRightList, int nCount, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) GetUserRoleListA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszUserName, LPSTR pRoleList, int nCount, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) GetUserRoleListW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszUserName, LPWSTR pRoleList, int nCount, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) SetRoleRightValueA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszRoleName, LPCSTR lpszRightIdentity, BOOL bDeny, BOOL bAllow, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) SetRoleRightValueW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszRoleName, LPCWSTR lpszRightIdentity, BOOL bDeny, BOOL bAllow, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) GetRoleRightValueA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszRoleName, LPCSTR lpszRightIdentity, BOOL *pDeny, BOOL *pAllow, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) GetRoleRightValueW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszRoleName, LPCWSTR lpszRightIdentity, BOOL *pDeny, BOOL *pAllow, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) RegisterExStateA(LPCSTR lpszAddr,WORD wPort,LPCSTR lpszExStateName,LPSTR lpszResult=NULL,int iResultSize=0);
APPSDKCAPI_(int) RegisterExStateW(LPCWSTR lpszAddr,WORD wPort,LPCWSTR lpszExStateName,LPWSTR lpszResult=NULL,int iResultSize=0);

APPSDKCAPI_(int) UnRegisterExStateA(LPCSTR lpszAddr,WORD wPort,LPCSTR lpszExStateName,LPSTR lpszResult=NULL,int iResultSize=0);
APPSDKCAPI_(int) UnRegisterExStateW(LPCWSTR lpszAddr,WORD wPort,LPCWSTR lpszExStateName,LPWSTR lpszResult=NULL,int iResultSize=0);

APPSDKCAPI_(int) GetExStateListA(LPCSTR lpszAddr,WORD wPort,LPSTR lpszExStateList,int nCount,LPSTR lpszResult=NULL,int iResultSize=0);
APPSDKCAPI_(int) GetExStateListW(LPCWSTR lpszAddr,WORD wPort,LPWSTR lpszExStateList,int nCount,LPWSTR lpszResult=NULL,int iResultSize=0);

APPSDKCAPI_(int) SetUserExStateA(LPCSTR lpszAddr,WORD wPort,LPCSTR lpszExStateName,LPCSTR lpszUserList,LPCSTR lpszExStateList,LPSTR lpszResult=NULL,int iResultSize=0);
APPSDKCAPI_(int) SetUserExStateW(LPCWSTR lpszAddr,WORD wPort,LPCWSTR lpszExStateName,LPCWSTR lpszUserList,LPCWSTR lpszExStateList,LPWSTR lpszResult=NULL,int iResultSize=0);

APPSDKCAPI_(int) GetUserExStateA(LPCSTR lpszAddr,WORD wPort,LPCSTR lpszExStateName,LPCSTR lpszUserName,BYTE* pExState,LPSTR lpszResult=NULL,int iResultSize=0);
APPSDKCAPI_(int) GetUserExStateW(LPCWSTR lpszAddr,WORD wPort,LPCWSTR lpszExStateName,LPCWSTR lpszUserName,BYTE* pExState,LPWSTR lpszResult=NULL,int iResultSize=0);

APPSDKCAPI_(int) SetUserAuthTypeA(LPCSTR lpszAddr,WORD wPort,LPCSTR lpszUserName,BOOL bAuthType,LPSTR lpszResult=NULL,int iResultSize=0);
APPSDKCAPI_(int) SetUserAuthTypeW(LPCWSTR lpszAddr,WORD wPort,LPCWSTR lpszUserName,BOOL bAuthType,LPWSTR lpszResult=NULL,int iResultSize=0);

APPSDKCAPI_(int) SetUserAuthTypeExA(LPCSTR lpszAddr,WORD wPort,LPCSTR lpszUserName,BYTE byteAuthType,LPSTR lpszResult=NULL,int iResultSize=0);
APPSDKCAPI_(int) SetUserAuthTypeExW(LPCWSTR lpszAddr,WORD wPort,LPCWSTR lpszUserName,BYTE byteAuthType,LPWSTR lpszResult=NULL,int iResultSize=0);

APPSDKCAPI_(int) SetDeptAuthTypeA(LPCSTR lpszAddr,WORD wPort,LPCSTR lpszDept,int iAuthType,LPSTR lpszResult=NULL,int iResultSize=0);
APPSDKCAPI_(int) SetDeptAuthTypeW(LPCWSTR lpszAddr,WORD wPort,LPCWSTR lpszDept,int iAuthType,LPWSTR lpszResult=NULL,int iResultSize=0);

APPSDKCAPI_(int) QueryUserAuthTypeA(LPCSTR lpszAddr,WORD wPort,LPCSTR lpszUserName,BOOL* pbAuthType,LPSTR lpszResult=NULL,int iResultSize=0);
APPSDKCAPI_(int) QueryUserAuthTypeW(LPCWSTR lpszAddr,WORD wPort,LPCWSTR lpszUserName,BOOL* pbAuthType,LPWSTR lpszResult=NULL,int iResultSize=0);

APPSDKCAPI_(int) QueryUserAuthTypeExA(LPCSTR lpszAddr,WORD wPort,LPCSTR lpszUserName,BYTE* pbAuthType,LPSTR lpszResult=NULL,int iResultSize=0);
APPSDKCAPI_(int) QueryUserAuthTypeExW(LPCWSTR lpszAddr,WORD wPort,LPCWSTR lpszUserName,BYTE* pbAuthType,LPWSTR lpszResult=NULL,int iResultSize=0);

APPSDKCAPI_(int) GetSessionkeyA(LPCSTR lpszAddr,WORD wPort,LPCSTR lpszUserName,LPSTR lpszSessionkey,int iCount,LPSTR lpszResult=NULL,int iResultSize=0);
APPSDKCAPI_(int) GetSessionkeyW(LPCWSTR lpszAddr,WORD wPort,LPCWSTR lpszUserName,LPWSTR lpszSessionkey,int iCount,LPWSTR lpszResult=NULL,int iResultSize=0);

APPSDKCAPI_(int) AddUserA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszUserName, long lAuthType = 0, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) AddUserW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszUserName, long lAuthType = 0, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) DeleteUserA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszUserName, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) DeleteUserW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszUserName, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) SetUserPwdA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszUserName, LPCSTR lpszPwd, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) SetUserPwdW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszUserName, LPCWSTR lpszPwd, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) GetUserBasicInfoA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszUserName, LPSTR lpszName, int iNameBufLen, long* plGender, LPSTR lpszMobile, int iMobileBufLen,
								   LPSTR lpszEmail, int iEmailBufLen, LPSTR lpszPhone, int iPhoneBufLen, long* plAuthType, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) GetUserBasicInfoW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszUserName, LPWSTR lpszName, int iNameBufLen, long* plGender, LPWSTR lpszMobile, int iMobileBufLen,
								   LPWSTR lpszEmail, int iEmailBufLen, LPWSTR lpszPhone, int iPhoneBufLen, long* plAuthType, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) SetUserBasicInfoA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszUserName, LPCSTR lpszName, long lGender, LPCSTR lpszMobile, LPCSTR lpszEMail, LPCSTR lpszPhone, long lAuthType = 0, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) SetUserBasicInfoW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszUserName, LPCWSTR lpszName, long lGender, LPCWSTR lpszMobile, LPCWSTR lpszEMail, LPCWSTR lpszPhone, long lAuthType = 0, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) GetUserDetailInfoA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszUserName, long* plBirthday, short* psBloodType, short* psConstellation,
									LPSTR lpszCollege, int iColBufLen, LPSTR lpszHomePage, int iHpBufLen, LPSTR lpszFax, int iFaxBufLen,
									LPSTR lpszAddress, int iAddrBufLen, LPSTR lpszPostCode, int iPCodeBufLen,
									LPSTR lpszCountry, int iCtryBufLen, LPSTR lpszProvince, int iProvBufLen, LPSTR lpszCity, int iCityBufLen, LPSTR lpszStreet, int iStreetBufLen,
									LPSTR lpszResult = NULL, int iResultSize = 0, LPSTR lpszPosition = NULL, int iPositionBufLen = 0, long* plUserLevel = NULL);
APPSDKCAPI_(int) GetUserDetailInfoW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszUserName, long* plBirthday, short* psBloodType, short* psConstellation,
									LPWSTR lpszCollege, int iColBufLen, LPWSTR lpszHomePage, int iHpBufLen, LPWSTR lpszFax, int iFaxBufLen,
									LPWSTR lpszAddress, int iAddrBufLen, LPWSTR lpszPostCode, int iPCodeBufLen,
									LPWSTR lpszCountry, int iCtryBufLen, LPWSTR lpszProvince, int iProvBufLen, LPWSTR lpszCity, int iCityBufLen, LPWSTR lpszStreet, int iStreetBufLen,
									LPWSTR lpszResult = NULL, int iResultSize = 0, LPWSTR lpszPosition = NULL, int iPositionbufLen = 0, long *plUserLevel = NULL);

APPSDKCAPI_(int) SetUserDetailInfoA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszUserName, long lBirthday, short sBloodType, short sConstellation,
									LPCSTR lpszCollege, LPCSTR lpszHomePage, LPCSTR lpszFax, LPCSTR lpszAddress, LPCSTR lpszPostCode, 
									LPCSTR lpszCountry, LPCSTR lpszProvince, LPCSTR lpszCity, LPCSTR lpszStreet, LPSTR lpszResult = NULL, int iResultSize = 0, LPCSTR lpszPosition = NULL, long lUserLevel = 0);
APPSDKCAPI_(int) SetUserDetailInfoW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszUserName, long lBirthday, short sBloodType, short sConstellation,
									LPCWSTR lpszCollege, LPCWSTR lpszHomePage, LPCWSTR lpszFax, LPCWSTR lpszAddress, LPCWSTR lpszPostCode,
									LPCWSTR lpszCountry, LPCWSTR lpszProvince, LPCWSTR lpszCity, LPCWSTR lpszStreet, LPWSTR lpszResult = NULL, int iResultSize = 0, LPCWSTR lpszPosition = NULL, long lUserLevel = 0);

APPSDKCAPI_(int) SendNotifyA(LPCSTR lpszAddr,WORD wPort,LPCSTR lpszReceiver,LPCSTR lpszTitle,long lDelayTime,LPCSTR lpszMsg,LPSTR lpszResult=NULL,int iResultSize=0);
APPSDKCAPI_(int) SendNotifyW(LPCWSTR lpszAddr,WORD wPort,LPCWSTR lpszReceiver,LPCWSTR lpszTitle,long lDelayTime,LPCWSTR lpszMsg,LPWSTR lpszResult=NULL,int iResultSize=0);

APPSDKCAPI_(int) AddDeptA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszDeptName, LPCSTR lpszParentDept, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) AddDeptW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszDeptName, LPCWSTR lpszParentDept, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) DelDeptA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszDeptName, BOOL bDelComplete, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) DelDeptW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszDeptName, BOOL bDelComplete, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) SetDeptNameA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszDeptName, LPCSTR lpszNewDeptName, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) SetDeptNameW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszDeptName, LPCWSTR lpszNewDeptName, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) GetParentDeptA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszDeptName, LPSTR lpszParentDept, int iCount, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) GetParentDeptW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszDeptName, LPWSTR lpszParentDept, int iCount, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) SetParentDeptA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszDeptName, LPCSTR lpszParentDept, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) SetParentDeptW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszDeptName, LPCWSTR lpszParentDept, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) GetChildDeptsA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszDeptName, LPSTR lpszChildDepts, int iCount, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) GetChildDeptsW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszDeptName, LPWSTR lpszChildDepts, int iCount, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) GetDeptUsersA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszDeptName, LPSTR lpszDeptUsers, int iCount, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) GetDeptUsersW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszDeptName, LPWSTR lpszDeptUsers, int iCount, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) GetUserDeptsA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszUserName, LPSTR lpszDepts, int iCount, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) GetUserDeptsW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszUserName, LPWSTR lpszDepts, int iCount, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) AddUserToDeptA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszUserName, LPCSTR lpszSrcDeptName, LPCSTR lpszDestDeptName, BOOL bIsCopy = FALSE, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) AddUserToDeptW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszUserName, LPCWSTR lpszSrcDeptName, LPCWSTR lpszDestDeptName, BOOL bIsCopy = FALSE, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) DelUserFromDeptA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszUserName, LPCSTR lpszDeptName, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) DelUserFromDeptW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszUserName, LPCWSTR lpszDeptName, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) EnumAllOrgInfoA(LPCSTR lpszAddr, WORD wPort, LPSTR lpszOrgInfo, int iCount, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) EnumAllOrgInfoW(LPCWSTR lpszAddr, WORD wPort, LPWSTR lpszOrgInfo, int iCount, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) SetOrgUserA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszUserName, LPCSTR lpszOrgName, LPSTR lpszResult = NULL, int iResultSize = 0);
APPSDKCAPI_(int) SetOrgUserW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszUserName, LPCWSTR lpszOrgName, LPWSTR lpszResult = NULL, int iResultSize = 0);

APPSDKCAPI_(int) SignatureAuthA(LPCSTR lpszAddr,WORD wPort, LPCSTR lpszUser, LPCSTR lpszSign, BOOL* pbSuccess, LPSTR lpszResult=NULL, int iResultSize = 0);
APPSDKCAPI_(int) SignatureAuthW(LPCWSTR lpszAddr,WORD wPort, LPCWSTR lpszUser, LPCWSTR lpszSign, BOOL* pbSuccess, LPWSTR lpszResult=NULL, int iResultSize = 0);

APPSDKCAPI_(void) FreeMemory(void* pData);

APPSDKCAPI_(int) LoginA(LPCSTR lpszAddr, WORD wPort, LPCSTR lpszUserAccount, LPCSTR lpszPassWord);
APPSDKCAPI_(int) LoginW(LPCWSTR lpszAddr, WORD wPort, LPCWSTR lpszUserAccount, LPCWSTR lpszPassWord);


#ifdef UNICODE

#define ONRECVMSG	ONRECVMSGW
#define EVENTHANDLER	EVENTHANDLERW
#define StartApp StartAppW
#define StopApp  StopAppW
#define GetErrorString GetErrorStringW
#define SendMsg SendMsgW
#define RegisterApp RegisterAppW
#define UnRegisterApp UnRegisterAppW
#define GetVersion GetVersionW
#define GetEnterpriseInfo GetEnterpriseInfoW
#define QueryUserState	QueryUserStateW
#define QueryUsersByState QueryUsersByStateW
#define GetRightInfo  GetRightInfoW
#define GetUserRightList  GetUserRightListW
#define CheckRightAllow CheckRightAllowW
#define GetRoleInfo  GetRoleInfoW
#define AddRole  AddRoleW
#define DeleteRole  DeleteRoleW
#define ModifyRoleInfo  ModifyRoleInfoW
#define AddUserToRole  AddUserToRoleW
#define DeleteUserFromRole  DeleteUserFromRoleW
#define AddRightToRole  AddRightToRoleW
#define DeleteRightFromRole  DeleteRightFromRoleW
#define GetRoleRightList  GetRoleRightListW
#define GetUserRoleList  GetUserRoleListW
#define SetRoleRightValue  SetRoleRightValueW
#define GetRoleRightValue  GetRoleRightValueW

#define RegisterExState	RegisterExStateW
#define UnRegisterExState UnRegisterExStateW
#define GetExStateList GetExStateListW
#define SetUserExState SetUserExStateW
#define GetUserExState GetUserExStateW

#define SetUserAuthType SetUserAuthTypeW
#define SetUserAuthTypeEx  SetUserAuthTypeExW
#define QueryUserAuthType QueryUserAuthTypeW
#define QueryUserAuthTypeEx QueryUserAuthTypeExW
#define SetDeptAuthType SetDeptAuthTypeW

#define GetSessionkey GetSessionkeyW

#define AddUser  AddUserW
#define DeleteUser  DeleteUserW
#define SetUserPwd  SetUserPwdW
#define GetUserBasicInfo  GetUserBasicInfoW
#define SetUserBasicInfo  SetUserBasicInfoW
#define GetUserDetailInfo  GetUserDetailInfoW
#define SetUserDetailInfo  SetUserDetailInfoW

#define SendNotify SendNotifyW

#define AddDept  AddDeptW
#define DelDept  DelDeptW
#define SetDeptName  SetDeptNameW
#define GetParentDept  GetParentDeptW
#define SetParentDept  SetParentDeptW
#define GetChildDepts  GetChildDeptsW
#define GetDeptUsers  GetDeptUsersW
#define GetUserDepts  GetUserDeptsW
#define AddUserToDept  AddUserToDeptW
#define DelUserFromDept  DelUserFromDeptW

#define EnumAllOrgInfo  EnumAllOrgInfoW
#define SetOrgUser  SetOrgUserW

#define SignatureAuth SignatureAuthW
#define Login	LoginW

#else

#define ONRECVMSG	ONRECVMSGA
#define EVENTHANDLER	EVENTHANDLERA
#define StartApp StartAppA
#define StopApp  StopAppA
#define GetErrorString GetErrorStringA
#define SendMsg SendMsgA
#define RegisterApp RegisterAppA
#define UnRegisterApp UnRegisterAppA
#define GetVersion GetVersionA
#define GetEnterpriseInfo GetEnterpriseInfoA
#define QueryUserState	QueryUserStateA
#define QueryUsersByState QueryUsersByStateA
#define GetRightInfo  GetRightInfoA
#define GetUserRightList  GetUserRightListA
#define CheckRightAllow CheckRightAllowA
#define GetRoleInfo  GetRoleInfoA
#define AddRole  AddRoleA
#define DeleteRole  DeleteRoleA
#define ModifyRoleInfo  ModifyRoleInfoA
#define AddUserToRole  AddUserToRoleA
#define DeleteUserFromRole  DeleteUserFromRoleA
#define AddRightToRole  AddRightToRoleA
#define DeleteRightFromRole  DeleteRightFromRoleA
#define GetRoleRightList  GetRoleRightListA
#define GetUserRoleList  GetUserRoleListA
#define SetRoleRightValue  SetRoleRightValueA
#define GetRoleRightValue  GetRoleRightValueA

#define RegisterExState	RegisterExStateA
#define UnRegisterExState	UnRegisterExStateA
#define GetExStateList GetExStateListA
#define SetUserExState SetUserExStateA
#define GetUserExState GetUserExStateA

#define SetUserAuthType SetUserAuthTypeA
#define SetUserAuthTypeEx  SetUserAuthTypeExA
#define QueryUserAuthType QueryUserAuthTypeA
#define QueryUserAuthTypeEx QueryUserAuthTypeExA
#define SetDeptAuthType SetDeptAuthTypeA

#define GetSessionkey GetSessionkeyA

#define AddUser  AddUserA
#define DeleteUser  DeleteUserA
#define SetUserPwd  SetUserPwdA
#define GetUserBasicInfo  GetUserBasicInfoA
#define SetUserBasicInfo  SetUserBasicInfoA
#define GetUserDetailInfo  GetUserDetailInfoA
#define SetUserDetailInfo  SetUserDetailInfoA

#define SendNotify SendNotifyA

#define AddDept  AddDeptA
#define DelDept  DelDeptA
#define SetDeptName  SetDeptNameA
#define GetParentDept  GetParentDeptA
#define SetParentDept  SetParentDeptA
#define GetChildDepts  GetChildDeptsA
#define GetDeptUsers  GetDeptUsersA
#define GetUserDepts  GetUserDeptsA
#define AddUserToDept  AddUserToDeptA
#define DelUserFromDept  DelUserFromDeptA

#define EnumAllOrgInfo  EnumAllOrgInfoA
#define SetOrgUser  SetOrgUserA

#define SignatureAuth SignatureAuthA

#define Login	LoginA

#endif


#endif