#include "AGIPBind.h"

AGIPBind::AGIPBind(void)
: SysProtocol(AGIP_CURRENT_VERSION, CMD_BIND, sizeof(SAGIPBind))
{
    
}

AGIPBind::~AGIPBind(void)
{
}


int AGIPBind::getGatewayCode(char *strGatewayCode)
{
    ASSERT(strGatewayCode != NULL);
    SAGIPBind *pSAGIPBind = (SAGIPBind *)m_pucPDU;
    strncpy(strGatewayCode, pSAGIPBind->str_Gateway_Code, AGIP_GATEWAY_CODE_LEN);
    return S_SUCCESS;
}

int AGIPBind::getPassword(char *strPassword)
{
    ASSERT(strPassword != NULL);
    SAGIPBind *pSAGIPBind = (SAGIPBind *)m_pucPDU;
    strncpy(strPassword, pSAGIPBind->str_Password, AGIP_PASSWORD_LEN);
    return S_SUCCESS;
}

int AGIPBind::getMAC(char *strMAC)
{
    ASSERT(strMAC != NULL);
    SAGIPBind *pSAGIPBind = (SAGIPBind *)m_pucPDU;
    snprintf(
        strMAC,
        18,
        "%02X-%02X-%02X-%02X-%02X-%02X",
        pSAGIPBind->uc_MAC[0],
        pSAGIPBind->uc_MAC[1],
        pSAGIPBind->uc_MAC[2],
        pSAGIPBind->uc_MAC[3],
        pSAGIPBind->uc_MAC[4],
        pSAGIPBind->uc_MAC[5]
    );
    return S_SUCCESS;
}

int AGIPBind::getReconnectFlag(int8_t *pcReconnectFlag)
{
    ASSERT(pcReconnectFlag != NULL);
    SAGIPBind *pSAGIPBind = (SAGIPBind *)m_pucPDU;
    *pcReconnectFlag = pSAGIPBind->c_Reconnect_Flag;
    return S_SUCCESS;
}

int AGIPBind::getServerID(uint32_t *punServerID)
{
    ASSERT(punServerID != NULL);
    SAGIPBind *pSAGIPBind = (SAGIPBind *)m_pucPDU;
    *punServerID = htonl(pSAGIPBind->un_Server_ID);
    return S_SUCCESS;
}

/////////////////////////
// Setter
int AGIPBind::setGatewayCode(const char *pcGatewayCode)
{
    ASSERT(pcGatewayCode != NULL);
    SAGIPBind *pSAGIPBind = (SAGIPBind *)m_pucPDU;
    strncpy(pSAGIPBind->str_Gateway_Code, pcGatewayCode, AGIP_GATEWAY_CODE_LEN);
    return S_SUCCESS;
}

int AGIPBind::setPassword(const char *pcPassword)
{
    ASSERT(pcPassword != NULL);
    SAGIPBind *pSAGIPBind = (SAGIPBind *)m_pucPDU;
    strncpy(pSAGIPBind->str_Password, pcPassword, AGIP_PASSWORD_LEN);
    return S_SUCCESS;
}

int AGIPBind::setMAC(const uint8_t arrucMAC[6])
{
    ASSERT(arrucMAC != NULL);
    SAGIPBind *pSAGIPBind = (SAGIPBind *)m_pucPDU;
    memcpy(pSAGIPBind->uc_MAC, arrucMAC, AGIP_MAC_LEN);
    return S_SUCCESS;
}

int AGIPBind::setReconnectFlag(int8_t cReconnectFlag)
{
    SAGIPBind *pSAGIPBind = (SAGIPBind *)m_pucPDU;
    pSAGIPBind->c_Reconnect_Flag = cReconnectFlag;
    return S_SUCCESS;
}

int AGIPBind::setServerID(uint32_t unServerID)
{
    SAGIPBind *pSAGIPBind = (SAGIPBind *)m_pucPDU;
    pSAGIPBind->un_Server_ID = htonl(unServerID);
    return S_SUCCESS;
}

int AGIPBind::showInfo()
{
    int nRetCode        = E_ERROR;
    char strGatewayCode[32];
    char strPassword[32];
    char strMAC[32];
    int8_t cReconnectFlag = 0;

    SysProtocol::showInfo();
    nRetCode = getGatewayCode(strGatewayCode);
    nRetCode = getPassword(strPassword);
    nRetCode = getMAC(strMAC);
    nRetCode = getReconnectFlag(&cReconnectFlag);
    
    strGatewayCode[sizeof(strGatewayCode) - 1] = '\0';
    strPassword[sizeof(strPassword) - 1] = '\0';
    strMAC[sizeof(strMAC) - 1] = '\0';
    printf("--------------------------------------------------------AGIPBind\n");
    printf("Gateway_Code:\t%s\n", strGatewayCode);
    printf("Password:\t%s\n", strPassword);
    printf("MAC:\t%s\n", strMAC);
    switch(cReconnectFlag )
    {
    case RECONNECT_FLAG_FIRST_CONNECTION:
        printf("Reconnect_Flag:\t%d(First Connection)\n", cReconnectFlag);
        break;
    case RECONNECT_FLAG_RECONNECTION:
        printf("Reconnect_Flag:\t%d(Reconnection)\n", cReconnectFlag);
        break;
    default:
        printf("Reconnect_Flag:\t%d(INVALID)\n", cReconnectFlag);
        break;
    }
    printf("Gateway_Code:\t%s\n", strGatewayCode);
    printf("--------------------------------------------------------AGIPBind\n");
    return S_SUCCESS;
}


//////////////////////////
/// AGIPBindRes
    
AGIPBindRes::AGIPBindRes(void)
: SysProtocol(AGIP_CURRENT_VERSION, CMD_BIND_RES, sizeof(SAGIPBindRes))
{

}

AGIPBindRes::~AGIPBindRes(void)
{

}

int AGIPBindRes::getResultCode(int32_t *pnResultCode)
{
    ASSERT(pnResultCode != NULL);
    SAGIPBindRes *pSAGIPBindRes = (SAGIPBindRes *)m_pucPDU;
    *pnResultCode = htonl(pSAGIPBindRes->n_Result_Code);
    return S_SUCCESS;
}

int AGIPBindRes::getGatewayID(uint32_t *punGatewayID)
{
    ASSERT(punGatewayID != NULL);
    SAGIPBindRes *pSAGIPBindRes = (SAGIPBindRes *)m_pucPDU;
    *punGatewayID = htonl(pSAGIPBindRes->un_Gateway_ID);
    return S_SUCCESS;
}

int AGIPBindRes::setResultCode(int32_t nResultCode)
{
    SAGIPBindRes *pSAGIPBindRes = (SAGIPBindRes *)m_pucPDU;
    pSAGIPBindRes->n_Result_Code = htonl(nResultCode);
    return S_SUCCESS;
}

int AGIPBindRes::setGatewayID(uint32_t unGatewayID)
{
    SAGIPBindRes *pSAGIPBindRes = (SAGIPBindRes *)m_pucPDU;
    pSAGIPBindRes->un_Gateway_ID = htonl(unGatewayID);
    return S_SUCCESS;
}

int AGIPBindRes::showInfo()
{
    int nRetCode        = E_ERROR;
    
    int32_t  nResultCode = 0;
    uint32_t unGatewayID = 0;

    SysProtocol::showInfo();
    nRetCode = getResultCode(&nResultCode);
    nRetCode = getGatewayID(&unGatewayID);

    printf("--------------------------------------------------------AGIPBindRes\n");
    printf("Result_Code:\t%d\n", nResultCode);
    printf("Gateway_ID:\t%u\n",  unGatewayID);
    printf("--------------------------------------------------------AGIPBindRes\n");
    return S_SUCCESS;
}
