#ifndef __AGIP_USER_USE_PRODUCT_H__
#define __AGIP_USER_USE_PRODUCT_H__
#include "SysProtocol.h"

class Engine_Export AGIPUserUseProduct :
    public SysProtocol
{
public:
    
    AGIPUserUseProduct(void);
public:
    virtual ~AGIPUserUseProduct(void);

    int getUserID(uint32_t *punUserID);
    int getRoleID(uint32_t *punRoleID);
    int getPurchaseID(uint32_t *punPurchaseID);
    
    int setUserID(uint32_t unUserID);
    int setRoleID(uint32_t unRoleID);
    int setPurchaseID(uint32_t unPurchaseID);

    virtual int showInfo();
};

class Engine_Export AGIPUserUseProductRes :
    public SysProtocol
{
public:
    AGIPUserUseProductRes(void);
public:
    virtual ~AGIPUserUseProductRes(void);
    int getResultCode(int32_t *pnResultCode);

    int setResultCode(int32_t nResultCode);

    virtual int showInfo();
};

#endif
