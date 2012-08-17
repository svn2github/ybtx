#ifndef __AGIP_USER_DRAW_BALANCE_H__
#define __AGIP_USER_DRAW_BALANCE_H__

#include "SysProtocol.h"

class Engine_Export AGIPUserDrawBalance :
    public SysProtocol
{
public:
    AGIPUserDrawBalance(void);
public:
    virtual ~AGIPUserDrawBalance(void);

    int getUserID(uint32_t *punUserID);
    int getRoleID(uint32_t *punRoleID);
    int getRatingID(uint32_t *punRatingID);
    int getSubjectID(int32_t *pnSubjectID);
    int getAmount(int32_t *pnAmount);

    int setUserID(uint32_t unUserID);
    int setRoleID(uint32_t unRoleID);
    int setRatingID(uint32_t unRatingID);
    int setSubjectID(int32_t nSubjectID);
    int setAmount(int32_t nAmount);
};


class Engine_Export AGIPUserDrawBalanceRes :
    public SysProtocol
{
public:
    AGIPUserDrawBalanceRes(void);
public:
    virtual ~AGIPUserDrawBalanceRes(void);

    int getResultCode(int32_t *pnResultCode); 
    int getAmount(int32_t *pnAmount);
    int getLeftAmount(int32_t *pnLeftAmount);

    int setResultCode(int32_t nResultCode); 
    int setAmount(int32_t nAmount);
    int setLeftAmount(int32_t nLeftAmount);
};

#endif
