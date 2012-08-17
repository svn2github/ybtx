#include "AGIPUserUseProduct.h"

AGIPUserUseProduct::AGIPUserUseProduct(void)
: SysProtocol(AGIP_CURRENT_VERSION, CMD_USER_USE_PRODUCT, sizeof(SAGIPUserUseProduct))
{
}

AGIPUserUseProduct::~AGIPUserUseProduct(void)
{
}

int AGIPUserUseProduct::getUserID(uint32_t *punUserID)
{
    ASSERT(punUserID != NULL);
    SAGIPUserUseProduct *pSAGIPUserUseProduct = (SAGIPUserUseProduct *)m_pucPDU;
    *punUserID = htonl(pSAGIPUserUseProduct->un_User_ID);
    return S_SUCCESS;
}

int AGIPUserUseProduct::getRoleID(uint32_t *punRoleID)
{
    ASSERT(punRoleID != NULL);
    SAGIPUserUseProduct *pSAGIPUserUseProduct = (SAGIPUserUseProduct *)m_pucPDU;
    *punRoleID = htonl(pSAGIPUserUseProduct->un_Role_ID);
    return S_SUCCESS;
}

int AGIPUserUseProduct::getPurchaseID(uint32_t *punPurchaseID)
{
    ASSERT(punPurchaseID != NULL);
    SAGIPUserUseProduct *pSAGIPUserUseProduct = (SAGIPUserUseProduct *)m_pucPDU;
    *punPurchaseID = htonl(pSAGIPUserUseProduct->un_Purchase_ID);
    return S_SUCCESS;
}

int AGIPUserUseProduct::setUserID(uint32_t unUserID)
{
    SAGIPUserUseProduct *pSAGIPUserUseProduct = (SAGIPUserUseProduct *)m_pucPDU;
    pSAGIPUserUseProduct->un_User_ID = htonl(unUserID);
    return S_SUCCESS;
}

int AGIPUserUseProduct::setRoleID(uint32_t unRoleID)
{
    SAGIPUserUseProduct *pSAGIPUserUseProduct = (SAGIPUserUseProduct *)m_pucPDU;
    pSAGIPUserUseProduct->un_Role_ID = htonl(unRoleID);
    return S_SUCCESS;
}

int AGIPUserUseProduct::setPurchaseID(uint32_t unPurchaseID)
{
    SAGIPUserUseProduct *pSAGIPUserUseProduct = (SAGIPUserUseProduct *)m_pucPDU;
    pSAGIPUserUseProduct->un_Purchase_ID = htonl(unPurchaseID);
    return S_SUCCESS;

}

int AGIPUserUseProduct::showInfo()
{
    int nRetCode        = E_ERROR;

    uint32_t   unUserID = 0;
    uint32_t   unRoleID = 0;
    uint32_t   unPurchaseID = 0;

    SysProtocol::showInfo();
    nRetCode = getUserID(&unUserID);
    nRetCode = getRoleID(&unRoleID);
    nRetCode = getPurchaseID(&unPurchaseID);
    printf("--------------------------------------------------------AGIPUserUseProduct\n");
    printf("User_ID:\t%u\n", unUserID);
    printf("Role_ID:\t%u\n", unRoleID);
    printf("Purchase_ID:\t%u\n", unPurchaseID);
    printf("--------------------------------------------------------AGIPUserUseProduct\n");
    return S_SUCCESS;
}
///////////////////////////
// AGIPUserUseProductRes

AGIPUserUseProductRes::AGIPUserUseProductRes(void)
: SysProtocol(AGIP_CURRENT_VERSION, CMD_USER_USE_PRODUCT_RES, sizeof(SAGIPUserUseProductRes))
{

}

AGIPUserUseProductRes::~AGIPUserUseProductRes(void)
{

}

int AGIPUserUseProductRes::getResultCode(int32_t *pnResultCode)
{
    ASSERT(pnResultCode != NULL);
    SAGIPUserUseProductRes *pSAGIPUserUseProductRes = (SAGIPUserUseProductRes *)m_pucPDU;
    *pnResultCode = htonl(pSAGIPUserUseProductRes->n_Result_Code);

    return S_SUCCESS;
}

int AGIPUserUseProductRes::setResultCode(int32_t nResultCode)
{
    SAGIPUserUseProductRes *pSAGIPUserUseProductRes = (SAGIPUserUseProductRes *)m_pucPDU;
    pSAGIPUserUseProductRes->n_Result_Code = htonl(nResultCode);

    return S_SUCCESS;
}

int AGIPUserUseProductRes::showInfo()
{
    int nRetCode        = E_ERROR;
    int32_t   nResultCode = 0;

    SysProtocol::showInfo();
    nRetCode = getResultCode(&nResultCode);
    printf("--------------------------------------------------------AGIPUserUseProductRes\n");
    printf("Result_Code:\t%d\n", nResultCode);
    printf("--------------------------------------------------------AGIPUserUseProductRes\n");
    return S_SUCCESS;
}


