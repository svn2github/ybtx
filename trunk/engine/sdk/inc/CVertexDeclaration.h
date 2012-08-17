#pragma once
#include "GraphicBase.h"
#include "CVertexHardwareBuffer.h"

namespace sqr
{
	class CVertexElement : public CGraphicMallocObject
	{
	public:
		static size_t GetTypeSize(CVertexElementType etype);
		static USHORT GetTypeCount(CVertexElementType etype);
		static CVertexElementType MultiplyTypeCount(CVertexElementType baseType, USHORT count);
		static CVertexElementType GetBaseType(CVertexElementType multiType);
		static DWORD GetElementSemanticID(CVertexElementSemantic semantic, USHORT index);
		static DWORD GetBaseFormatID(uint Source, CVertexElementSemantic semantic, USHORT index = 0);
	protected:
		/// 用来检索顶点缓存
		USHORT m_Source;
		/// 顶点起始偏移量
		size_t m_Offset;

		CVertexElementType m_Type;
		CVertexElementSemantic m_Semantic;
		/// 元素索引，只有在纹理坐标时有效
		USHORT m_Index;
	public:
		CVertexElement(USHORT source, size_t offset, CVertexElementType theType,
			CVertexElementSemantic semantic, USHORT index = 0);

		USHORT GetSource(void) const;
		size_t GetOffset(void) const;



		CVertexElementType GetType(void) const;
		CVertexElementSemantic GetSemantic(void) const;

		USHORT GetIndex(void) const;
		size_t GetSize(void) const;

		bool operator== (const CVertexElement& rhs) const;
		void* GetVertexPtr(void* pBase) const;
	private:
	};

	//-------------------------------------------------------------------------------------
	class CVertexDeclaration : public CGraphicMallocObject
	{
	public:
		typedef GList<CVertexElement> VertexElementList;
		typedef GMap<DWORD,CVertexDeclaration*> VerDeclMap;
		///用于排列顶点元素
		static bool CVertexElementLess(const CVertexElement& e1, const CVertexElement& e2);
	protected:
		VertexElementList	m_ElementList;
		bool				m_bIsChanged;
		const	DWORD		m_FVF;

	public:
		CVertexDeclaration(DWORD FVF);
		virtual ~CVertexDeclaration();
		virtual void update() = 0;

		size_t GetElementCount(void);
		const VertexElementList& GetElements(void) const;
		const CVertexElement* GetElement(unsigned short index);


		//针对渲染API和显卡进行最大限度排序
		void Sort(void);
		//优化将去掉描述中的数据源的间隙
		void Optimize(void);
		//将描述重新排序
		void Reform(void);

		inline	DWORD GetVertexFormatID(void)
		{
			return m_FVF;
		}


		USHORT GetMaxSource(void) const;



		CVertexElement& AddElement(USHORT source,  size_t offset, CVertexElementType theType,
			CVertexElementSemantic semantic, USHORT index = 0);


		CVertexElement& InsertElement(USHORT atPosition,
			USHORT source, size_t offset, CVertexElementType theType,
			CVertexElementSemantic semantic, USHORT index = 0);


		void RemoveElement(USHORT elem_index);
		void RemoveElement(CVertexElementSemantic semantic, USHORT index = 0);
		void RemoveAllElements(void);

		void ModifyElement(USHORT elem_index, USHORT source, size_t offset, CVertexElementType theType,
			CVertexElementSemantic semantic, USHORT index = 0);

		const CVertexElement* FindElementBySemantic(CVertexElementSemantic sem, USHORT index = 0);
		VertexElementList FindElementsBySource(USHORT source);
		size_t GetVertexSize(USHORT source);
		CVertexDeclaration* Clone(void);


		bool operator== (const CVertexDeclaration& rhs) const;
		bool operator!= (const CVertexDeclaration& rhs) const;


		bool  IsChanged();
		void  NeedRebuild();

		static bool REGISTER_VERDECL(CVertexDeclaration* decl);
		static CVertexDeclaration* GET_VERDECL(DWORD KEY);
		static void RELEASE_ALL_VERDECL(void);
	private:

	protected:
		static VerDeclMap st_verDecls;
	};

	//----------------------------------------------------------------------------------
	class CVertexBufferBinding : public CGraphicMallocObject
	{
	public:
		typedef GMap<USHORT, BufferBindingInfo> VertexBufferBindingMap;
	protected:
		VertexBufferBindingMap m_BindingMap;
		USHORT				   m_HighIndex;
		bool				   m_bChanged;
	public:
		CVertexBufferBinding();
		~CVertexBufferBinding();

		void SetBinding(USHORT index,CVertexHardwareBuffer* buffer, size_t offset, size_t lenght, size_t versize);
		void SetBinding(USHORT index,CVertexHardwareBuffer* buffer, size_t offset = 0);
		void UnsetBinding(USHORT index);


		void UnsetAllBindings(void);
		const VertexBufferBindingMap& GetBindings(void) const;

		CVertexHardwareBuffer*		GetBuffer(USHORT index) const;
		bool IsBufferBound(USHORT index) const;

		size_t GetBufferCount(void) const;
		USHORT GetNextIndex(void) const;

		USHORT GetLastBoundIndex(void) const;

		typedef GMap<USHORT, USHORT> BindingIndexMap;


		bool HasGaps(void) const;
		void CloseGaps(BindingIndexMap& bindingIndexMap);

		void DestroyAllBuffer(void);
		bool NeedUpdate(void);
		void Update(void);
	protected:
	private:
	};

	inline bool CVertexBufferBinding::NeedUpdate(void)
	{
		return m_bChanged;
	}

	inline void CVertexBufferBinding::Update(void)
	{
		m_bChanged = false;
	}
}
