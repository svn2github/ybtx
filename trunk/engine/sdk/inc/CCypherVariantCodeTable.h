#pragma once
#include "BaseDefs.h"
#include "CommonDefs.h"
#include "CCypherVariantMallocObject.h"
#include "CVariantMallocObject.h"
#include "TCypherVariantAllocator.h"
#include "TVariantAllocator.h"

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4251)
#endif
class TiXmlNode;

namespace sqr
{
	class IdMaker;
	class CCypherVariantCodeTable;

	class COMMON_API CBaseCypherVariantCode
		//: public CCypherVariantMallocObject
		: public CVariantMallocObject
	{
		friend class CCypherVariantCodeTable;
		friend class CCypherVariant;
		friend class CCypherVariantHolder;
		friend class CCypherVariantHolderForDecode;
		friend class CCypherVariantCode;
		friend class CCypherVariantDynCode;
		friend class CCypherVariantTreeRootCodeForEncode;
	protected:
		CBaseCypherVariantCode(uint32 uID,const char* szName, CBaseCypherVariantCode *pParent);/* constructor */
		virtual ~CBaseCypherVariantCode();

	public:
		virtual bool IsDynCode() const {return false;}
		virtual bool IsTreeRoot()const {return false;}
		virtual bool IsCloned()const{return false;}
		virtual bool HasDynChildCode()const{return false;}

		inline uint32 GetID() const
		{
			return m_uID;
		}
		virtual const char* GetName() const
		{
			return m_szName.c_str();
		}
		inline CBaseCypherVariantCode* GetParent() const
		{
			return m_pParent;
		}
		inline uint32 GetIndex()const{
			return m_uIndex;
		}
		//virtual CBaseCypherVariantCode* GetEntity(uint32 uID) const = 0;
		CBaseCypherVariantCode* GetEntity(const char* szName) const;

		template < class PathContainer >
		void GetPath(PathContainer &PathElems)
		{
			CBaseCypherVariantCode *pCode = this;
			while (pCode->GetParent())
			{
				PathElems.push_front(pCode->GetName());
				pCode = pCode->GetParent();
			}
		}
		uint32 GetIDByPath(const char* szPath) const;
		virtual void DelEntity(const char* szName){}
		size_t GetChildrenCount()const;

	protected:

#ifdef _WIN32
		struct string_less : public binary_function<std::string, std::string, bool>
		{
			bool operator()(const std::string& szLeft, const std::string& szRight) const 
			{
				return szLeft < szRight;
			}
		};

		typedef hash_map<std::string, CBaseCypherVariantCode*, hash_compare<std::string, string_less>, 
			TCypherVariantAllocator<pair<std::string, CBaseCypherVariantCode*> > >	NameMapType;

#else
		struct string_hash     
		{                                                                                                                            
			size_t operator()(const std::string& str) const                                                                               	{                                                                                                                        
				return __stl_hash_string(str.c_str());                                                                               
			}                                                                                                                        
		};    

		struct string_equal
		{
			bool operator()(const std::string& szLeft, const std::string& szRight) const
			{
				return szLeft == szRight;
			}
		};

		typedef hash_map<std::string, CBaseCypherVariantCode*, string_hash, string_equal, 
			TCypherVariantAllocator<pair<std::string, CBaseCypherVariantCode*> > > NameMapType;

#endif

		//typedef std::map<std::string, CBaseCypherVariantCode*, less<std::string>, TCypherVariantAllocator<pair<std::string, CBaseCypherVariantCode*> > > NameMapType;

		uint32 m_uID;
		std::string m_szName;
		CBaseCypherVariantCode *m_pParent;
		NameMapType m_NameMap;
		uint32 m_uIndex;
	};

	class COMMON_API CCypherVariantCode : public CBaseCypherVariantCode
	{
		friend class CCypherVariantHolder;
		friend class CCypherVariantHolderForDecode;
		friend class CCypherVariantCodeTable;
	public:
		CCypherVariantCode(uint32 uID,const char* szName, CCypherVariantCode *pParent);/* constructor */
		virtual ~CCypherVariantCode();

		void AddEntity(CBaseCypherVariantCode *pEntity);
		virtual CBaseCypherVariantCode* GetEntity(uint32 uID) const;

	private:
		typedef std::vector<CBaseCypherVariantCode*,TCypherVariantAllocator<CBaseCypherVariantCode*> > CodeVec_t;

		CodeVec_t m_IDMap;
		uint32	m_uFirstChildID;
	};

	class CCypherVariantDynCodeForEncode;

	class COMMON_API CCypherVariantDynCode : public CBaseCypherVariantCode
	{
		friend class CCypherVariant;
		friend class CCypherVariantHolder;		
		friend class CCypherVariantCodeTable; 
		friend class CCypherVariantDynCodeForEncode;
	public:
		virtual bool IsCloned()const{return m_bIsClone;}
		virtual bool IsDynCode() const {return true;}
		virtual bool HasDynChildCode()const{return !(m_NameMap.empty());}

		void AddEntity(CCypherVariantDynCode* pEntity, uint32 slot);
		//virtual CBaseCypherVariantCode* GetEntity(uint32 uId)const;
		virtual CBaseCypherVariantCode* GetEntity(const char* )const;
		virtual void DelEntity(const char* szName);
		virtual void clear();
		virtual CCypherVariantDynCode* clone(CBaseCypherVariantCode* pParent);

		static void InterpretSubTreePath(uint32 uFullPath, uint32& uParentIndex, uint32& uSubTreeIndex, uint32& 
uSubTreeType, uint32& uIndex);
		static uint32 GenerateSubTreePath(uint32 uParentIndex, uint32 uSubTreeIndex, uint32 uSubTreeType, uint32 uIndex);

	protected:
		typedef std::vector<CBaseCypherVariantCode*, TCypherVariantAllocator<CBaseCypherVariantCode*> > SubTreeNode_t;

		CCypherVariantDynCode(uint32 uID, const char* szName, const char* szNewName, bool bHasDynChildCode, CBaseCypherVariantCode *pParent);
		CCypherVariantDynCode(uint32 uID, const char* szName, bool bHasDynChildCode, CBaseCypherVariantCode *pParent);
		~CCypherVariantDynCode();
		uint32 IntDelEntity(const char* szName);

		//SubTreeNode_t m_vecSubTreeNode;
		uint32 m_uSlot;
		uint32 m_uSubNodeCount;
		bool m_bIsClone;
		bool m_bHasDynChildCode;

	private:
		//void DelSubTree(CCypherVariantDynCodeForEncode* pCode);
	};

	class COMMON_API CCypherVariantDynCodeForEncode : public CCypherVariantDynCode
	{
		friend class CCypherVariant;		
		friend class CCypherVariantHolder;		
		friend class CCypherVariantCodeTable; 
		friend class CCypherVariantDynCode;
	public:
		//CCypherVariantDynCodeForEncode(uint32 uID, uint32 uDynamicId, const char* szName, const char* szNewName, bool bHasDynChildCode, CBaseCypherVariantCode *pParent);
		CCypherVariantDynCodeForEncode(uint32 uID, uint32 uDynamicId, const char* szName, bool bHasDynChildCode, CBaseCypherVariantCode *pParent);
		~CCypherVariantDynCodeForEncode();

		virtual bool IsCloned()const{return m_bIsClone;}

		virtual void DelEntity(const char* szName);
		inline uint32 GetDynamicId()const{ return m_uDynamicId;}
		virtual CCypherVariantDynCodeForEncode* clone(CBaseCypherVariantCode* pParent);

	protected:
		uint32 m_uDynamicId;
	};

	class COMMON_API CCypherVariantTreeRootCode 
	{
	protected:
		CCypherVariantTreeRootCode(){}
		virtual ~CCypherVariantTreeRootCode(){}

		void AddSubTreeType(const std::string&);
		bool ValidateSubTreeType(const std::string&)const ;
		std::string GetSubTreeName(size_t index)const ;

		typedef std::vector<std::string, TVariantAllocator<std::string> > SubTreeType_t;
		SubTreeType_t m_vecSubTreeType;
	};

	class COMMON_API CCypherVariantTreeRootCodeForEncode 
		: public CCypherVariantDynCodeForEncode
		, protected CCypherVariantTreeRootCode
	{
		friend class CCypherVariant;
		friend class CCypherVariantHolder;
		friend class CCypherVariantCodeTable;
	public:	
		CCypherVariantTreeRootCodeForEncode(uint32 uId, uint32 uDynamicId, const char* szName, 
CBaseCypherVariantCode* pParent);
		~CCypherVariantTreeRootCodeForEncode();
		virtual bool IsTreeRoot()const {return true;}
		virtual bool IsDynCode() const {return false;}

		virtual void DelEntity(const char* szName);
		virtual void clear();
		uint32 GetNewChildId();

		CCypherVariantDynCodeForEncode* clone(CBaseCypherVariantCode* pParent);
	private:
		IdMaker* m_IdMaker;
	};

	class COMMON_API CCypherVariantTreeRootCodeForDecode 
		: public CCypherVariantDynCode
		, protected CCypherVariantTreeRootCode
	{
		friend class CCypherVariant;
		friend class CCypherVariantHolderForDecode;
		friend class CCypherVariantCodeTable;
	public:	
		CCypherVariantTreeRootCodeForDecode(uint32 uId, const char* szName, CBaseCypherVariantCode* pParent);
		~CCypherVariantTreeRootCodeForDecode();

		virtual bool IsTreeRoot()const {return true;}
		virtual bool IsDynCode() const {return false;}

		CCypherVariantDynCode* clone(CBaseCypherVariantCode* pParent);
	};

	class COMMON_API CCypherVariantCodeTable
		: public CCypherVariantMallocObject
		//: public CVariantMallocObject
	{
		friend class CCypherVariantHolder;
		friend class CCypherVariantHolderForDecode;
	public:
		CCypherVariantCodeTable();
		~CCypherVariantCodeTable();                             /* destructor */

		bool LoadConfig(istream& strm, istream& subTree, const char* sRootName);
		CBaseCypherVariantCode* GetRoot() const;
		bool AddEntity(CBaseCypherVariantCode*);
		CBaseCypherVariantCode* GetEntity(uint32) const;
		void DeleteAll();
		
	private:
		typedef std::vector<CBaseCypherVariantCode*, TVariantAllocator<CBaseCypherVariantCode*> > CodeVec_t;
		bool SetSubTreeRootInfo(TiXmlNode &Node, CCypherVariantCode* pParent);
		bool LoadChildNode(TiXmlNode &Node, CCypherVariantCode* pParent);
		bool LoadSubTree(TiXmlNode& Node, CCypherVariantCode* pParent);
		CodeVec_t m_IDMap;
		CodeVec_t m_DynamicMap;
		CodeVec_t m_SubTreeNode;
	};
}

#ifdef _WIN32
#pragma warning(pop)
#endif

