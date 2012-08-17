#ifndef __AGIP_USER_ITEM_INFO__
#define __AGIP_USER_ITEM_INFO__
#include "SysProtocol.h"

class Engine_Export AGIPUserItemInfo :
    public SysProtocol
{
public:
    AGIPUserItemInfo(void);
public:
    virtual ~AGIPUserItemInfo(void);

    int getUserID(uint32_t *punUserID);
    int getRoleID(uint32_t *punRoleID);
    int getActivityID(uint32_t *punActivityID);
    int getItemCode(char *strItemCode);
    int getItemGatewayID(uint32_t *punItemGatewayID);

    int setUserID(uint32_t unUserID);
    int setRoleID(uint32_t unRoleID);
    int setActivityID(uint32_t unActivityID);
    int setItemCode(const char *strItemCode);
    int setItemGatewayID(uint32_t unItemGatewayID);
};

class Engine_Export AGIPUserItemInfoRes :
    public SysProtocol
{
    friend class AGIPUserItemInfo;
public:
    AGIPUserItemInfoRes(void);
public:
    virtual ~AGIPUserItemInfoRes(void);

    int getResultCode(int32_t *pnResultCode);
    int getItemNum(int32_t *pnItemNum);
    int getBeginTime(int32_t *pnBeginTime);
    int getEndTime(int32_t *pnEndTime);
    
    int setResultCode(int32_t nResultCode);
    int setItemNum(int32_t nItemNum);
    int setBeginTime(int32_t nBeginTime);
    int setEndTime(int32_t nEndTime);
};

#endif
