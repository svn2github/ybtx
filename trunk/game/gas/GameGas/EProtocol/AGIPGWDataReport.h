#ifndef __AGIP_GW_DATA_REPORT_H__
#define __AGIP_GW_DATA_REPORT_H__
#include "SysProtocol.h"

class Engine_Export AGIPGWDataReport :
    public SysProtocol
{
public:
    AGIPGWDataReport(void);
public:
    virtual ~AGIPGWDataReport(void);

    virtual int initialize();

    int getGatewayID(uint32_t *punGatewayID);
    int getServerID(uint32_t *punServerID);
    int getDataCount(int32_t *pnDataCount);
    int getDataInfo(int32_t nIndex, DATA_INFO *pDataInfo);
    int getDataInfo(int32_t nIndex, int32_t *pnDataType, int32_t *pnDataValue);

    int setGatewayID(uint32_t unGatewayID);
    int setServerID(uint32_t unServerID);
    int addDataInfo(const DATA_INFO *pDataInfo);
    int addDataInfo(int32_t nDataType, int32_t nDataValue);

    virtual int showInfo();
private:
    int setDataCount(int32_t nDataCount);
};

class Engine_Export AGIPGWDataReportRes :
    public SysProtocol
{
    friend class AGIPGWDataReport;
public:
    AGIPGWDataReportRes(void);
public:
    virtual ~AGIPGWDataReportRes(void);

    int getResultCode(int32_t *pnResultCode);

    int setResultCode(int32_t nResultCode);

    virtual int showInfo();
};

#endif
