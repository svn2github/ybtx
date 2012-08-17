#include "AGIPUserItemList.h"

AGIPUserItemList::AGIPUserItemList(void)
: SysProtocol(AGIP_CURRENT_VERSION, CMD_USER_ITEM_LIST, sizeof(SAGIPUserItemList))
{
}

AGIPUserItemList::~AGIPUserItemList(void)
{
}


int AGIPUserItemList::getUserID(uint32_t *punUserID)
{
    ASSERT(punUserID != NULL);
    SAGIPUserItemList *pSAGIPUserItemList = (SAGIPUserItemList *)m_pucPDU;
    *punUserID = htonl(pSAGIPUserItemList->un_User_ID);
    return S_SUCCESS;
}

int AGIPUserItemList::getRoleID(uint32_t *punRoleID)
{
    ASSERT(punRoleID != NULL);
    SAGIPUserItemList *pSAGIPUserItemList = (SAGIPUserItemList *)m_pucPDU;
    *punRoleID = htonl(pSAGIPUserItemList->un_Role_ID);
    return S_SUCCESS;
}

int AGIPUserItemList::getActivityID(uint32_t *punActivityID)
{
    ASSERT(punActivityID != NULL);
    SAGIPUserItemList *pSAGIPUserItemList = (SAGIPUserItemList *)m_pucPDU;
    *punActivityID = htonl(pSAGIPUserItemList->un_Activity_ID);
    return S_SUCCESS;
}

int AGIPUserItemList::getItemGatewayID(uint32_t *punItemGatewayID)
{
    ASSERT(punItemGatewayID != NULL);
    SAGIPUserItemList *pSAGIPUserItemList = (SAGIPUserItemList *)m_pucPDU;
    *punItemGatewayID = htonl(pSAGIPUserItemList->un_Item_Gateway_ID);
    return S_SUCCESS;
}

int AGIPUserItemList::setUserID(uint32_t unUserID)
{
    SAGIPUserItemList *pSAGIPUserItemList = (SAGIPUserItemList *)m_pucPDU;
    pSAGIPUserItemList->un_User_ID = htonl(unUserID);
    return S_SUCCESS;
}

int AGIPUserItemList::setRoleID(uint32_t unRoleID)
{
    SAGIPUserItemList *pSAGIPUserItemList = (SAGIPUserItemList *)m_pucPDU;
    pSAGIPUserItemList->un_Role_ID = htonl(unRoleID);
    return S_SUCCESS;
}

int AGIPUserItemList::setActivityID(uint32_t unActivityID)
{
    SAGIPUserItemList *pSAGIPUserItemList = (SAGIPUserItemList *)m_pucPDU;
    pSAGIPUserItemList->un_Activity_ID = htonl(unActivityID);
    return S_SUCCESS;
}

int AGIPUserItemList::setItemGatewayID(uint32_t unItemGatewayID)
{
    SAGIPUserItemList *pSAGIPUserItemList = (SAGIPUserItemList *)m_pucPDU;
    pSAGIPUserItemList->un_Item_Gateway_ID = htonl(unItemGatewayID);
    return S_SUCCESS;
}

int AGIPUserItemList::showInfo()
{
    int nRetCode        = E_ERROR;

    uint32_t  unUserID = 0;
    uint32_t  unRoleID = 0;
    uint32_t  unActivityID = 0;

    SysProtocol::showInfo();
    nRetCode = getUserID(&unUserID);
    nRetCode = getRoleID(&unRoleID);
    nRetCode = getActivityID(&unActivityID);
    printf("--------------------------------------------------------AGIPUserItemList\n");
    printf("User_ID:\t%u\n", unUserID);
    printf("Role_ID:\t%u\n", unRoleID);
    printf("Activity_ID:\t%u\n", unActivityID);
    printf("--------------------------------------------------------AGIPUserItemList\n");
    return S_SUCCESS;
}
///////////////////
//

AGIPUserItemListRes::AGIPUserItemListRes(void)
: SysProtocol(AGIP_CURRENT_VERSION, CMD_USER_ITEM_LIST_RES, sizeof(SAGIPUserItemListRes) - AGIP_ITEM_CODE_LEN - sizeof(int32_t))
{

}

AGIPUserItemListRes::~AGIPUserItemListRes(void)
{

}

int AGIPUserItemListRes::initialize()
{
    int nRetCode = E_ERROR;

    m_usTotalLength = sizeof(SAGIPUserItemListRes) - AGIP_ITEM_CODE_LEN - sizeof(int32_t);
    nRetCode = SysProtocol::initialize();
    ASSERT(nRetCode == S_SUCCESS);

    return S_SUCCESS;
}

int AGIPUserItemListRes::getResultCode(int32_t *pnResultCode)
{
    ASSERT(pnResultCode != NULL);
    SAGIPUserItemListRes *pSAGIPUserItemListRes = (SAGIPUserItemListRes *)m_pucPDU;
    *pnResultCode = htonl(pSAGIPUserItemListRes->n_Result_Code);
    return S_SUCCESS;
}

int AGIPUserItemListRes::getItemCount(int32_t *pnItemCount)
{
    ASSERT(pnItemCount != NULL);
    SAGIPUserItemListRes *pSAGIPUserItemListRes = (SAGIPUserItemListRes *)m_pucPDU;
    *pnItemCount = htonl(pSAGIPUserItemListRes->n_Item_Count);
    return S_SUCCESS;
}

int AGIPUserItemListRes::getItemInfo(int32_t nIndex, char *strItemCode, int32_t *pnItemNum)
{
    int nResult  = E_PDU_INVALID_FIELD;
    int nRetCode = E_PDU_INVALID_FIELD;

    ITEM_INFO *pItemInfoBuffer = NULL;
    int32_t nItemCount = 0;

    ASSERT(strItemCode != NULL);
    ASSERT(pnItemNum   != NULL);
    SAGIPUserItemListRes *pSAGIPUserItemListRes = (SAGIPUserItemListRes *)m_pucPDU;
    nItemCount = htonl(pSAGIPUserItemListRes->n_Item_Count);
    if (
        (nIndex <  0) ||
        (nIndex >= nItemCount)
        )
    {
        goto ExitError;
    }
    pItemInfoBuffer = (ITEM_INFO *)pSAGIPUserItemListRes->str_Item_Code;
    pItemInfoBuffer += nIndex;

    strncpy(strItemCode, pItemInfoBuffer->strItemCode, AGIP_ROLE_NAME_LEN);
    *pnItemNum = htonl(pItemInfoBuffer->nItemNum);

    nResult = S_SUCCESS;
ExitError:
    return nResult;
}

int AGIPUserItemListRes::setResultCode(int32_t nResultCode)
{
    SAGIPUserItemListRes *pSAGIPUserItemListRes = (SAGIPUserItemListRes *)m_pucPDU;
    pSAGIPUserItemListRes->n_Result_Code = htonl(nResultCode);
    return S_SUCCESS;
}

int AGIPUserItemListRes::addItemInfo(const char *strItemCode, int32_t nItemNum)
{
    int nResult  = E_PDU_INVALID_FIELD;
    int nRetCode = E_PDU_INVALID_FIELD;

    ITEM_INFO *pItemInfoBuffer = NULL;
    int32_t nItemCount = 0;

    ASSERT(strItemCode != NULL);
    SAGIPUserItemListRes *pSAGIPUserItemListRes = (SAGIPUserItemListRes *)m_pucPDU;
    nItemCount = htonl(pSAGIPUserItemListRes->n_Item_Count);

    pItemInfoBuffer =  (ITEM_INFO *)pSAGIPUserItemListRes->str_Item_Code;
    pItemInfoBuffer += nItemCount;

    strncpy(pItemInfoBuffer->strItemCode, strItemCode, AGIP_ROLE_NAME_LEN);
    pItemInfoBuffer->nItemNum = htonl(nItemNum);

    nItemCount++;
    pSAGIPUserItemListRes->n_Item_Count = htonl(nItemCount);
    m_usTotalLength += AGIP_ITEM_CODE_LEN + sizeof(int32_t);
    this->setTotalLength(m_usTotalLength);

    nResult = S_SUCCESS;
    //ExitError:
    return nResult;
}

int AGIPUserItemListRes::showInfo()
{
    int nRetCode        = E_ERROR;

    int32_t  nResultCode = 0;
    int32_t  nItemCount  = 0;
    char     strItemCode[AGIP_ITEM_CODE_LEN];
    int32_t  nItemNum    = 0;

    SysProtocol::showInfo();
    nRetCode = getResultCode(&nResultCode);
    nRetCode = getItemCount(&nItemCount);
    printf("--------------------------------------------------------AGIPUserItemListRes\n");
    printf("Result_Code:\t%d\n", nResultCode);
    printf("Item_Count:\t%d\n", nItemCount);
    for (int i = 0; i < nItemCount; i++)
    {
        nRetCode = getItemInfo(i, strItemCode, &nItemNum);
        strItemCode[sizeof(strItemCode) - 1] = '\0';
        printf("[%d]Item_Code: \t%s,\tItem_Num: \t%d.\n", i, strItemCode, nItemNum);
    }

    printf("--------------------------------------------------------AGIPUserItemListRes\n");
    return S_SUCCESS;
}
