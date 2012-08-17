#ifndef __AGIP_USER_ITEM_LIST_H__
#define __AGIP_USER_ITEM_LIST_H__
#include "SysProtocol.h"

class Engine_Export AGIPUserItemList :
    public SysProtocol
{
public:
    AGIPUserItemList(void);
public:
    virtual ~AGIPUserItemList(void);
    int getUserID(uint32_t *punUserID);
    int getRoleID(uint32_t *punRoleID);
    int getActivityID(uint32_t *punActivityID);
    int getItemGatewayID(uint32_t *punItemGatewayID);

    int setUserID(uint32_t unUserID);
    int setRoleID(uint32_t unRoleID);
    int setActivityID(uint32_t unActivityID);
    int setItemGatewayID(uint32_t unItemGatewayID);

    virtual int showInfo();
};

class Engine_Export AGIPUserItemListRes :
    public SysProtocol
{
    friend class AGIPUserItemList;
public:
    AGIPUserItemListRes(void);
    virtual int initialize();
public:
    virtual ~AGIPUserItemListRes(void);

    int getResultCode(int32_t *pnResultCode);
    int getItemCount(int32_t *pnItemCount);
    int getItemInfo(int32_t nIndex, char *strItemCode, int32_t *pnItemNum);

    int setResultCode(int32_t nResultCode);
    int addItemInfo(const char *strItemCode, int32_t nItemNum);

    virtual int showInfo();
private:
    //int setItemCount(int32_t nItemCount);
};
#endif
