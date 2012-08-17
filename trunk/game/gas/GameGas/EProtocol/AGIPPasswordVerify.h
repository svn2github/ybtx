#ifndef __AGIP_PASSWORD_VERIFY_H__
#define __AGIP_PASSWORD_VERIFY_H__
#include "SysProtocol.h"

class Engine_Export AGIPPasswordVerify :
    public SysProtocol
{
public:
    enum
    {
        PASSWORD_TYPE_AUTHENTICATION = 1,
        PASSWORD_TYPE_SECURITY       = 2
    };
    AGIPPasswordVerify(void);

    int getUserID(uint32_t *punUserID);
    int getPassword(char *strPassword);
    int getPasswordType(int8_t *pcPasswordType);

    int setUserID(uint32_t unUserID);
    int setPassword(const char *strPassword);
    int setPasswordType(int8_t cPasswordType);
    
    virtual int showInfo();

public:
    virtual ~AGIPPasswordVerify(void);
};

class Engine_Export AGIPPasswordVerifyRes :
    public SysProtocol
{
    friend class AGIPPasswordVerify;
public:
    AGIPPasswordVerifyRes(void);

    int getResultCode(int32_t *pnResultCode);
    int setResultCode(int32_t nResultCode);

    virtual int showInfo();
public:
    virtual ~AGIPPasswordVerifyRes(void);
};

#endif

