#ifndef __AGIP_DELETE_ROLE_H__
#define __AGIP_DELETE_ROLE_H__
#include "SysProtocol.h"

class Engine_Export AGIPDeleteRole :
    public SysProtocol
{
public:
    AGIPDeleteRole(void);

    int getUserID(uint32_t *punUserID);
    int getRoleID(uint32_t *punRoleID);
    
    int setUserID(uint32_t unUserID);
    int setRoleID(uint32_t unRoleID);

    virtual int showInfo();

public:
    virtual ~AGIPDeleteRole(void);
};

class Engine_Export AGIPDeleteRoleRes :
    public SysProtocol
{
    friend class AGIPDeleteRole;
public:
    AGIPDeleteRoleRes(void);

    int getResultCode(int32_t *pnResultCode);

    int setResultCode(int32_t nResultCode);

    virtual int showInfo();

public:
    virtual ~AGIPDeleteRoleRes(void);
};

#endif

