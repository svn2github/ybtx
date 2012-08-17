#ifndef __AGIP_USER_DATA_REPORT_H__
#define __AGIP_USER_DATA_REPORT_H__
#include "SysProtocol.h"

class Engine_Export AGIPUserDataReport :
    public SysProtocol
{
public:
    AGIPUserDataReport(void);
    virtual ~AGIPUserDataReport(void);

public:
    virtual int initialize();

    int getUserID(uint32_t *punUserID);
    int getRoleID(uint32_t *punRoleID);
    int getDataCount(int32_t *pnDataCount);
    int getDataInfo(int32_t nIndex, DATA_INFO *pDataInfo);
    int getDataInfo(int32_t nIndex, int32_t *pnDataType, int32_t *pnDataValue);

    int setUserID(uint32_t unUserID);
    int setRoleID(uint32_t unRoleID);
    int addDataInfo(const DATA_INFO *pDataInfo);
    int addDataInfo(int32_t nDataType, int32_t nDataValue);
private:
    int setDataCount(int32_t nDataCount);
};

class Engine_Export AGIPUserDataReportRes :
    public SysProtocol
{
    friend class AGIPUserDataReport;
public:
    AGIPUserDataReportRes(void);
public:
    virtual ~AGIPUserDataReportRes(void);

    int getResultCode(int32_t *pnResultCode);

    int setResultCode(int32_t nResultCode);
};

#endif
