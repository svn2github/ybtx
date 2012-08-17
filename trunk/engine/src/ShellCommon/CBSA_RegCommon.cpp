#include "stdafx.h"
#include "ScriptX.h"
#include "CBaseScriptApp.h"
#include "CDir.h"
#include "CPos.h"
#include "CRectangle.h"
#include "CVector2.h"
#include "CVector3.h"
#include "CMd5Digester.h"
#include "StringHelper.h"
#include "CTick.h"
#include "BaseHelper.h"
#include "ByteOrder.h"
#include "TRandomMersenne.h"
#include "MemoryHelper.h"
#include "CoreCommon.h"
#include "ErrLogHelper.h"
#include "CLog.h"
#include "ExpHelper.h"
#include "CodeCvs.h"
#include "ConvertMbcsToUtf8.h"
#include "ConvertUtf8ToMbcs.h"
#include "CDistortedTick.h"
#include "CodeCvs.h"
#include "CTxtTableFile.h"
#include "CValueTable.h"
#include "VariantTypes.h"
#include "CRowTableFile.h"
#include "CRowTableFileNode.h"
#include "CTableFileValue.h"
#include "CVariant.h"
#include "CValueTableData.h"
#include "TSqrAllocator.inl"
#include "CLuaIO.h"
#include "CrcAlgorithm.h"
#include "CMultiLangTxtTableFile.h"
#include "CXmlConfig.inl"
#include "CItemXmlRead.h"
#include "CXMLReadAndWrite.h"
#include "CPathMgr.h"

static bool IsWindows()
{
#ifdef _WIN32
	return true;
#else
	return false;
#endif
}

int32 CBaseScriptApp::RegisterCommon(CScript& Script)
{
	REGIST_ISOLATED_CLASS( Script, CPos );
	REGIST_ISOLATED_CLASS( Script, CFPos );
	REGIST_ISOLATED_CLASS( Script, CVector2f );
	REGIST_ISOLATED_CLASS( Script, CVector3f );
	REGIST_ISOLATED_CLASS( Script, CDir );
	REGIST_ISOLATED_CLASS( Script, CIRect );
	REGIST_ISOLATED_CLASS( Script, CFRect );

	REGIST_ISOLATED_CLASS( Script, CMd5Digester );
	
	REGIST_B_CLASS( Script, CTick );
	
	REGIST_CLASSMEMBER( Script, CPos, x, "i" );
	REGIST_CLASSMEMBER( Script, CPos, y, "i" );

	REGIST_CLASSMEMBER( Script, CFPos, x, "f" );
	REGIST_CLASSMEMBER( Script, CFPos, y, "f" );

	REGIST_CLASSMEMBER( Script, CVector2f, x, "f" );
	REGIST_CLASSMEMBER( Script, CVector2f, y, "f" );

	REGIST_CLASSMEMBER( Script, CVector3f, x, "f" );
	REGIST_CLASSMEMBER( Script, CVector3f, y, "f" );
	REGIST_CLASSMEMBER( Script, CVector3f, z, "f" );

	REGIST_CLASSMEMBER( Script, CDir, uDir, "C" );
	REGIST_CLASSFUNCTION( Script, CDir, Get, "(CVector2f)" );
	REGIST_CLASSFUNCTION( Script, CDir, Set, "(CVector2f)" );
	REGIST_CLASSFUNCTION( Script, CDir, SetDir, "C" );

	REGIST_CLASSMEMBER( Script, CIRect, left, "i" );
	REGIST_CLASSMEMBER( Script, CIRect, right, "i" );
	REGIST_CLASSMEMBER( Script, CIRect, top, "i" );
	REGIST_CLASSMEMBER( Script, CIRect, bottom, "i" );
	REGIST_CLASSFUNCTION( Script, CIRect, Width, ":i");
	REGIST_CLASSFUNCTION( Script, CIRect, Height, ":i");

	REGIST_CLASSMEMBER( Script, CFRect, left, "f" );
	REGIST_CLASSMEMBER( Script, CFRect, right, "f" );
	REGIST_CLASSMEMBER( Script, CFRect, top, "f" );
	REGIST_CLASSMEMBER( Script, CFRect, bottom, "f" );
	REGIST_CLASSFUNCTION( Script, CFRect, Width, ":f");
	REGIST_CLASSFUNCTION( Script, CFRect, Height, ":f");

	REGIST_CLASSFUNCTION( Script, CMd5Digester, Begin, "");
	REGIST_CLASSFUNCTION( Script, CMd5Digester, Append, "sI");
	REGIST_CLASSFUNCTION( Script, CMd5Digester, GetMD5Base16Str, "m");

	REGIST_ISOLATED_CLASS( Script, TRandomMersenne );
	REGIST_CLASSFUNCTION( Script, TRandomMersenne, IRandom, "ii:i" );
	//对象事件通知类		
	
	
	REGIST_GLOBALFUNCTION(Script, String2ID, "s:I" );
	REGIST_GLOBALFUNCTION(Script, GetSystemName, ":s" );
	REGIST_GLOBALFUNCTION(Script, IsWindows, ":b" );
	REGIST_GLOBALFUNCTION(Script, Crc16, "mH:H" );
	REGIST_GLOBALFUNCTION(Script, Crc32, "mI:I" );

	REGIST_CALLBACKFUNCTION( Script, CTick, OnTick, "" );
	REGIST_CALLBACKFUNCTION( Script, CTick, GetTickClassName, ":s" );
	REGIST_CLASSFUNCTION(Script, CTick, SetTickName, "s");
	REGIST_CLASSFUNCTION( Script, CTick, GetTickName, ":s");

	REGIST_GLOBALFUNCTION(Script, ChangeByteOrder_int32, "i:i");
	REGIST_GLOBALFUNCTION(Script, ChangeByteOrder_uint32, "I:I");
	REGIST_GLOBALFUNCTION(Script, ChangeByteOrder_int16, "h:h");
	REGIST_GLOBALFUNCTION(Script, ChangeByteOrder_uint16, "H:H");
	REGIST_GLOBALFUNCTION(Script, ChangeByteOrder_uint64, "Z:Z");

	REGIST_GLOBALFUNCTION(Script, GetPixelOfGrid, "if:f");

	REGIST_GLOBALFUNCTION(Script, ShowErrorMsgAndExit, "S");

	REGIST_ABSTRACT_B_CLASS( Script, CLog );

	REGIST_STATICFUNCTION(Script, CLog, CreateLog, "ss:(CLog)");
	REGIST_CLASSFUNCTION(Script, CLog, Release, "");
	REGIST_CLASSFUNCTION(Script, CLog, EnableTime, "b");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CLog, void (CLog::*)(const char*), Write, Write, "S");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CLog, void (CLog::*)(const void*, uint32), Write, WriteBuffer, "mI");

	REGIST_GLOBALFUNCTION(Script, StringFindBaseOnUTF8, "ssi:i");
	REGIST_GLOBALFUNCTION(Script, MemH16, "s:s");
	REGIST_GLOBALFUNCTION(Script, MemH64, "s:s");
	REGIST_GLOBALFUNCTION(Script, GB2U8, "s:s");

	REGIST_GLOBALFUNCTION(Script, MemH64ToMbcs, "s:s");
	REGIST_GLOBALFUNCTION(Script, MakeDirectoryUTF8, "s:b");

	REGIST_GLOBALFUNCTION(Script, find_sbc_on_utf8, "s:i");

	REGIST_GLOBALFUNCTION(Script, GetCharCount, "s:i");
	
	REGIST_GLOBALFUNCTION(Script, ErrLogMsg, "ss");
	REGIST_GLOBALFUNCTION(Script, ErrLogHeadMsg, "ss");

	REGIST_GLOBALFUNCTION(Script, GetCurrentRevision, ":I");

	REGIST_B_CLASS( Script, CDistortedTick );
	REGIST_CLASSFUNCTION( Script, CDistortedTick, SetTickName, "s" );
	REGIST_CALLBACKFUNCTION( Script, CDistortedTick, OnTick, "" );

	REGIST_GLOBALFUNCTION(Script, GetAllMemUsage, ":t");
	REGIST_GLOBALFUNCTION(Script, GetAllCapacity, ":t");
	REGIST_GLOBALFUNCTION(Script, WatchLeakage_PrintMemInfo, "");

	REGIST_ENUMTYPE(Script, EVariantType);
	REGIST_ENUMERATION(Script, EVariantType, eVT_Bool);
	REGIST_ENUMERATION(Script, EVariantType, eVT_Int8);
	REGIST_ENUMERATION(Script, EVariantType, eVT_Uint8);
	REGIST_ENUMERATION(Script, EVariantType, eVT_Int16);
	REGIST_ENUMERATION(Script, EVariantType, eVT_Uint16);
	REGIST_ENUMERATION(Script, EVariantType, eVT_Int32);
	REGIST_ENUMERATION(Script, EVariantType, eVT_Uint32);
	REGIST_ENUMERATION(Script, EVariantType, eVT_Int64);
	REGIST_ENUMERATION(Script, EVariantType, eVT_Uint64);
	REGIST_ENUMERATION(Script, EVariantType, eVT_Float);
	REGIST_ENUMERATION(Script, EVariantType, eVT_Double);
	REGIST_ENUMERATION(Script, EVariantType, eVT_String);
	REGIST_ENUMERATION(Script, EVariantType, eVT_Table);

	REGIST_B_CLASS(Script, CValueTableData);

	REGIST_B_CLASS(Script, CValueTable);
	REGIST_CLASSFUNCTION(Script, CValueTable, Release, "");
	REGIST_CLASSFUNCTION(Script, CValueTable, GetRowNum, ":I");
	REGIST_CLASSFUNCTION(Script, CValueTable, GetColNum, ":I");
	REGIST_CLASSFUNCTION(Script, CValueTable, GetBufferSize, ":I");

	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, EVariantType (CValueTable::*)(uint32) , 
		GetType, GetType, "I:I");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, const char*(CValueTable::*)(uint32, uint32) ,
		GetString, GetString, "II:s");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, bool(CValueTable::*)(uint32, uint32) ,
		IsNull, IsNull, "II:b");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, EVariantType (CValueTable::*)(const char*) , 
		GetType, GetTypeByName, "s:I");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, const char*(CValueTable::*)(uint32, const char*) ,
		GetString, GetStringByName, "Is:s");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, bool(CValueTable::*)(uint32, const char*) ,
		IsNull, IsNullByName, "Is:b");

	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, bool (CValueTable::*)(uint32, uint32), 
		GetNumber, GetBool, "II:b");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, int8 (CValueTable::*)(uint32, uint32), 
		GetNumber, GetInt8, "II:c");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, uint8 (CValueTable::*)(uint32, uint32), 
		GetNumber, GetUint8, "II:C");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, int16 (CValueTable::*)(uint32, uint32), 
		GetNumber, GetInt16, "II:h");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, uint16 (CValueTable::*)(uint32, uint32), 
		GetNumber, GetUint16, "II:H");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, int32 (CValueTable::*)(uint32, uint32), 
		GetNumber, GetInt32, "II:i");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, uint32 (CValueTable::*)(uint32, uint32), 
		GetNumber, GetUint32, "II:I");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, int64 (CValueTable::*)(uint32, uint32), 
		GetNumber, GetInt64, "II:z");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, uint64 (CValueTable::*)(uint32, uint32), 
		GetNumber, GetUint64, "II:Z");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, float (CValueTable::*)(uint32, uint32), 
		GetNumber, GetFloat, "II:f");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, double (CValueTable::*)(uint32, uint32), 
		GetNumber, GetDouble, "II:d");

	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, bool (CValueTable::*)(uint32, const char*), 
		GetNumber, GetBoolByName, "Is:b");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, int8 (CValueTable::*)(uint32, const char*), 
		GetNumber, GetInt8ByName, "Is:c");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, uint8 (CValueTable::*)(uint32, const char*), 
		GetNumber, GetUint8ByName, "Is:C");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, int16 (CValueTable::*)(uint32, const char*), 
		GetNumber, GetInt16ByName, "Is:h");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, uint16 (CValueTable::*)(uint32, const char*), 
		GetNumber, GetUint16ByName, "Is:H");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, int32 (CValueTable::*)(uint32, const char*), 
		GetNumber, GetInt32ByName, "Is:i");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, uint32 (CValueTable::*)(uint32, const char*), 
		GetNumber, GetUint32ByName, "Is:I");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, int64 (CValueTable::*)(uint32, const char*), 
		GetNumber, GetInt64ByName, "Is:z");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, uint64 (CValueTable::*)(uint32, const char*), 
		GetNumber, GetUint64ByName, "Is:Z");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, float (CValueTable::*)(uint32, const char*), 
		GetNumber, GetFloatByName, "Is:f");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, double (CValueTable::*)(uint32, const char*), 
		GetNumber, GetDoubleByName, "Is:d");

	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, CVariant* (CValueTable::*)(CValueTableData*, uint32, uint32),
		GetValue, GetValue, "(CValueTableData)II:a");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CValueTable, CVariant* (CValueTable::*)(CValueTableData*, uint32, const char*),
		GetValue, GetValueByName, "(CValueTableData)Is:a");

	//for table file
	REGIST_ENUMTYPE(Script, ETableFileType);
	REGIST_ENUMERATION(Script, ETableFileType, eTFT_Txt);
	REGIST_ENUMERATION(Script, ETableFileType, eTFT_Row);
	REGIST_ENUMERATION(Script, ETableFileType, eTFT_None);
	REGIST_ENUMERATION(Script, ETableFileType, eTFT_TypeError);


	REGIST_ENUMTYPE(Script, ETableFileFieldType);
	REGIST_ENUMERATION(Script, ETableFileFieldType, eTFFT_String);
	REGIST_ENUMERATION(Script, ETableFileFieldType, eTFFT_Number);
	REGIST_ENUMERATION(Script, ETableFileFieldType, eTFFT_Empty);

	//TableFileValue
	REGIST_B_CLASS(Script, CTableFileValue);

	//TxtTableFile
	REGIST_B_CLASS( Script, CTxtTableFile );
	REGIST_CLASSFUNCTION(Script, CTxtTableFile, Release, "");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CTxtTableFile, bool (CTxtTableFile::*)(const char*, vector<CVariant*>*), Load, Load, "sl:b");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CTxtTableFile, bool (CTxtTableFile::*)(vector<CVariant*>*), Reload, Reload, "l:b");
	REGIST_CLASSFUNCTION( Script, CTxtTableFile, GetHeight, ":i");
	REGIST_CLASSFUNCTION( Script, CTxtTableFile, GetWidth, ":i");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CTxtTableFile, ETableFileFieldType (CTxtTableFile::*)(const char*), GetType, GetTypeByName, "s:I");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CTxtTableFile, ETableFileFieldType (CTxtTableFile::*)(int32), GetType, GetType, "i:I");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CTxtTableFile, ETableFileFieldType (CTxtTableFile::*)(const char*), GetType, GetTypeByName, "s:I");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CTxtTableFile, const char* (CTxtTableFile::*)(int32, int32), GetString, GetString, "ii:s");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CTxtTableFile, const char* (CTxtTableFile::*)(int32, const char*), GetString, GetStringByName, "is:s");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CTxtTableFile, double (CTxtTableFile::*)(int32, int32, double), 
		GetNumber, GetNumber, "iid:d");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CTxtTableFile, double (CTxtTableFile::*)(int32, const char*, double), 
		GetNumber, GetNumberByName, "isd:d");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CTxtTableFile, CVariant* (CTxtTableFile::*)(CTableFileValue*, int32, int32), 
		GetValue, GetValue, "(CTableFileValue)ii:a");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CTxtTableFile, CVariant* (CTxtTableFile::*)(CTableFileValue*, int32, const char*), 
		GetValue, GetValueByName, "(CTableFileValue)is:a");

	//CMultiLangTxtTableFile
	REGIST_B_CLASS( Script, CMultiLangTxtTableFile );
	REGIST_CLASSFUNCTION(Script, CMultiLangTxtTableFile, Release, "");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CMultiLangTxtTableFile, bool (CMultiLangTxtTableFile::*)(const char*, vector<CVariant*>*), Load, Load, "sl:b");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CMultiLangTxtTableFile, bool (CMultiLangTxtTableFile::*)(vector<CVariant*>*), Reload, Reload, "l:b");
	REGIST_CLASSFUNCTION( Script, CMultiLangTxtTableFile, GetHeight, ":i");
	REGIST_CLASSFUNCTION( Script, CMultiLangTxtTableFile, GetWidth, ":i");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CMultiLangTxtTableFile, ETableFileFieldType (CMultiLangTxtTableFile::*)(const char*), GetType, GetTypeByName, "s:I");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CMultiLangTxtTableFile, ETableFileFieldType (CMultiLangTxtTableFile::*)(int32), GetType, GetType, "i:I");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CMultiLangTxtTableFile, ETableFileFieldType (CMultiLangTxtTableFile::*)(const char*), GetType, GetTypeByName, "s:I");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CMultiLangTxtTableFile, const char* (CMultiLangTxtTableFile::*)(int32, int32), GetString, GetString, "ii:s");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CMultiLangTxtTableFile, const char* (CMultiLangTxtTableFile::*)(int32, const char*), GetString, GetStringByName, "is:s");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CMultiLangTxtTableFile, double (CMultiLangTxtTableFile::*)(int32, int32, double), 
		GetNumber, GetNumber, "iid:d");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CMultiLangTxtTableFile, double (CMultiLangTxtTableFile::*)(int32, const char*, double), 
		GetNumber, GetNumberByName, "isd:d");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CMultiLangTxtTableFile, CVariant* (CMultiLangTxtTableFile::*)(CTableFileValue*, int32, int32), 
		GetValue, GetValue, "(CTableFileValue)ii:a");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CMultiLangTxtTableFile, CVariant* (CMultiLangTxtTableFile::*)(CTableFileValue*, int32, const char*), 
		GetValue, GetValueByName, "(CTableFileValue)is:a");

	

	//RowTableFile
	REGIST_ENUMTYPE(Script, ERowTableNodeType);
	REGIST_ENUMERATION(Script, ERowTableNodeType, eRTNT_Tree);
	REGIST_ENUMERATION(Script, ERowTableNodeType, eRTNT_LeafNoData);
	REGIST_ENUMERATION(Script, ERowTableNodeType, eRTNT_LeafOneData);
	REGIST_ENUMERATION(Script, ERowTableNodeType, eRTNT_LeafMoreData);

	REGIST_B_CLASS(Script, CRowTableNodeKeys);
	REGIST_CLASSFUNCTION(Script, CRowTableNodeKeys, Clear, "");

	REGIST_ABSTRACT_B_CLASS( Script, CRowTableFileNode );
	REGIST_CLASSFUNCTION(Script, CRowTableFileNode, GetFileName, ":s");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CRowTableFileNode, ERowTableNodeType (CRowTableFileNode::*)(vector<CVariant*>* ), GetType, GetType, "l:I");

	REGIST_CLASSFUNCTION_OVERLOAD( Script, CRowTableFileNode, CRowTableFileNode* (CRowTableFileNode::*)(vector<CVariant*>*), GetSubNode, GetSubNode, "l:(CRowTableFileNode)" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CRowTableFileNode, int32 (CRowTableFileNode::*)(vector<CVariant*>*), GetSubNodeNum, GetSubNodeNum, "l:i" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CRowTableFileNode, vector<CVariant*>* (CRowTableFileNode::*)(CRowTableNodeKeys*, vector<CVariant*>*), 
		GetSubNodeKeys, GetSubNodeKeys, "(CRowTableNodeKeys)l:l");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CRowTableFileNode, bool (CRowTableFileNode::*)(vector<CVariant*>*), HasSubNode, HasSubNode, "l:b" );

	REGIST_CLASSFUNCTION_OVERLOAD( Script, CRowTableFileNode, ETableFileFieldType (CRowTableFileNode::*)(vector<CVariant*>*), GetFieldType, GetFieldType, "l:I");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CRowTableFileNode, const char* (CRowTableFileNode::*)(vector<CVariant*>*), GetString, GetString, "l:s");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CRowTableFileNode, double (CRowTableFileNode::*)(vector<CVariant*>*), 
		GetNumber, GetNumber, "l:d");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CRowTableFileNode, CVariant* (CRowTableFileNode::*)(CTableFileValue*, vector<CVariant*>*), 
		GetValue, GetValue, "(CTableFileValue)l:a");

	REGIST_B_CLASS( Script, CRowTableFile );
	REGIST_CLASSFUNCTION(Script, CRowTableFile, Release, "");
	REGIST_CLASSFUNCTION( Script, CRowTableFile, GetRowTableNode, ":(CRowTableFileNode)" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CRowTableFile, bool (CRowTableFile::*)(const char*, vector<CVariant*>*), Load, Load, "sl:b");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CRowTableFile, bool (CRowTableFile::*)(vector<CVariant*>*), Reload, Reload, "l:b");

	REGIST_ABSTRACT_B_CLASS( Script, CLuaIO );
	REGIST_STATICFUNCTION( Script, CLuaIO, Open, "ss:(CLuaIO)" );
	REGIST_CLASSFUNCTION( Script, CLuaIO, Close, "" );
	REGIST_CLASSFUNCTION( Script, CLuaIO, Flush, "" );
	REGIST_CLASSFUNCTION( Script, CLuaIO, ReadAll, ":s" );
	REGIST_CLASSFUNCTION( Script, CLuaIO, ReadNum, ":d" );
	REGIST_CLASSFUNCTION( Script, CLuaIO, ReadLine, ":s" );
	REGIST_CLASSFUNCTION( Script, CLuaIO, WriteNum, "d" );
	REGIST_CLASSFUNCTION( Script, CLuaIO, WriteString, "s" );
	REGIST_CLASSFUNCTION( Script, CLuaIO, Seek, "is:i" );
	
	REGIST_GLOBALFUNCTION(Script, GetTableFileType, "ss:I");

	
	REGIST_ISOLATED_CLASS(Script, CXmlSiblingElemIter);
	REGIST_ISOLATED_CLASS(Script, CXmlConfig);
	REGIST_ISOLATED_CLASS(Script, CItemTableXml);
	REGIST_ISOLATED_CLASS(Script, CXmlReadAndWrite);

	REGIST_CLASSFUNCTION(Script, CItemTableXml, GetItemXmlNumber, ":I");
	REGIST_CLASSFUNCTION(Script, CItemTableXml, GetItemXmlAppName, "I:s");
	REGIST_CLASSFUNCTION(Script, CItemTableXml, GetItemXmlLanName, "I:s");
	REGIST_CLASSFUNCTION(Script, CItemTableXml, GetItemXmlBigId, "I:I");

	REGIST_CLASSFUNCTION(Script, CXmlReadAndWrite, XmlRead, "s:s");
	REGIST_CLASSFUNCTION(Script, CXmlReadAndWrite, XmlReadLast, "s:s");
	REGIST_CLASSFUNCTION(Script, CXmlReadAndWrite, XmlReadNext, "s:s");
	REGIST_CLASSFUNCTION(Script, CXmlReadAndWrite, XmlNodeNumber, ":I");
	REGIST_CLASSFUNCTION(Script, CXmlReadAndWrite, XmlReadByNumber, "I:s");
	REGIST_CLASSFUNCTION(Script, CXmlReadAndWrite, XmlReadContentByNumber, "I:s");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CXmlReadAndWrite, void (CXmlReadAndWrite::*)(const char *), XmlWrite,XmlWrite_1, "s");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CXmlReadAndWrite, void (CXmlReadAndWrite::*)(const char *,const char *), XmlWrite,XmlWrite_2, "ss");
	REGIST_CLASSFUNCTION(Script, CXmlReadAndWrite, XmlOpen, "s");
	REGIST_CLASSFUNCTION(Script, CXmlReadAndWrite, XmlClose, "s");
	REGIST_CLASSFUNCTION(Script, CXmlReadAndWrite, XmlNodeIsExist, ":b");
	REGIST_CLASSFUNCTION(Script, CXmlReadAndWrite, XmlReadLastNode, ":s");

	REGIST_CLASSFUNCTION(Script, CXmlSiblingElemIter, MoveNext, "");
	REGIST_CLASSFUNCTION(Script, CXmlSiblingElemIter, IsEnd, ":b");
	REGIST_CLASSFUNCTION(Script, CXmlSiblingElemIter, Get, "s:s");
	REGIST_CLASSFUNCTION(Script, CXmlSiblingElemIter, GetSiblingName,":s");
	

	REGIST_CLASSFUNCTION(Script, CXmlConfig, LoadConfigFormString, "ss:b");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CXmlConfig, const char*(CXmlConfig::*)(const char*) const, Get, GetStr_1, "s:s");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CXmlConfig, const char*(CXmlConfig::*)(const char*, const char*) const, Get, GetStr_2, "ss:s");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CXmlConfig, const char*(CXmlConfig::*)(const char*, const char*, const char*) const, Get, GetStr_3, "sss:s");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CXmlConfig, void (CXmlConfig::*)(CXmlSiblingElemIter*,const char*) const, GetIter, GetIter_1, "(CXmlSiblingElemIter)s");
	REGIST_CLASSFUNCTION_OVERLOAD(Script, CXmlConfig, void (CXmlConfig::*)(CXmlSiblingElemIter*, const char*, const char*) const, GetIter, GetIter_2, "(CXmlSiblingElemIter)ss");
	return 1;
}
