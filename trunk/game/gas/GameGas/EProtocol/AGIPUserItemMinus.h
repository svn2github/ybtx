#ifndef __AGIP_USER_ITEM_MINUS_H__
#define __AGIP_USER_ITEM_MINUS_H__
#include "SysProtocol.h"

class Engine_Export AGIPUserItemMinus :
    public SysProtocol
{
public:
    AGIPUserItemMinus(void);
public:
    virtual ~AGIPUserItemMinus(void);

    int getUserID(uint32_t *punUserID);
    int getRoleID(uint32_t *punRoleID);
    int getActivityID(uint32_t *punActivityID);
    int getItemCode(char *strItemCode);
    int getItemNum(int32_t *pnItemNum);
    int getItemGatewayID(uint32_t *punItemGatewayID);

    int setUserID(uint32_t unUserID);
    int setRoleID(uint32_t unRoleID);
    int setActivityID(uint32_t unActivityID);
    int setItemCode(const char *strItemCode);
    int setItemNum(int32_t nItemNum);
    int setItemGatewayID(uint32_t unItemGatewayID);
};

class Engine_Export AGIPUserItemMinusRes :
    public SysProtocol
{
    friend class AGIPUserItemAdd;
public:
    AGIPUserItemMinusRes(void);
public:
    virtual ~AGIPUserItemMinusRes(void);

    int getResultCode(int32_t *pnResultCode);
    int setResultCode(int32_t nResultCode);
};

#endif
