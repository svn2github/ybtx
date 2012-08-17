#ifndef __AGIP_HANDSET_H__
#define __AGIP_HANDSET_H__
#include "SysProtocol.h"

class Engine_Export AGIPHandset :
    public SysProtocol
{
public:
    AGIPHandset(void);
public:
    virtual ~AGIPHandset(void);
};


class Engine_Export AGIPHandsetRes :
    public SysProtocol
{
public:
    AGIPHandsetRes(void);
public:
    virtual ~AGIPHandsetRes(void);
};

#endif
