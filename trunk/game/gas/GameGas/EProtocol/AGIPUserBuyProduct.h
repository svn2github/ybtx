#ifndef __AGIP_USER_BUY_PRODUCT_H__
#define __AGIP_USER_BUY_PRODUCT_H__
#include "SysProtocol.h"

class Engine_Export AGIPUserBuyProduct :
    public SysProtocol
{
public:
    enum
    {
        PACKAGE_FLAG_NORMAL      = 1,
        PACKAGE_FLAG_COMBINATION = 2

    };
    AGIPUserBuyProduct(void);
    virtual ~AGIPUserBuyProduct(void);
public:

    //virtual int initialize();

    int getUserID(uint32_t *punUserID);
    int getRoleID(uint32_t *punRoleID);
    int getIBCode(char *strIBCode);
    int getPackageFlag(int8_t *pcPackageFlag);
    int getAmount(int16_t *psAmount);
    int getRatingID(uint32_t *punRatingID);
    int getServerID(uint32_t *punServerID);
    int getUserIP(uint32_t *punUserIP);
    int getSubjectID(int32_t *pnSubjectID);
    int getPrice(int32_t *pnPrice);
    int getDiscountPrice(int32_t *pnDiscountPrice);

    int setUserID(uint32_t unUserID);
    int setRoleID(uint32_t unRoleID);
    int setIBCode(char *strIBCode);
    int setPackageFlag(int8_t cPackageFlag);
    int setAmount(int16_t sAmount);
    int setRatingID(uint32_t unRatingID);
    int setServerID(uint32_t unServerID);
    int setUserIP(uint32_t unUserIP);
    int setSubjectID(int32_t nSubjectID);
    int setPrice(int32_t nPrice);
    int setDiscountPrice(int32_t nDiscountPrice);

    virtual int showInfo();
private:

};

class Engine_Export AGIPUserBuyProductRes :
    public SysProtocol
{
    friend class AGIPUserBuyProduct;
public:
    AGIPUserBuyProductRes(void);
    virtual ~AGIPUserBuyProductRes(void);
public:
    virtual int initialize();

    int getResultCode(int32_t *pnResultCode);
    int getCostMoney(int32_t *pnCostMoney);
    int getRemainMoney(int32_t *pnRemainMoney);
    int getIBCount(int32_t *pnIBCount);
    //int getPurchaseID(uint64_t *pullPurchaseID);
    int getPurchaseID(int32_t nIndex, uint64_t *pullPurchaseID);
    int getPurchaseID(int32_t nIndex, PURCHASE_ID *pPurchaseID);
    int getPurchaseID(int32_t nIndex, uint32_t *punPurchaseIDHigher, uint32_t *punPurchaseIDLower);

    int setResultCode(int32_t nResultCode);
    int setCostMoney(int32_t nCostMoney);
    int setRemainMoney(int32_t nRemainMoney);
    
    int addPurchaseID(uint64_t ullPurchaseID);
    int addPurchaseID(PURCHASE_ID *pPurchaseID);
    int addPurchaseID(uint32_t unPurchaseIDHigher, uint32_t unPurchaseIDLower);

    //int setPurchaseID(uint64_t ullPurchaseID);
    virtual int showInfo();
private:
    int setIBCount(int32_t nIBCount);
    
};




#endif
