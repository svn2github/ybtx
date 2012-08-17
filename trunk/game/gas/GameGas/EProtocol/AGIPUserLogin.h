#ifndef __AGIP_USER_LOGIN_H__
#define __AGIP_USER_LOGIN_H__
#include "SysProtocol.h"

class Engine_Export AGIPUserLogin :
    public SysProtocol
{
public:
    enum
    {
        PASSWORD_TYPE_AUTHENTICATION = 1,
        PASSWORD_TYPE_SECURITY       = 2
    };

    AGIPUserLogin(void);

    int getUserName(char *strUserName);
    int getPassword(char *strPassword);
    int getPasswordType(int8_t *pcPasswordType);
    int getUserPort(uint16_t *pusUserPort);
    int getUserIP(uint32_t *punUserIP);

    int setUserName(const char *strUserName);
    int setPassword(const char *strPassword);
    int setPasswordType(int8_t cPasswordType);
    int setUserPort(uint16_t usUserPort);
    int setUserIP(uint32_t unUserIP);

    virtual int showInfo();

public:
    virtual ~AGIPUserLogin(void);
};

class Engine_Export AGIPUserLoginRes :
    public SysProtocol
{
    friend class AGIPUserLogin;
public:
    AGIPUserLoginRes(void);

    int getResultCode(int32_t *pnResultCode);
    int getUserID(uint32_t *punUserID);
    int getUserType(int8_t *pcUserType);
    int getAdultFlag(int8_t *pcAdultFlag);
    int getUserClass(int8_t *pcUserClass);
    int getUserRoleCount(int8_t *pcUserRoleCount);
    int getUserPoint(int32_t *pnUserPoint);
    int getPromoterID(uint32_t *punPromoterID);
    int getUserFlag(uint32_t *punUserFlag);

    int setResultCode(int32_t nResultCode);
    int setUserID(uint32_t unUserID);
    int setUserType(int8_t cUserType);
    int setAdultFlag(int8_t cAdultFlag);
    int setUserClass(int8_t cUserClass);
    int setUserRoleCount(int8_t cUserRoleCount);
    int setUserPoint(int32_t nUserPoint);
    int setPromoterID(uint32_t unPromoterID);
    int setUserFlag(uint32_t unUserFlag);

    virtual int showInfo();
    
public:
    virtual ~AGIPUserLoginRes(void);
};

#endif
