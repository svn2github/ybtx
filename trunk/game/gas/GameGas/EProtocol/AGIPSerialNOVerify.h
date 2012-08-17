#ifndef __AGIP_SERIALNO_H__
#define __AGIP_SERIALNO_H__
#include "SysProtocol.h"

class Engine_Export AGIPSerialNOVerify :
    public SysProtocol
{
public:
    AGIPSerialNOVerify(void);
    
    enum
    {
        SERIALNO_TYPE_CDKEY = 1
    };
    int getUserID(uint32_t *punUserID);
    int getSerialNO(char *strSerialNO);
    int getSerialType(int8_t *pcSerialType);

    int setUserID(uint32_t unUserID);
    int setSerialNO(const char *strSerialNO);
    int setSerialType(int8_t cSerialType);

    virtual int showInfo();

public:
    virtual ~AGIPSerialNOVerify(void);
};

class Engine_Export AGIPSerialNOVerifyRes :
    public SysProtocol
{
    friend class AGIPSerialNOVerify;
public:
    AGIPSerialNOVerifyRes(void);

    int getResultCode(int32_t *pnResultCode);

    int setResultCode(int32_t nResultCode);
    virtual int showInfo();

public:
    virtual ~AGIPSerialNOVerifyRes(void);
};

#endif
