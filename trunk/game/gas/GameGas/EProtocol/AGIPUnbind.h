#ifndef _AGIP_UNBIND_H__
#define _AGIP_UNBIND_H__
#include "SysProtocol.h"

class Engine_Export AGIPUnbind :
    public SysProtocol
{
public:
    AGIPUnbind(void);
    
    int getGatewayID(uint32_t *punGatewayID);

    int setGatewayID(uint32_t unGatewayID);

    virtual int showInfo();
public:
    virtual ~AGIPUnbind(void);
};

class Engine_Export AGIPUnbindRes :
    public SysProtocol
{
    friend class AGIPUnbind;
public:
    AGIPUnbindRes(void);

public:
    virtual ~AGIPUnbindRes(void);

    int getResultCode(int32_t *pnResultCode);
    int setResultCode(int32_t nResultCode);
    virtual int showInfo();
};


#endif

