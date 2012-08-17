#include "SysProtocol.h"
#include "SysAlgorithmCRC.h"
#include "SysAlgorithmMD5.h"
#include "SysEncryptConfig.h"
#include <time.h>

SysAlgorithmCRC     gSysAlgorithmCRC;
SysAlgorithmMD5     gSysAlgorithmMD5;
SysEncryptConfig    gSysEncryptConfig;

typedef struct __SECURITY_KEY__
{
    uint8_t arrucKey[16];
    int32_t nBeginTime;
    int32_t nEndTime;
    char    strAuthor[32];
    uint8_t arrucCheckSum[16];
}SECURITY_KEY;

static uint8_t sgarrucKey[] = {
    0x01, 0xF4, 0x55, 0x12,
    0x0A, 0x9F, 0x6E, 0x01,
    0xEE, 0x10, 0x1A, 0xA0,
    0x8C, 0xBD, 0xD1, 0x11
};
static SECURITY_KEY sgSecurity_Key = {0};

int SysProtocol::m_nKeyLoaded = false;

int isSecurityKeyValid(SECURITY_KEY &security_Key)
{
    int nResult  = E_ERROR;
    int nRetCode = E_ERROR;

    uint8_t arrucInput[80];
    uint8_t arrucDigest[16];
    // 
    time_t tmCurrentTime = 0;
   
    memcpy(arrucInput, &security_Key, sizeof(SECURITY_KEY) - 16);
    memcpy(arrucInput + sizeof(SECURITY_KEY) - 16, sgarrucKey, 16);
    nRetCode = gSysAlgorithmMD5.compute(arrucInput, sizeof(SECURITY_KEY), arrucDigest);
    ASSERT(nRetCode == S_SUCCESS);

    nRetCode = memcmp(arrucDigest, security_Key.arrucCheckSum, 16);
    if (nRetCode != 0)
    {
        goto ExitError;
    }

    security_Key.nBeginTime = htonl(security_Key.nBeginTime);
    security_Key.nEndTime   = htonl(security_Key.nEndTime);
    time(&tmCurrentTime);
    if ((int32_t)tmCurrentTime > security_Key.nEndTime)
    {
        goto ExitError;
    }

    
    nResult = S_SUCCESS;
ExitError:
    return nResult;
}

SysProtocol::SysProtocol(
    uint8_t     ucVersion,
    uint32_t    unCommandID,
    uint16_t    usTotalLength
)

:   m_usTotalLength(usTotalLength),
    m_pucPDU(m_arrucPDU),
    m_pSAGIPHeader((SAGIPHeader *)m_pucPDU),
    m_ucVersion(ucVersion),
    m_unCommandID(unCommandID)
{
    this->initialize();
}

SysProtocol::~SysProtocol(void)
{
    if (m_pucPDU != m_arrucPDU)
    {
        //free(m_pucPDU);
    }
}


int SysProtocol::initialize()
{
    ASSERT(m_pucPDU != NULL);
    ASSERT(m_pSAGIPHeader != NULL);
    ASSERT(AGIP_MAX_BUFFER_SIZE >= m_usTotalLength);
    //ASSERT(m_nKeyLoaded);
    memset(m_pucPDU, 0, m_usTotalLength);
    this->setVersion(m_ucVersion);
    this->setCommandID(m_unCommandID);
    this->setRemainPackages(AGIP_NO_REMAIN_PACKAGES);
    this->setTotalLength(m_usTotalLength);
    return S_SUCCESS;
}

int SysProtocol::showInfo()
{
    int      nRetCode         = E_ERROR;
    uint16_t i                = 0;
    uint16_t usTotalLength    = 0;
    uint8_t  ucVersion        = 0;
    uint8_t  ucRemainPackages = 0;
    uint32_t unCommandID      = 0;
    uint32_t unSequenceID     = 0;
    printf(">>\n");
    for (i = 0; i < m_usTotalLength; i++)
    {
        printf("[%d]%02X ", i, m_pucPDU[i]);
    }
    
    nRetCode = getTotalLength(&usTotalLength);
    nRetCode = getVersion(&ucVersion);
    nRetCode = getRemainPackages(&ucRemainPackages);
    nRetCode = getCommandID(&unCommandID);
    nRetCode = getSequenceID(&unSequenceID);
    printf("\n------------\n");
    printf("Total_Length:\t%u\n", usTotalLength);
    printf("Version:\t0x%02X\n", ucVersion);
    printf("Remain_Packages:\t%u\n", ucRemainPackages);
    printf("Command_ID:\t0x%08X\n", unCommandID);
    printf("Sequence_ID:\t0x%08X\n", unSequenceID);
    printf("\n<<\n");
    
    return S_SUCCESS;
}

int SysProtocol::loadKey(const char *strKeyFile /* = NULL */)
{
    int nResult = E_ERROR;
    int nRetCode = E_ERROR;
    
    uint8_t arrucKeyBuffer[80] = {0};

    size_t  stReadBufferLen = 0;
    int     nIsSecurityKeyValid = false;
    int     i = 0;

    FILE *pKeyFile = NULL;

    if (SysProtocol::m_nKeyLoaded)
    {
        goto ExitOK;
    }

    if (strKeyFile == NULL)
    {
        goto ExitError;
    }

    
    pKeyFile = fopen(strKeyFile, "rb");
    if (pKeyFile == NULL)
    {
        goto ExitError;
    }
    while (!feof(pKeyFile))
    {
        stReadBufferLen = fread(&sgSecurity_Key, sizeof(SECURITY_KEY), 1, pKeyFile);
    }
    /*
    if (!stReadBufferLen)
    {
        printf("errorno is %d.\n", errno);
        goto ExitError;
    }
    */
    // To Check whether the security key is valid.
    nRetCode = ::isSecurityKeyValid(sgSecurity_Key);
    if (nRetCode != S_SUCCESS)
    {
        goto ExitError;
    }

    SysProtocol::m_nKeyLoaded = true;
   
ExitOK:
    nResult = S_SUCCESS;
ExitError:
    if (pKeyFile != NULL)
    {
        fclose(pKeyFile);
    }
    return nResult;
}

int SysProtocol::calCheckSum(const void *pPDU, int32_t nLength, uint16_t *pusRetCheckSum)
{

    int nResult = E_ERROR;
    int nRetCode = E_ERROR;
    
    ASSERT(pusRetCheckSum != NULL);
    ASSERT(pPDU != NULL);

    
    // We must exclude the length of checksum.

    nRetCode = gSysAlgorithmCRC.compute(pPDU, nLength, 0xFEFF, pusRetCheckSum);

    nResult = nRetCode;
    
    return nResult;
}

int SysProtocol::checkPDUValid(const void *pPDU, int32_t nLength, int32_t nNeedsCheckSum)
{
    int nResult                 = E_PDU_ERROR;
    int nRetCode                = E_PDU_ERROR;
    uint16_t usCheckSum         = 0;

    int32_t  nTotalLength  = 0;
    uint8_t  ucVersion      = AGIP_INVALID_VERSION;
    uint32_t unCommandID    = CMD_INVALID;

    ASSERT(pPDU != NULL);

    SAGIPHeader   *pSAGIPHeader = (SAGIPHeader *)pPDU;
    SAGIPChecksum *pChecksum    = (SAGIPChecksum *)((char *)pPDU + nLength - 4);

    if (pSAGIPHeader->uc_Version != m_ucVersion)
    {
        nResult = E_PDU_UNKNOWN_VERSION;
        goto ExitError;
    }
    nTotalLength = htons(pSAGIPHeader->us_Total_Length);
    if (nTotalLength != nLength)
    {
        nResult = E_PDU_LENGTH_ERROR;
        goto ExitError;
    }

    if (nNeedsCheckSum == 0)
    {
        goto ExitOK;
    }
    // We must exclude the length of checksum.
    nRetCode = gSysAlgorithmCRC.compute(pPDU, nLength - 4, 0xFFFF, &usCheckSum);
    if (nRetCode != S_SUCCESS)
    {
        nResult = E_PDU_LENGTH_ERROR;
        goto ExitError;
    }
    if (pChecksum->us_Checksum != usCheckSum)
    {
        nResult = E_PDU_CHECKSUM_ERROR;
        goto ExitError;
    }
ExitOK:
    nResult = S_SUCCESS;
ExitError:
    return nResult;
}

int SysProtocol::encode(const void *pPDU, int32_t nInputLength, void *pPDUEncrypt, int32_t *pnOutputLength)
{
    int nResult = E_ERROR;
    int nRetCode = E_ERROR;
    
    ASSERT(pPDU != NULL);
    ASSERT(pPDUEncrypt != NULL);
    ASSERT(pnOutputLength != NULL);

    uint8_t *pucInputBuffer   = NULL;
    uint8_t *pucEncryptBuffer = NULL;

    pucInputBuffer = (uint8_t *)pPDU;

    nRetCode = gSysEncryptConfig.encrypt(
        pucInputBuffer + sizeof(SAGIPHeader),
        nInputLength   - sizeof(SAGIPHeader),
        sgSecurity_Key.arrucKey,
        16,
        pPDUEncrypt,
        nInputLength   - sizeof(SAGIPHeader) + 4,
        pnOutputLength
    );
    if (nRetCode != S_SUCCESS)
    {
        goto ExitError;
    }

    pucEncryptBuffer = (uint8_t *)pPDUEncrypt + *pnOutputLength;
    ASSERT(sizeof(SAGIPHeader) <= sizeof(sgSecurity_Key.arrucKey));
    for (int i = 0; i < sizeof(SAGIPHeader); i++)
    {
        pucEncryptBuffer[i] = (pucInputBuffer[i] ^ sgSecurity_Key.arrucKey[i]);
    }
    *pnOutputLength += sizeof(SAGIPHeader);
    
    nResult = S_SUCCESS;
ExitError:
    return nResult;
}

int SysProtocol::decode(const void *pPDU, int32_t nInputLength, void *pPDUDecrypt, int32_t *pnOutputLength)
{
    int nResult = E_ERROR;
    int nRetCode = E_ERROR;

    uint8_t *pucInputBuffer   = NULL;
    uint8_t *pucDecryptBuffer = NULL;

    ASSERT(pPDU           != NULL);
    ASSERT(pPDUDecrypt    != NULL);
    ASSERT(pnOutputLength != NULL);

    pucInputBuffer   = (uint8_t *)pPDU;
    pucDecryptBuffer = (uint8_t *)pPDUDecrypt;
    nRetCode = gSysEncryptConfig.decrypt(
        pPDU,
        nInputLength     - sizeof(SAGIPHeader),
        sgSecurity_Key.arrucKey,
        16,
        pucDecryptBuffer + sizeof(SAGIPHeader),
        nInputLength     - 4,
        pnOutputLength
    );

    if (nRetCode != S_SUCCESS)
    {
        goto ExitError;
    }

    ASSERT(sizeof(SAGIPHeader) <= sizeof(sgSecurity_Key.arrucKey));
    for (int i = 0; i < sizeof(SAGIPHeader); i++)
    {
        pucDecryptBuffer[i] = (pucInputBuffer[i + *pnOutputLength + 4] ^ sgSecurity_Key.arrucKey[i]);
    }

    *pnOutputLength += sizeof(SAGIPHeader);
    nResult = S_SUCCESS;
ExitError:
    return nResult;
}

int SysProtocol::getTotalLength(uint16_t *pusTotalLength)
{
    ASSERT(pusTotalLength != NULL && m_pSAGIPHeader != NULL);
    *pusTotalLength = htons(m_pSAGIPHeader->us_Total_Length);
    return S_SUCCESS;
}

int SysProtocol::getVersion(uint8_t *pucVersion)
{
    ASSERT(pucVersion != NULL && m_pSAGIPHeader != NULL);
    *pucVersion = m_pSAGIPHeader->uc_Version;
    return S_SUCCESS;
}

int SysProtocol::getRemainPackages(uint8_t *pucRemainPackages)
{
    ASSERT(pucRemainPackages != NULL && m_pSAGIPHeader != NULL);
    *pucRemainPackages = m_pSAGIPHeader->uc_Remain_Packages;
    return S_SUCCESS;
}

int SysProtocol::getCommandID(uint32_t *punCommandID)
{
    ASSERT(punCommandID != NULL && m_pSAGIPHeader != NULL);
    *punCommandID = htonl(m_pSAGIPHeader->un_Command_ID);
    return S_SUCCESS;
}

int SysProtocol::getSequenceID(uint32_t *punSequenceID)
{
    ASSERT(punSequenceID != NULL && m_pSAGIPHeader != NULL);
    *punSequenceID = htonl(m_pSAGIPHeader->un_Sequence_ID);
    return S_SUCCESS;
}

int SysProtocol::setTotalLength(uint16_t usTotalLength)
{
    ASSERT(m_pSAGIPHeader != NULL);
    ASSERT(AGIP_MAX_BUFFER_SIZE >= m_usTotalLength);
    m_pSAGIPHeader->us_Total_Length = htons(usTotalLength);
    return S_SUCCESS;
}

int SysProtocol::setVersion(uint8_t ucVersion)
{
    ASSERT(m_pSAGIPHeader != NULL);
    m_pSAGIPHeader->uc_Version = ucVersion;
    return S_SUCCESS;
}

int SysProtocol::setRemainPackages(uint8_t ucRemainPackages)
{
    ASSERT(m_pSAGIPHeader != NULL);
    m_pSAGIPHeader->uc_Remain_Packages = ucRemainPackages;
    return S_SUCCESS;
}

int SysProtocol::setCommandID(uint32_t unCommandID)
{
    ASSERT(m_pSAGIPHeader != NULL);
    m_pSAGIPHeader->un_Command_ID = htonl(unCommandID);
    return S_SUCCESS;
}

int SysProtocol::setSequenceID(uint32_t unSequenceID)
{
    ASSERT(m_pSAGIPHeader != NULL);
    m_pSAGIPHeader->un_Sequence_ID = htonl(unSequenceID);
    return S_SUCCESS;
}

int SysProtocol::cloneSequenceID(const SysProtocol *pSysProtocol)
{
    ASSERT(pSysProtocol != NULL);
    m_pSAGIPHeader->un_Sequence_ID = pSysProtocol->m_pSAGIPHeader->un_Sequence_ID;
    return S_SUCCESS;
}
/*
int SysProtocol::getHeader(SAGIPHeader *pSAGIPHeader)
{
    ASSERT(pSAGIPHeader != NULL);
    pSAGIPHeader->us_Total_Length    = htons(this->m_pSAGIPHeader->us_Total_Length);
    pSAGIPHeader->uc_Remain_Packages = this->m_pSAGIPHeader->uc_Remain_Packages;
    pSAGIPHeader->uc_Version         = this->m_pSAGIPHeader->uc_Version;
    pSAGIPHeader->un_Sequence_ID     = htonl(this->m_pSAGIPHeader->un_Sequence_ID);
    pSAGIPHeader->un_Command_ID      = htonl(this->m_pSAGIPHeader->un_Command_ID);
    return S_SUCCESS;
}
*/
int SysProtocol::parseCommandID(const void *pPDU, int32_t nPDULength, uint32_t *punCommandID)
{

    int nResult  = E_ERROR;
    int nRetCode = E_ERROR;

    uint8_t *pucPDU       = NULL;
    uint8_t *pucCommandID = NULL;
         
    ASSERT(pPDU != NULL && nPDULength >= 16 && punCommandID != NULL);

    pucPDU       = (uint8_t *)pPDU + nPDULength - sizeof(SAGIPHeader);
    pucCommandID = (uint8_t *)punCommandID;

    for (int i = 4; i < 8; i++)
    {
        pucCommandID[i - 4] = (pucPDU[i] ^ sgSecurity_Key.arrucKey[i]);
    }


    *punCommandID = htonl(*punCommandID);
    return S_SUCCESS;
}

int SysProtocol::setEncodedPDU(const void *pPDU, int32_t nPDULength, int32_t nNeedsCheckSum)
{
    int     nRetCode         = E_ERROR;
    int     nResult          = E_ERROR;
    
    int32_t nPDUOutputLength = 0;

    nRetCode        = SysProtocol::decode(pPDU, nPDULength, m_pucPDU, &nPDUOutputLength);
    ASSERT(nPDUOutputLength <= 0xFFFF);
    m_usTotalLength = (uint16_t)nPDUOutputLength;

    if (nRetCode != S_SUCCESS)
    {
        nResult = E_PDU_PARSE_ERROR;
        goto ExitError;
    }
    nRetCode = checkPDUValid(m_pucPDU, nPDUOutputLength, nNeedsCheckSum);
    if (nRetCode != S_SUCCESS)
    {
        nResult = nRetCode;
        goto ExitError;
    }

    nResult = S_SUCCESS;

ExitError:
    return nResult;
}

int SysProtocol::setDecodedPDU(const void *pPDU, int32_t nPDULength, int32_t nNeedsCheckSum)
{
    int nRetCode = E_ERROR;
    int nResult  = E_ERROR;

    size_t stPDUOutputLength = 0;

    ASSERT(pPDU != NULL);
    ASSERT(nPDULength >= 16 && nPDULength <= AGIP_MAX_BUFFER_SIZE);
    //nRetCode        = SysProtocol::decrypt(pPDU, stPDULength, m_pucPDU, &stPDUOutputLength);
    m_usTotalLength = (uint16_t)nPDULength;
    nRetCode = checkPDUValid(pPDU, nPDULength, nNeedsCheckSum);
    if (nRetCode != S_SUCCESS)
    {
        nResult = nRetCode;
        goto ExitError;
    }

    memcpy(m_pucPDU, pPDU, nPDULength);    
    nResult = S_SUCCESS;

ExitError:
    return nResult;
}

/***********************************************************************
    To get PDU pointer, and sent it directly.
***********************************************************************/

int SysProtocol::getDecodedPDU(void *pPDUBuffer, uint16_t usBufferSize, int32_t *pnPDULength)
{
    int nRetCode = E_ERROR;
    int nResult  = E_ERROR;

    uint16_t usTotalLength = 0;


    SAGIPChecksum *pSAGIPChecksum = NULL;
    ASSERT(pPDUBuffer  != NULL);
    ASSERT(pnPDULength != NULL);

    //pPDUPtr = m_pucPDU;
    nRetCode = this->getTotalLength(&usTotalLength);
    ASSERT(usBufferSize >= m_usTotalLength);

    pSAGIPChecksum = (SAGIPChecksum *)(m_pucPDU + m_usTotalLength - 4);
    pSAGIPChecksum->us_Resv = 0x0000;
    nRetCode = SysProtocol::calCheckSum(m_pucPDU, m_usTotalLength - 2, &pSAGIPChecksum->us_Checksum);
    ASSERT(nRetCode == S_SUCCESS);
    *pnPDULength = (uint32_t)m_usTotalLength;
    memcpy(pPDUBuffer, m_pucPDU, m_usTotalLength);
    return S_SUCCESS;
}

int SysProtocol::getEncodedPDU(void *pPDUBuffer, uint16_t usBufferSize, int32_t *pnPDULength)
{
    int      nRetCode      = E_ERROR;
    int      nResult       = E_ERROR;

    uint16_t       usTotalLength  = 0;
    SAGIPChecksum *pSAGIPChecksum = NULL;

    ASSERT(pPDUBuffer  != NULL);
    ASSERT(pnPDULength != NULL);

    //pPDUPtr = m_pucPDU;
    nRetCode = this->getTotalLength(&usTotalLength);
    ASSERT(usTotalLength == m_usTotalLength);
    ASSERT(usTotalLength >= 16);
    ASSERT(usBufferSize  >= m_usTotalLength + 4);
    //*pstPDULength = m_usTotalLength + 4;

    pSAGIPChecksum          = (SAGIPChecksum *)(m_pucPDU + m_usTotalLength - 4);
    pSAGIPChecksum->us_Resv = 0x0000;

    nRetCode = SysProtocol::calCheckSum(m_pucPDU, m_usTotalLength - 2, &pSAGIPChecksum->us_Checksum);
    ASSERT(nRetCode == S_SUCCESS);

    nRetCode = SysProtocol::encode(m_pucPDU, m_usTotalLength, pPDUBuffer, pnPDULength);
    ASSERT(nRetCode == S_SUCCESS);

    return S_SUCCESS;
}


int SysProtocol::compare(const SysProtocol *pSysProtocol, int *pnBytesEqual)
{
    int nBytesEqual = 0;

    ASSERT(pnBytesEqual != NULL);

    *pnBytesEqual = 0;

    nBytesEqual = memcmp(this->m_pucPDU, pSysProtocol->m_pucPDU, m_usTotalLength);
    if (nBytesEqual == 0)
    {
        *pnBytesEqual = 1;
    }
    return S_SUCCESS;
}

