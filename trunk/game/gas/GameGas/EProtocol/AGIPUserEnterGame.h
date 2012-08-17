#ifndef __AGIP_USER_ENTER_GAME_H__
#define __AGIP_USER_ENTER_GAME_H__
#include "SysProtocol.h"

class Engine_Export AGIPUserEnterGame :
    public SysProtocol
{
public:
    AGIPUserEnterGame(void);

    int getUserID(uint32_t *punUserID);
    int getRoleID(uint32_t *punRoleID);
    int getServerID(uint32_t *punServerID);
    int getRatingID(uint32_t *punRatingID);
    int getRoleLevel(int8_t *pcRoleLevel);
    int getUserPort(uint16_t *pusUserPort);
    int getUserIP(uint32_t *punUserIP);

    
    int setUserID(uint32_t unUserID);
    int setRoleID(uint32_t unRoleID);
    int setServerID(uint32_t unServerID);
    int setRatingID(uint32_t unRatingID);
    int setRoleLevel(int8_t cRoleLevel);
    int setUserPort(uint16_t usUserPort);
    int setUserIP(uint32_t unUserIP);

    int showInfo();

public:
    virtual ~AGIPUserEnterGame(void);
};

class Engine_Export AGIPUserEnterGameRes :
    public SysProtocol
{
    friend class AGIPUserEnterGame;
public:
    AGIPUserEnterGameRes(void);
    
    virtual int initialize();

    int getResultCode(int32_t *pnResultCode);
    int getBalanceCount(int32_t *pnBalanceCount);
    int getBalanceInfo(int32_t nIndex, int32_t *pnSubjectID, int32_t *pnAmount);
    int getBalanceInfo(int32_t nIndex, BALANCE_INFO *pBalanceInfo);

    int setResultCode(int32_t nResultCode);
    int addBalanceInfo(int32_t nSubjectID, int32_t nAmount);
    int addBalanceInfo(const BALANCE_INFO *pBalanceInfo);

    int showInfo();
public:
    virtual ~AGIPUserEnterGameRes(void);
private:
    int setBalanceCount(int32_t nBalanceCount);
};
#endif
