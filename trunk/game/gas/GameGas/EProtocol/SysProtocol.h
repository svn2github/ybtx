#ifndef __SYS_PROTOCOL_H__
#define __SYS_PROTOCOL_H__

#include "Macro.h"
#define AGIP_INVALID_VERSION            0x00
#define AGIP_CURRENT_VERSION            0x10
#define AGIP_NO_REMAIN_PACKAGES         0x00
#define AGIP_MAX_BUFFER_SIZE            1024

#define CMD_INVALID                     0x00000000
#define CMD_BIND                        0x10000001
#define CMD_BIND_RES                    0x20000001
#define CMD_UNBIND                      0x10000002
#define CMD_UNBIND_RES                  0x20000002
#define CMD_TIME_SYN                    0x10000003
#define CMD_TIME_SYN_RES                0x20000003
#define CMD_HANDSET                     0x10001001
#define CMD_HANDSET_RES                 0x20001001
#define CMD_ONLINE_ACCOUNTS_SUM         0x10002001
#define CMD_ONLINE_ACCOUNTS_SUM_RES     0x20002001
#define CMD_PRODUCT_INFO                0x10002002
#define CMD_PRODUCT_INFO_RES            0x20002002
#define CMD_GW_DATA_REPORT              0x10002003
#define CMD_GW_DATA_REPORT_RES          0x20002003
#define CMD_CREATE_OR_UPDATE_USER       0x10003001
#define CMD_CREATE_OR_UPDATE_USER_RES   0x20003001
#define CMD_SET_OR_UPDATE_PASSWORD      0x10003002
#define CMD_SET_OR_UPDATE_PASSWORD_RES  0x20003002
#define CMD_USER_INFO                   0x10003003
#define CMD_USER_INFO_RES               0x20003003
#define CMD_ROLE_LIST                   0x10003101
#define CMD_ROLE_LIST_RES               0x20003101
#define CMD_CREATE_ROLE                 0x10003102
#define CMD_CREATE_ROLE_RES             0x20003102
#define CMD_DELETE_ROLE                 0x10003103
#define CMD_DELETE_ROLE_RES             0x20003103
#define CMD_PASSWORD_VERIFY             0x10003202
#define CMD_PASSWORD_VERIFY_RES         0x20003202
#define CMD_SERIALNO_VERIFY             0x10003203
#define CMD_SERIALNO_VERIFY_RES         0x20003203
#define CMD_USER_LOGIN                  0x10003301
#define CMD_USER_LOGIN_RES              0x20003301
#define CMD_USER_ENTER_GAME             0x10003302
#define CMD_USER_ENTER_GAME_RES         0x20003302
#define CMD_USER_LOGOUT                 0x10003303
#define CMD_USER_LOGOUT_RES             0x20003303
#define CMD_USER_CHARGE                 0x10003401
#define CMD_USER_CHARGE_RES             0x20003401
#define CMD_USER_BALANCE_INFO           0x10003402
#define CMD_USER_BALANCE_INFO_RES       0x20003402

#define CMD_USER_DRAW_BALANCE           0x10003403
#define CMD_USER_DRAW_BALANCE_RES       0x20003403

#define CMD_USER_BUY_PRODUCT            0x10003501
#define CMD_USER_BUY_PRODUCT_RES        0x20003501
#define CMD_USER_USE_PRODUCT            0x10003502
#define CMD_USER_USE_PRODUCT_RES        0x20003502
#define CMD_USER_PRODUCT_INFO           0x10003503
#define CMD_USER_PRODUCT_INFO_RES       0x20003503
#define CMD_USER_ITEM_ADD               0x10003504
#define CMD_USER_ITEM_ADD_RES           0x20003504
#define CMD_USER_ITEM_MINUS             0x10003505
#define CMD_USER_ITEM_MINUS_RES         0x20003505
#define CMD_USER_ITEM_SET               0x10003506
#define CMD_USER_ITEM_SET_RES           0x20003506
#define CMD_USER_ITEM_INFO              0x10003507
#define CMD_USER_ITEM_INFO_RES          0x20003507
#define CMD_USER_ITEM_LIST              0x10003508
#define CMD_USER_ITEM_LIST_RES          0x20003508

#define CMD_USER_DATA_REPORT            0x10003601
#define CMD_USER_DATA_REPORT_RES        0x20003601

enum _AGIP_ARRAY_FIELD_LEN_
{
    AGIP_GATEWAY_CODE_LEN   = 32,
    AGIP_MAC_LEN            = 6,

    AGIP_USER_NAME_LEN      = 32,
    AGIP_ROLE_NAME_LEN      = 32,

    AGIP_PASSWORD_LEN       = 32,
    AGIP_SERIAL_NO_LEN      = 32,
    AGIP_IB_CODE_LEN        = 32,
    AGIP_ITEM_CODE_LEN      = 32
};


typedef struct _BALANCE_INFO_
{
    int32_t nSubjectID;
    int32_t nAmount;
}BALANCE_INFO;

typedef struct _ALL_BALANCE_INFO_
{
    int32_t nSubjectID;
    int32_t nAmount;
    int32_t nLeftAmount;
}ALL_BALANCE_INFO;

typedef struct _PRICE_INFO_
{
    int32_t nSubjectID;
    int32_t nPrice;
    int32_t nDiscountPrice;
}PRICE_INFO;

typedef struct _ROLE_INFO_
{
    uint32_t unRoleID;
    char     strRoleName[AGIP_ROLE_NAME_LEN];
}ROLE_INFO;

typedef struct _DATA_INFO_
{
    int32_t nDataType;
    int32_t nDataValue;
}DATA_INFO;

typedef struct _ITEM_INFO_
{
    char    strItemCode[AGIP_ITEM_CODE_LEN];
    int32_t nItemNum;
}ITEM_INFO;

typedef struct _PURCHASE_ID_
{
    uint32_t        un_Purchase_ID_Higher;
    uint32_t        un_Purchase_ID_Lower;
}PURCHASE_ID;

typedef struct _AGIP_HEADER_
{
    uint16_t us_Total_Length;
    uint8_t  uc_Version;
    uint8_t  uc_Remain_Packages;
    uint32_t un_Command_ID;
    uint32_t un_Sequence_ID;
}SAGIPHeader, *PSAGIPHeader;

typedef struct _AGIP_CHECKSUM_
{
    int16_t     us_Resv;
    uint16_t    us_Checksum;
        
}SAGIPChecksum, *PSAGIPChecksum;

enum _AGIP_PDU_LENGTH_
{
    AGIP_BIND_LEN = sizeof(int)
};




typedef struct _AGIP_BIND_
{
    _AGIP_HEADER_   header;
    char            str_Gateway_Code[AGIP_GATEWAY_CODE_LEN];
    char            str_Password[AGIP_PASSWORD_LEN];
    uint8_t         uc_MAC[AGIP_MAC_LEN];
    int8_t          c_Reconnect_Flag;
    int8_t          c_Pad;
    uint32_t        un_Server_ID; // 增加的Server_ID，用于批量登出。
    _AGIP_CHECKSUM_ checkSum;
} SAGIPBind, *PSAGIPBind;

typedef struct _AGIP_BIND_RES_
{
    _AGIP_HEADER_   header;
    int32_t         n_Result_Code;
    uint32_t        un_Gateway_ID;
    
    _AGIP_CHECKSUM_ checksum;
} SAGIPBindRes, *PSAGIPBindRes;

typedef struct _AGIP_UNBIND_
{
    _AGIP_HEADER_   header;
    uint32_t        un_Gateway_ID;
    _AGIP_CHECKSUM_ checkSum;
} SAGIPUnbind, *PSAGIPUnbind;

typedef struct _AGIP_UNBIND_RES_
{
    _AGIP_HEADER_   header;
    int32_t         n_Result_Code;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUnbindRes, *PSAGIPUnbindRes;


//////////////////////////////Handset///////////////////////////////////
typedef struct _AGIP_HANDSET_
{
    _AGIP_HEADER_   header;
    _AGIP_CHECKSUM_ checksum;
} SAGIPHandset, *PSAGIPHandset;

typedef struct _AGIP_HANDSET_RES_
{
    _AGIP_HEADER_   header;
    _AGIP_CHECKSUM_ checksum;
} SAGIPHandsetRes, *PSAGIPHandsetRes;

//////////////////
// Gateway Business.
typedef struct _AGIP_GW_DATA_REPORT_
{
    _AGIP_HEADER_   header;
    uint32_t        un_Gateway_ID;
    uint32_t        un_Server_ID;// Added at 2007-09-02
    int32_t         n_Data_Count;
    int32_t         n_Data_Type;
    int32_t         n_Data_Value;
    _AGIP_CHECKSUM_ checksum;
} SAGIPGWDataReport, *PSAGIPGWDataReport;

typedef struct _AGIP_GW_DATA_REPORT_RES_
{
    _AGIP_HEADER_   header;
    int32_t         n_Result_Code;
    _AGIP_CHECKSUM_ checksum;
} SAGIPGWDataReportRes, *PSAGIPGWDataReportRes;


////////////////////Player Bussiness//////////////////////////////////
// Roles
typedef struct _AGIP_CREATE_ROLE_
{
    _AGIP_HEADER_   header;
    uint32_t        un_User_ID;
    char            str_Role_Name[AGIP_ROLE_NAME_LEN];
    int8_t          c_Gender;
    int8_t          c_Occupation;
    int8_t          c_Initial_Level;
    uint8_t         uc_Community_ID;
    uint32_t        un_User_IP;
    uint16_t        us_User_Port;
    int16_t         s_Pad2;
    _AGIP_CHECKSUM_ checksum;
} SAGIPCreateRole, *PSAGIPCreateRole;

typedef struct _AGIP_CREATE_ROLE_RES_
{
    _AGIP_HEADER_   header;
    int32_t         n_Result_Code;
    uint32_t        un_Role_ID;
    _AGIP_CHECKSUM_ checksum;
} SAGIPCreateRoleRes, *PSAGIPCreateRoleRes;

typedef struct _AGIP_DELETE_ROLE_
{
    _AGIP_HEADER_   header;
    uint32_t        un_User_ID;
    uint32_t        un_Role_ID;
    _AGIP_CHECKSUM_ checksum;
} SAGIPDeleteRole, *PSAGIPDeleteRole;

typedef struct _AGIP_DELETE_ROLE_RES_
{
    _AGIP_HEADER_   header;
    int32_t         n_Result_Code;
    _AGIP_CHECKSUM_ checksum;
} SAGIPDeleteRoleRes, *PSAGIPDeleteRoleRes;

typedef struct _AGIP_ROLE_LIST_
{
    _AGIP_HEADER_   header;
    uint32_t        un_User_ID;
    _AGIP_CHECKSUM_ checksum;
} SAGIPRoleList, *PSAGIPRoleList;

typedef struct _AGIP_ROLE_LIST_RES_
{
    _AGIP_HEADER_   header;
    int32_t         n_Result_Code;
    int32_t         n_Role_Count;
    uint32_t        un_Role_ID;
    char            str_Role_Name[AGIP_ROLE_NAME_LEN];
    _AGIP_CHECKSUM_ checksum;
} SAGIPRoleListRes, *PSAGIPRoleListRes;

// Password
typedef struct _AGIP_PASSWORD_VERIFY_
{
    _AGIP_HEADER_   header;
    uint32_t        un_User_ID;
    char            str_Password[AGIP_PASSWORD_LEN];
    int8_t          c_Password_Type;
    int8_t          c_Pad1;
    int16_t         s_Pad2;
    _AGIP_CHECKSUM_ checksum;
} SAGIPPasswordVerify, *PSAGIPPasswordVerify;

typedef struct _AGIP_PASSWORD_VERIFY_RES_
{
    _AGIP_HEADER_   header;
    int32_t         n_Result_Code;
    _AGIP_CHECKSUM_ checksum;
} SAGIPPasswordVerifyRes, *PSAGIPPasswordVerifyRes;

typedef struct _AGIP_SERIALNO_VERIFY_
{
    _AGIP_HEADER_   header;
    uint32_t        un_User_ID;
    char            str_Serial_NO[AGIP_SERIAL_NO_LEN];
    int8_t          c_Serial_Type;
    int8_t          Pad1;
    int16_t         Pad2;
    _AGIP_CHECKSUM_ checksum;
} SAGIPSerialNOVerify, *PSAGIPSerialNOVerify;

typedef struct _AGIP_SERIALNO_VERIFY_RES_
{
    _AGIP_HEADER_   header;
    int32_t         n_Result_Code;
    _AGIP_CHECKSUM_ checksum;
} SAGIPSerialNOVerifyRes, *PSAGIPSerialNOVerifyRes;
// Player
typedef struct _AGIP_USER_LOGIN_
{
    _AGIP_HEADER_   header;
    char            str_User_Name[AGIP_USER_NAME_LEN];
    char            str_Password[AGIP_PASSWORD_LEN];
    int8_t          c_Password_Type;
    int8_t          c_Pad1;
    uint16_t        us_User_Port;
    uint32_t        un_User_IP;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserLogin, *PSAGIPUserLogin;

typedef struct _AGIP_USER_LOGIN_RES_
{
    _AGIP_HEADER_   header;
    int32_t         n_Result_Code;
    uint32_t        un_User_ID;
    int8_t          c_User_Type;
    int8_t          c_Adult_Flag;
    int8_t          c_User_Class;
    int8_t          c_User_Role_Count;
    int32_t         n_User_Point;
    uint32_t        un_Promoter_ID;
    uint32_t        un_User_Flag;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserLoginRes, *PSAGIPUserLoginRes;

typedef struct _AGIP_USER_ENTER_GAME_
{
    _AGIP_HEADER_   header;
    uint32_t        un_User_ID;
    uint32_t        un_Role_ID;
    uint32_t        un_Rating_ID;
    uint32_t        un_Server_ID;
    int8_t          c_Role_Level;
    int8_t          c_Pad1;
    uint16_t        us_User_Port;
    uint32_t        un_User_IP;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserEnterGame, *PSAGIPUserEnterGame;

typedef struct _AGIP_USER_ENTER_GAME_RES_
{
    _AGIP_HEADER_   header;
    int32_t         n_Result_Code;
    int32_t         n_Balance_Count;
    int32_t         n_Subject_ID;
    int32_t         n_Amount;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserEnterGameRes, *PSAGIPUserEnterGameRes;

typedef struct _AGIP_USER_LOGOUT_
{
    _AGIP_HEADER_   header;
    uint32_t        un_User_ID;
    uint32_t        un_Role_ID;
    int8_t          c_Logout_Flag;
    int8_t          c_Role_Level;
    int8_t          c_Occupation;
    int8_t          c_Pad;
    uint32_t        un_Rating_ID;
    uint32_t        un_Money1;
    uint32_t        un_Money2;
    uint32_t        un_Experience_Higher;
    uint32_t        un_Experience_Lower;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserLogout, *PSAGIPUserLogout;

typedef struct _AGIP_USER_LOGOUT_RES_
{
    _AGIP_HEADER_   header;
    int32_t         n_Result_Code;
    int32_t         n_Balance_Count;
    int32_t         n_Subject_ID;
    int32_t         n_Amount;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserLogoutRes, *PSAGIPUserLogoutRes;

typedef struct _AGIP_USER_BALANCE_INFO_
{
    _AGIP_HEADER_   header;
    uint32_t        un_User_ID;
    uint32_t        un_Role_ID;
    uint32_t        un_Rating_ID;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserBalanceInfo, *PSAGIPUserBalanceInfo;

typedef struct _AGIP_USER_BALANCE_INFO_RES_
{
    _AGIP_HEADER_   header;
    int32_t         n_Result_Code;
    int32_t         n_Balance_Count;
    int32_t         n_Subject_ID;
    int32_t         n_Amount;
    int32_t         n_Left_Amount;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserBalanceInfoRes, *PSAGIPUserBalanceInfoRes;

typedef struct _AGIP_USER_DRAW_BALANCE_
{
    _AGIP_HEADER_   header;
    uint32_t        un_User_ID;
    uint32_t        un_Role_ID;
    uint32_t        un_Rating_ID;
    int32_t         n_Subject_ID;
    int32_t         n_Amount;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserDrawBalance, *PSAGIPUserDrawBalance;

typedef struct _AGIP_USER_DRAW_BALANCE_RES_
{
    _AGIP_HEADER_   header;
    int32_t         n_Result_Code;
    int32_t         n_Amount;
    int32_t         n_Left_Amount;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserDrawBalanceRes, *PSAGIPUserDrawBalanceRes;

typedef struct _AGIP_USER_BUY_PRODUCT_
{
    _AGIP_HEADER_   header;
    uint32_t        un_User_ID;
    uint32_t        un_Role_ID;
    char            str_IB_Code[AGIP_IB_CODE_LEN];
    int8_t          c_Package_Flag;
    int8_t          c_Pad;
    int16_t         s_Amount;
    uint32_t        un_Rating_ID;
    uint32_t        un_Server_ID;
    uint32_t        un_User_IP;
    int32_t         n_Subject_ID;
    int32_t         n_Price;
    int32_t         n_Discount_Price;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserBuyProduct, *PSAGIPUserBuyProduct;

typedef struct _AGIP_USER_BUY_PRODUCT_RES_
{
    _AGIP_HEADER_   header;
    int32_t         n_Result_Code;
    int32_t         n_Cost_Money;
    int32_t         n_Remain_Money;
    int32_t         n_IB_Count;
    uint32_t        un_Purchase_ID_Higher;
    uint32_t        un_Purchase_ID_Lower;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserBuyProductRes, *PSAGIPUserBuyProductRes;

typedef struct _AGIP_USER_USE_PRODUCT_
{
    _AGIP_HEADER_   header;
    uint32_t        un_User_ID;
    uint32_t        un_Role_ID;
    uint32_t        un_Purchase_ID;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserUseProduct, *PSAGIPUserUseProduct;

typedef struct _AGIP_USER_USE_PRODUCT_RES_
{
    _AGIP_HEADER_   header;
    int32_t         n_Result_Code;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserUseProductRes, *PSAGIPUserUseProductRes;

typedef struct _AGIP_USER_ITEM_INFO_
{
    _AGIP_HEADER_   header;
    uint32_t        un_Item_Gateway_ID;
    uint32_t        un_User_ID;
    uint32_t        un_Role_ID;
    uint32_t        un_Activity_ID;
    char            str_Item_Code[AGIP_ITEM_CODE_LEN];
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserItemInfo, *PSAGIPUserItemInfo;

typedef struct _AGIP_USER_ITEM_INFO_RES_
{
    _AGIP_HEADER_   header;
    int32_t         n_Result_Code;
    int32_t         n_Item_Num;
    int32_t         n_Begin_Time;
    int32_t         n_End_Time;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserItemInfoRes, *PSAGIPUserItemInfoRes;

typedef struct _AGIP_USER_ITEM_LIST_
{
    _AGIP_HEADER_   header;
    uint32_t        un_Item_Gateway_ID;
    uint32_t        un_User_ID;
    uint32_t        un_Role_ID;
    uint32_t        un_Activity_ID;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserItemList, *PSAGIPUserItemList;

typedef struct _AGIP_USER_ITEM_LIST_RES_
{
    _AGIP_HEADER_   header;
    int32_t         n_Result_Code;
    int32_t         n_Item_Count;
    char            str_Item_Code[AGIP_ITEM_CODE_LEN];
    int32_t         n_Item_Num;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserItemListRes, *PSAGIPUserItemListRes;


typedef struct _AGIP_USER_ITEM_MINUS_
{
    _AGIP_HEADER_   header;
    uint32_t        un_Item_Gateway_ID;
    uint32_t        un_User_ID;
    uint32_t        un_Role_ID;
    uint32_t        un_Activity_ID;
    char            str_Item_Code[AGIP_ITEM_CODE_LEN];
    int32_t         n_Item_Num;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserItemMinus, *PSAGIPUserItemMinus;

typedef struct _AGIP_USER_ITEM_MINUS_RES_
{
    _AGIP_HEADER_   header;
    int32_t         n_Result_Code;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserItemMinusRes, *PSAGIPUserItemMinusRes;


typedef struct _AGIP_USER_ITEM_ADD_
{
    _AGIP_HEADER_   header;
    uint32_t        un_Item_Gateway_ID;
    uint32_t        un_User_ID;
    uint32_t        un_Role_ID;
    uint32_t        un_Activity_ID;
    char            str_Item_Code[AGIP_ITEM_CODE_LEN];
    int32_t         n_Item_Num;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserItemAdd, *PSAGIPUserItemAdd;

typedef struct _AGIP_USER_ITEM_ADD_RES_
{
    _AGIP_HEADER_   header;
    int32_t         n_Result_Code;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserItemAddRes, *PSAGIPUserItemAddRes;

typedef struct _AGIP_USER_ITEM_SET_
{
    _AGIP_HEADER_   header;
    uint32_t        un_Item_Gateway_ID;
    uint32_t        un_User_ID;
    uint32_t        un_Role_ID;
    uint32_t        un_Activity_ID;
    char            str_Item_Code[AGIP_ITEM_CODE_LEN];
    int32_t         n_Item_Num;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserItemSet, *PSAGIPUserItemSet;

typedef struct _AGIP_USER_ITEM_SET_RES_
{
    _AGIP_HEADER_   header;
    int32_t         n_Result_Code;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserItemSetRes, *PSAGIPUserItemSetRes;


typedef struct _AGIP_USER_DATA_REPORT_
{
    _AGIP_HEADER_   header;
    uint32_t        un_User_ID;
    uint32_t        un_Role_ID;
    int32_t         n_Data_Count;
    int32_t         n_Data_Type;
    int32_t         n_Data_Value;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserDataReport, *PSAGIPUserDataReport;

typedef struct _AGIP_USER_DATA_REPORT_RES_
{
    _AGIP_HEADER_   header;
    int32_t         n_Result_Code;
    _AGIP_CHECKSUM_ checksum;
} SAGIPUserDataReportRes, *PSAGIPUserDataReportRes;


class Engine_Export SysProtocol
{
public:
    SysProtocol(uint8_t ucVersion, uint32_t unCommandID, uint16_t usTotalLength);
public:
    virtual ~SysProtocol(void);
    
    static int loadKey(const char *strKeyFile); /* You must invoke this function to load private key from file. */
    static int parseCommandID(const void *pPDU, int32_t nPDULength, uint32_t *punCommandID);

    virtual int initialize();

    int setEncodedPDU(const void *pPDU, int32_t nPDULength, int32_t nNeedsCheckSum = 1);
    int setDecodedPDU(const void *pPDU, int32_t nPDULength, int32_t nNeedsCheckSum = 1);
    
    int getEncodedPDU(void *pPDUBuffer, uint16_t usBufferSize, int32_t *pnPDULength);
    int getDecodedPDU(void *pPDUBuffer, uint16_t usBufferSize, int32_t *pnPDULength);

    int getTotalLength(uint16_t *pusTotalLength);
    int getVersion(uint8_t *pucVersion);
    int getRemainPackages(uint8_t *pucRemainPackages);
    int getCommandID(uint32_t *punCommandID);
    int getSequenceID(uint32_t *punSequenceID);
    //int getHeader(SAGIPHeader *pSAGIPHeader);

    int setRemainPackages(uint8_t ucRemainPackages);
    int setSequenceID(uint32_t unSequenceID);

    int cloneSequenceID(const SysProtocol *pSysProtocol); /* Copy sequence ID from pSysProtocol directly. */

    int compare(const SysProtocol *pSysProtocol, int *pnBytesEqual); /* Compare binary stream between this and pSysProtocol. */

    virtual int showInfo();

protected:
    
    uint8_t        *m_pucPDU;
    SAGIPHeader    *m_pSAGIPHeader;
    uint16_t        m_usTotalLength;

    int setTotalLength(uint16_t usTotalLength);
    int setVersion(uint8_t ucVersion);
    int setCommandID(uint32_t unCommandID);

    int checkPDUValid(const void *pPDU, int32_t nLength, int32_t nNeedsCheckSum = 1);

    static int calCheckSum(const void *pPDU, int32_t nLength, uint16_t *pusRetCheckSum);
    static int encode(const void *pPDU, int32_t nInputLength, void *pPDUEncrypt, int32_t *pnOutputLength);
    static int decode(const void *pPDU, int32_t nInputLength, void *pPDUDecrypt, int32_t *pnOutputLength);
private:
    uint8_t         m_arrucPDU[AGIP_MAX_BUFFER_SIZE];
    uint32_t        m_unCommandID;
    uint8_t         m_ucVersion;
    static int      m_nKeyLoaded;

};

#endif
