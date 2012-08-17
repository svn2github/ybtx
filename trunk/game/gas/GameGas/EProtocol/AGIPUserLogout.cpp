#include "AGIPUserLogout.h"

AGIPUserLogout::AGIPUserLogout(void)
: SysProtocol(AGIP_CURRENT_VERSION, CMD_USER_LOGOUT, sizeof(SAGIPUserLogout))
{
}

AGIPUserLogout::~AGIPUserLogout(void)
{
}

int AGIPUserLogout::getUserID(uint32_t *punUserID)
{
    ASSERT(punUserID != NULL);
    SAGIPUserLogout *pSAGIPUserLogout = (SAGIPUserLogout *)m_pucPDU;
    *punUserID = htonl(pSAGIPUserLogout->un_User_ID);
    return S_SUCCESS;
}

int AGIPUserLogout::getRoleID(uint32_t *punRoleID)
{
    ASSERT(punRoleID != NULL);
    SAGIPUserLogout *pSAGIPUserLogout = (SAGIPUserLogout *)m_pucPDU;
    *punRoleID = htonl(pSAGIPUserLogout->un_Role_ID);
    return S_SUCCESS;
}

int AGIPUserLogout::getRatingID(uint32_t *punRatingID)
{
    ASSERT(punRatingID != NULL);
    SAGIPUserLogout *pSAGIPUserLogout = (SAGIPUserLogout *)m_pucPDU;
    *punRatingID = htonl(pSAGIPUserLogout->un_Rating_ID);
    return S_SUCCESS;
}

int AGIPUserLogout::getLogoutFlag(int8_t *pcLogoutFlag)
{
    ASSERT(pcLogoutFlag != NULL);
    SAGIPUserLogout *pSAGIPUserLogout = (SAGIPUserLogout *)m_pucPDU;
    *pcLogoutFlag = pSAGIPUserLogout->c_Logout_Flag;
    return S_SUCCESS;
}

int AGIPUserLogout::getRoleLevel(int8_t *pcRoleLevel)
{
    ASSERT(pcRoleLevel != NULL);
    SAGIPUserLogout *pSAGIPUserLogout = (SAGIPUserLogout *)m_pucPDU;
    *pcRoleLevel = pSAGIPUserLogout->c_Role_Level;
    return S_SUCCESS;
}

int AGIPUserLogout::getOccupation(int8_t *pcOccupation)
{
    ASSERT(pcOccupation != NULL);
    SAGIPUserLogout *pSAGIPUserLogout = (SAGIPUserLogout *)m_pucPDU;
    *pcOccupation = pSAGIPUserLogout->c_Occupation;
    return S_SUCCESS;
}

int AGIPUserLogout::getMoney1(uint32_t *punMoney1)
{
    ASSERT(punMoney1 != NULL);
    SAGIPUserLogout *pSAGIPUserLogout = (SAGIPUserLogout *)m_pucPDU;
    *punMoney1 = htonl(pSAGIPUserLogout->un_Money1);
    return S_SUCCESS;
}

int AGIPUserLogout::getMoney2(uint32_t *punMoney2)
{
    ASSERT(punMoney2 != NULL);
    SAGIPUserLogout *pSAGIPUserLogout = (SAGIPUserLogout *)m_pucPDU;
    *punMoney2 = htonl(pSAGIPUserLogout->un_Money2);
    return S_SUCCESS;
}

int AGIPUserLogout::getExperience(uint64_t *pullExperience)
{
    ASSERT(pullExperience != NULL);
    SAGIPUserLogout *pSAGIPUserLogout = (SAGIPUserLogout *)m_pucPDU;
    uint32_t *punExperience = (uint32_t *)pullExperience;
#if (defined(LINUX) || defined(WIN32))
    punExperience[1] = htonl(pSAGIPUserLogout->un_Experience_Higher);
    punExperience[0] = htonl(pSAGIPUserLogout->un_Experience_Lower);
#else
    punExperience[0] = pSAGIPUserLogout->un_Experience_Higher;
    punExperience[1] = pSAGIPUserLogout->un_Experience_Lower;
#endif
    
    return S_SUCCESS;
}
////////////////////
// Setter

int AGIPUserLogout::setUserID(uint32_t unUserID)
{
    SAGIPUserLogout *pSAGIPUserLogout = (SAGIPUserLogout *)m_pucPDU;
    pSAGIPUserLogout->un_User_ID = htonl(unUserID);
    return S_SUCCESS;
}

int AGIPUserLogout::setRoleID(uint32_t unRoleID)
{
    SAGIPUserLogout *pSAGIPUserLogout = (SAGIPUserLogout *)m_pucPDU;
    pSAGIPUserLogout->un_Role_ID = htonl(unRoleID);
    return S_SUCCESS;
}

int AGIPUserLogout::setRatingID(uint32_t unRatingID)
{
    SAGIPUserLogout *pSAGIPUserLogout = (SAGIPUserLogout *)m_pucPDU;
    pSAGIPUserLogout->un_Rating_ID = htonl(unRatingID);
    return S_SUCCESS;
}

int AGIPUserLogout::setLogoutFlag(int8_t cLogoutFlag)
{
    SAGIPUserLogout *pSAGIPUserLogout = (SAGIPUserLogout *)m_pucPDU;
    pSAGIPUserLogout->c_Logout_Flag = cLogoutFlag;
    return S_SUCCESS;
}

int AGIPUserLogout::setRoleLevel(int8_t cRoleLevel)
{
    SAGIPUserLogout *pSAGIPUserLogout = (SAGIPUserLogout *)m_pucPDU;
    pSAGIPUserLogout->c_Role_Level = cRoleLevel;
    return S_SUCCESS;
}

int AGIPUserLogout::setOccupation(int8_t cOccupation)
{
    SAGIPUserLogout *pSAGIPUserLogout = (SAGIPUserLogout *)m_pucPDU;
    pSAGIPUserLogout->c_Occupation = cOccupation;
    return S_SUCCESS;
}


int AGIPUserLogout::setMoney1(uint32_t unMoney1)
{
    SAGIPUserLogout *pSAGIPUserLogout = (SAGIPUserLogout *)m_pucPDU;
    pSAGIPUserLogout->un_Money1 = htonl(unMoney1);
    return S_SUCCESS;
}

int AGIPUserLogout::setMoney2(uint32_t unMoney2)
{
    SAGIPUserLogout *pSAGIPUserLogout = (SAGIPUserLogout *)m_pucPDU;
    pSAGIPUserLogout->un_Money2 = htonl(unMoney2);
    return S_SUCCESS;
}

int AGIPUserLogout::setExperience(uint64_t ullExperience)
{
    SAGIPUserLogout *pSAGIPUserLogout = (SAGIPUserLogout *)m_pucPDU;
    uint32_t *punExperience = (uint32_t *)&ullExperience;
#if (defined(LINUX) || defined(WIN32))
    pSAGIPUserLogout->un_Experience_Lower  = htonl(punExperience[0]);
    pSAGIPUserLogout->un_Experience_Higher = htonl(punExperience[1]);
#else
    pSAGIPUserLogout->un_Experience_Higher = punExperience[0];
    pSAGIPUserLogout->un_Experience_Lower = punExperience[1];
#endif

    return S_SUCCESS;
}

//////////////////////
// AGIPUserLogoutRes
AGIPUserLogoutRes::AGIPUserLogoutRes(void)
: SysProtocol(AGIP_CURRENT_VERSION, CMD_USER_LOGOUT_RES, sizeof(SAGIPUserLogoutRes) - sizeof(BALANCE_INFO))
{

}

AGIPUserLogoutRes::~AGIPUserLogoutRes(void)
{

}


int AGIPUserLogoutRes::initialize()
{
    int nRetCode = E_ERROR;
    
    m_usTotalLength = sizeof(SAGIPUserLogoutRes) - sizeof(BALANCE_INFO);
    nRetCode = SysProtocol::initialize();
    ASSERT(nRetCode == S_SUCCESS);
    
    return S_SUCCESS;
}

int AGIPUserLogoutRes::getResultCode(int32_t *pnResultCode)
{
    ASSERT(pnResultCode != NULL);
    SAGIPUserLogoutRes *pSAGIPUserLogoutRes = (SAGIPUserLogoutRes *)m_pucPDU;
    *pnResultCode = htonl(pSAGIPUserLogoutRes->n_Result_Code);
    return S_SUCCESS;
}

int AGIPUserLogoutRes::getBalanceCount(int32_t *pnBalanceCount)
{
    ASSERT(pnBalanceCount != NULL);
    SAGIPUserLogoutRes *pSAGIPUserLogoutRes = (SAGIPUserLogoutRes *)m_pucPDU;
    *pnBalanceCount = htonl(pSAGIPUserLogoutRes->n_Balance_Count);
    return S_SUCCESS;
}
int AGIPUserLogoutRes::getBalanceInfo(int32_t nIndex, int32_t *pnSubjectID, int32_t *pnAmount)
{
    int nResult  = E_PDU_INVALID_FIELD;
    int nRetCode = E_PDU_INVALID_FIELD;

    BALANCE_INFO *pBalanceInfoBuffer = NULL;
    int32_t nBalanceCount = 0;

    ASSERT(pnSubjectID != NULL && pnAmount != NULL);
    SAGIPUserLogoutRes *pSAGIPUserLogoutRes = (SAGIPUserLogoutRes *)m_pucPDU;
    nBalanceCount = htonl(pSAGIPUserLogoutRes->n_Balance_Count);
    if (
        (nIndex < 0) ||
        (nIndex >= nBalanceCount)
        )
    {
        goto ExitError;
    }
    pBalanceInfoBuffer = (BALANCE_INFO *)&pSAGIPUserLogoutRes->n_Subject_ID;
    pBalanceInfoBuffer += nIndex;

    *pnSubjectID = htonl(pBalanceInfoBuffer->nSubjectID);
    *pnAmount    = htonl(pBalanceInfoBuffer->nAmount);

    nResult = S_SUCCESS;
ExitError:
    return nResult;
}
int AGIPUserLogoutRes::getBalanceInfo(int32_t nIndex, BALANCE_INFO *pBalanceInfo)
{
    ASSERT(pBalanceInfo != NULL);
    return this->getBalanceInfo(nIndex, &pBalanceInfo->nSubjectID, &pBalanceInfo->nAmount);
}

int AGIPUserLogoutRes::setResultCode(int32_t nResultCode)
{
    SAGIPUserLogoutRes *pSAGIPUserLogoutRes = (SAGIPUserLogoutRes *)m_pucPDU;
    pSAGIPUserLogoutRes->n_Result_Code = htonl(nResultCode);
    return S_SUCCESS;
}

int AGIPUserLogoutRes::setBalanceCount(int32_t nBalanceCount)
{
    ASSERT(false && "Balance Count is calculated automatically. Pls use addBalanceInfo() method!!");
    return E_ERROR;
}

int AGIPUserLogoutRes::addBalanceInfo(int32_t nSubjectID, int32_t nAmount)
{
    int nResult  = E_PDU_INVALID_FIELD;
    int nRetCode = E_PDU_INVALID_FIELD;

    BALANCE_INFO *pBalanceInfoBuffer = NULL;
    int32_t nBalanceCount = 0;

    SAGIPUserLogoutRes *pSAGIPUserLogoutRes = (SAGIPUserLogoutRes *)m_pucPDU;
    nBalanceCount = htonl(pSAGIPUserLogoutRes->n_Balance_Count);

    pBalanceInfoBuffer = (BALANCE_INFO *)&pSAGIPUserLogoutRes->n_Subject_ID;
    pBalanceInfoBuffer += nBalanceCount;

    pBalanceInfoBuffer->nSubjectID = htonl(nSubjectID);
    pBalanceInfoBuffer->nAmount    = htonl(nAmount);

    nBalanceCount++;
    pSAGIPUserLogoutRes->n_Balance_Count = htonl(nBalanceCount);
    m_usTotalLength += sizeof(BALANCE_INFO);
    this->setTotalLength(m_usTotalLength);

    nResult = S_SUCCESS;
    //ExitError:
    return nResult;
}

int AGIPUserLogoutRes::addBalanceInfo(const BALANCE_INFO *pBalanceInfo)
{
    ASSERT(pBalanceInfo != NULL);
    return this->addBalanceInfo(pBalanceInfo->nSubjectID, pBalanceInfo->nAmount);
}


