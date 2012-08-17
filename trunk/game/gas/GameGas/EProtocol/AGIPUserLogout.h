#ifndef __AGIP_USER_LOGOUT_H__
#define __AGIP_USER_LOGOUT_H__
#include "SysProtocol.h"

class Engine_Export AGIPUserLogout :
    public SysProtocol
{
    
public:
    enum
    {
        LOGOUT_FLAG_PAYMENT_AND_LOGOUT = 1,
        LOGOUT_FLAG_PAYMENT_ONLY       = 2,

        FEE_FLAG_FREE = 0,
        FEE_FLAG_NOT_FREE = 1,

        FEE_FACTOR_NORMAL = 1

    };
    AGIPUserLogout(void);
    
    int getUserID(uint32_t *punUserID);
    int getRoleID(uint32_t *punRoleID);
    int getRatingID(uint32_t *punRatingID);
    int getLogoutFlag(int8_t *pcLogoutFlag);
    int getRoleLevel(int8_t *pcRoleLevel);
    int getOccupation(int8_t *pcOccupation);
    int getMoney1(uint32_t *punMoney1);  // 获取金钱数 1
    int getMoney2(uint32_t *punMoney2);  // 获取金钱数 2
    int getExperience(uint64_t *pullExperience); // 获取当前的经验数。


    int setUserID(uint32_t unUserID);
    int setRoleID(uint32_t unRoleID);
    int setRatingID(uint32_t unRatingID);
    int setLogoutFlag(int8_t cLogoutFlag);
    int setRoleLevel(int8_t cRoleLevel);
    int setOccupation(int8_t cOccupation);

    int setMoney1(uint32_t unMoney1);  // 设置金钱数 1
    int setMoney2(uint32_t unMoney2);  // 设置金钱数 2
    int setExperience(uint64_t ullExperience); // 设置当前的经验数。

public:
    virtual ~AGIPUserLogout(void);
};

class Engine_Export AGIPUserLogoutRes :
    public SysProtocol
{
    friend class AGIPUserLogout;
public:
    AGIPUserLogoutRes(void);
    virtual int initialize();

    int getResultCode(int32_t *pnResultCode);
    int getBalanceCount(int32_t *pnBalanceCount);
    int getBalanceInfo(int32_t nIndex, int32_t *pnSubjectID, int32_t *pnAmount);
    int getBalanceInfo(int32_t nIndex, BALANCE_INFO *pBalanceInfo);

    int setResultCode(int32_t nResultCode);

    int addBalanceInfo(int32_t nSubjectID, int32_t nAmount);
    int addBalanceInfo(const BALANCE_INFO *pBalanceInfo);

public:

    virtual ~AGIPUserLogoutRes(void);
private:
    int setBalanceCount(int32_t nBalanceCount);
};

#endif
