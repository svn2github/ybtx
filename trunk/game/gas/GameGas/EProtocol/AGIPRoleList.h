#ifndef __AGIP_ROLE_LIST_H__
#define __AGIP_ROLE_LIST_H__
#include "SysProtocol.h"

class Engine_Export AGIPRoleList :
    public SysProtocol
{
public:
    AGIPRoleList(void);
    
    int getUserID(uint32_t *punUserID);

    int setUserID(uint32_t unUserID);
    virtual int showInfo();

public:
    virtual ~AGIPRoleList(void);
};

class Engine_Export AGIPRoleListRes :
    public SysProtocol
{
    friend class AGIPRoleList;
public:
    AGIPRoleListRes(void);
    virtual ~AGIPRoleListRes(void);

    virtual int initialize();
public:
    int getResultCode(int32_t *pnResultCode);
    int getRoleCount(int32_t *pnRoleCount);
    int getRoleInfo(int32_t nIndex, uint32_t *punRoleID, char *strRoleName);
    int getRoleInfo(int32_t nIndex, ROLE_INFO *pRoleInfo);


    int setResultCode(int32_t nResultCode);
    int addRoleInfo(uint32_t unRoleID, const char *strRoleName);
    int addRoleInfo(const ROLE_INFO *pRoleInfo);

    virtual int showInfo();
private:
    int setRoleCount(int32_t nRoleCount);
};


#endif

