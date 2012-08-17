#ifndef __AGIP_BIND_H__
#define __AGIP_BIND_H__

#include "SysProtocol.h"

class Engine_Export AGIPBind :
    public SysProtocol
{
public:
    AGIPBind(void);

public:
    virtual ~AGIPBind(void);
    enum
    {
        RECONNECT_FLAG_FIRST_CONNECTION = 0,
        RECONNECT_FLAG_RECONNECTION     = 1
    };

    int getGatewayCode(char *strGatewayCode);
    int getPassword(char *strPassword);
    int getMAC(char *strMAC);
    int getReconnectFlag(int8_t *pcReconnectFlag);
    int getServerID(uint32_t *punServerID);

    int setGatewayCode(const char *strGatewayCode);
    int setPassword(const char *strPassword);
    int setMAC(const uint8_t arrucMAC[6]);
    int setReconnectFlag(int8_t cReconnectFlag);
    int setServerID(uint32_t unServerID);

    virtual int showInfo();
};


class Engine_Export AGIPBindRes :
    public SysProtocol
{
    friend class AGIPBind;
public:
    AGIPBindRes(void);

public:
    virtual ~AGIPBindRes(void);
    
    int getResultCode(int32_t *pnResultCode);
    int getGatewayID(uint32_t *punGatewayID);

    int setResultCode(int32_t nResultCode);
    int setGatewayID(uint32_t unGatewayID);

    virtual int showInfo();
};

#endif
