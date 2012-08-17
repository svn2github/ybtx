#ifndef __AGIP_USER_BALANCE_INFO_H__
#define __AGIP_USER_BALANCE_INFO_H__
#include "SysProtocol.h"

class Engine_Export AGIPUserBalanceInfo :
    public SysProtocol
{
public:
    AGIPUserBalanceInfo(void);
public:
    virtual ~AGIPUserBalanceInfo(void);
public:
    int getUserID(uint32_t *punUserID); 
    int getRoleID(uint32_t *punRoleID);
    int getRatingID(uint32_t *punRatingID);

    int setUserID(uint32_t unUserID); 
    int setRoleID(uint32_t unRoleID);
    int setRatingID(uint32_t unRatingID);
};

class Engine_Export AGIPUserBalanceInfoRes :
    public SysProtocol
{
    friend class AGIPUserBalanceInfo;
public:
    AGIPUserBalanceInfoRes(void);
public:
    virtual ~AGIPUserBalanceInfoRes(void);

    virtual int initialize();
public:
    int getResultCode(int32_t *pnResultCode); 
    int getBalanceCount(int32_t *pnBalanceCount);
    int getBalanceInfo(int32_t nIndex, int32_t *pnSubjectID, int32_t *pnAmount, int32_t *pnLeftAmount);
    int getBalanceInfo(int32_t nIndex, ALL_BALANCE_INFO *pAllBalanceInfo);

    int setResultCode(int32_t nResultCode); 
    int addBalanceInfo(int32_t nSubjectID, int32_t nAmount, int32_t nLeftAmount);
    int addBalanceInfo(const ALL_BALANCE_INFO *pAllBalanceInfo);
    int showInfo();
private:
    int setBalanceCount(int32_t nBalanceCount);
};


#endif

