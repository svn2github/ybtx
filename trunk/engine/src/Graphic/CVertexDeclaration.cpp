#include "stdafx.h"
#include "CVertexDeclaration.h"
#include "CVertexHardwareBuffer.h"
#include "CIndexHardwareBuffer.h"
#include "CHardwareBufferMgr.h"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"

//-------------------------------------------------------------
// CVertexElement部分
// 说明：
//-------------------------------------------------------------
size_t CVertexElement::GetTypeSize(CVertexElementType etype)
{
	switch (etype)
	{
		case VET_COLOUR:
			return sizeof(DWORD);
		case VET_FLOAT1:
			return sizeof(float);
		case VET_FLOAT2:
			return sizeof(float)*2;
		case VET_FLOAT3:
			return sizeof(float)*3;
		case VET_FLOAT4:
			return sizeof(float)*4;
		case VET_SHORT1:
			return sizeof(short);
		case VET_SHORT2:
			return sizeof(short)*2;
		case VET_SHORT3:
			return sizeof(short)*3;
		case VET_SHORT4:
			return sizeof(short)*4;
		case VET_UBYTE4:
			return sizeof(TBYTE)*4;
	}
	return 0;
}

USHORT CVertexElement::GetTypeCount(CVertexElementType etype)
{
	switch (etype)
	{
		case VET_COLOUR:
			return 1;
		case VET_FLOAT1:
			return 1;
		case VET_FLOAT2:
			return 2;
		case VET_FLOAT3:
			return 3;
		case VET_FLOAT4:
			return 4;
		case VET_SHORT1:
			return 1;
		case VET_SHORT2:
			return 2;
		case VET_SHORT3:
			return 3;
		case VET_SHORT4:
			return 4;
		case VET_UBYTE4:
			return 4;
	}
	return 1;
}

CVertexElementType CVertexElement::MultiplyTypeCount(CVertexElementType baSetype, USHORT count)
{
	switch (baSetype)
	{
		case VET_FLOAT1:
			switch (count)
			{
				case 1:
					return VET_FLOAT1;
				case 2:
					return VET_FLOAT2;
				case 3:
					return VET_FLOAT3;
				case 4:
					return VET_FLOAT4;
				default:
					break;
			}
			break;
		case VET_SHORT1:
			switch (count)
			{
				case 1:
					return VET_SHORT1;
				case 2:
					return VET_SHORT2;
				case 3:
					return VET_SHORT3;
				case 4:
					return VET_SHORT4;
				default:
					break;
			}
			break;
		default:

			break;
	}
	return baSetype;
}

CVertexElementType CVertexElement::GetBaseType(CVertexElementType multiType)
{
	switch (multiType)
	{
		case VET_FLOAT1:
		case VET_FLOAT2:
		case VET_FLOAT3:
		case VET_FLOAT4:
			return VET_FLOAT1;
		case VET_COLOUR:
			return VET_COLOUR;
		case VET_SHORT1:
		case VET_SHORT2:
		case VET_SHORT3:
		case VET_SHORT4:
			return VET_SHORT1;
		case VET_UBYTE4:
			return VET_UBYTE4;
	};
	// To keep compiler happy
	return multiType;

}

DWORD CVertexElement::GetElementSemanticID(CVertexElementSemantic semantic, USHORT index)
{
	switch (semantic)
	{
		case VES_POSITION:
			return FVF_XYZ;
		case VES_POSITIONT:
			return FVF_XYZRHW;
		case VES_BLEND_WEIGHTS:
			return FVF_XYZB4;
		case VES_BLEND_INDICES:
			return FVF_LASTBETA_UBYTE4;
		case VES_NORMAL:
			return FVF_NORMAL;
		case VES_DIFFUSE:
			return FVF_DIFFUSE;
		case VES_SPECULAR:
			return FVF_SPECULAR;
		case VES_TEXTURE_COORDINATES:
			return FVF_TEX0 + ((index+1)<<8);
		case VES_COLOR:
			return FVF_DIFFUSE + ((index+1)<<8);
		case VES_BINORMAL: //暂时还没用
			return 0;
		case VES_TANGENT://暂时还没用
			return 0;
	}
	return 0;
}

DWORD CVertexElement::GetBaseFormatID(uint Source, CVertexElementSemantic semantic, USHORT index)
{
	return SOURCE_MULTI_STREAM(Source)|GetElementSemanticID(semantic,index);
}

CVertexElement::CVertexElement(USHORT source, size_t offSet, CVertexElementType theType,
                               CVertexElementSemantic semantic, USHORT index )
		: m_Source(source), m_Offset(offSet), m_Type(theType),
		m_Semantic(semantic), m_Index(index)
{
}

USHORT CVertexElement::GetSource(void) const
{
	return m_Source;
}
size_t CVertexElement::GetOffset(void) const
{
	return m_Offset;
}

CVertexElementType CVertexElement::GetType(void) const
{
	return m_Type;
}
CVertexElementSemantic CVertexElement::GetSemantic(void) const
{
	return m_Semantic;
}
USHORT CVertexElement::GetIndex(void) const
{
	return m_Index;
}

size_t CVertexElement::GetSize(void) const
{
	return GetTypeSize(m_Type);
}

bool CVertexElement::operator== (const CVertexElement& rhs) const
{
	if (m_Type != rhs.m_Type || m_Semantic != rhs.m_Semantic ||
	        m_Offset != rhs.m_Offset || m_Index != rhs.m_Index ||
	        m_Source != rhs.m_Source)
		return false;
	else
		return true;
}

void* CVertexElement::GetVertexPtr(void* pBase) const
{
	return (void*)((UCHAR*)(pBase) + m_Offset);
}

//-------------------------------------------------------------
// CVertexDeclaration部分
// 说明：
//-------------------------------------------------------------
bool CVertexDeclaration::CVertexElementLess(const CVertexElement& e1, const CVertexElement& e2)
{
	if (e1.GetSource() < e2.GetSource())
	{
		return true;
	}
	else if (e1.GetSource() == e2.GetSource())
	{
		if (e1.GetSemantic() < e2.GetSemantic())
		{
			return true;
		}
		else if (e1.GetSemantic() == e2.GetSemantic())
		{
			if (e1.GetIndex() < e2.GetIndex())
			{
				return true;
			}
		}
	}
	return false;
}
CVertexDeclaration::CVertexDeclaration(DWORD FVF)
		: m_bIsChanged(true)
		, m_FVF(FVF)
{
	CVertexDeclaration::REGISTER_VERDECL(this);
}
CVertexDeclaration::~CVertexDeclaration()
{
}

size_t CVertexDeclaration::GetElementCount(void)
{
	return m_ElementList.size();
}

const CVertexDeclaration::VertexElementList& CVertexDeclaration::GetElements(void) const
{
	return m_ElementList;
}

const CVertexElement* CVertexDeclaration::GetElement(USHORT index)
{
	Ast( index < m_ElementList.size() && "CVertexDeclaration::GetElement 出界");
	VertexElementList::iterator i = m_ElementList.begin();
	std::advance(i, index);
	return &(*i);
}

//针对渲染API和显卡进行最大限度排序
void CVertexDeclaration::Sort(void)
{
	m_ElementList.sort(CVertexDeclaration::CVertexElementLess);
}

//优化将去掉描述中的数据源的间隙
void CVertexDeclaration::Optimize(void)
{
	if (m_ElementList.empty())
		return;

	Sort();

	VertexElementList::iterator i, iend;
	iend = m_ElementList.end();
	USHORT tarGetIdx = 0;
	USHORT lastIdx = GetElement(0)->GetSource();
	USHORT c = 0;
	for (i = m_ElementList.begin(); i != iend; ++i, ++c)
	{
		CVertexElement& elem = *i;
		if (lastIdx != elem.GetSource())
		{
			++tarGetIdx;
			lastIdx = elem.GetSource();
		}
		if (tarGetIdx != elem.GetSource())
		{
			ModifyElement(c, tarGetIdx, elem.GetOffset(), elem.GetType(),
			              elem.GetSemantic(), elem.GetIndex());
		}
	}
}
//将描述重新排序
void CVertexDeclaration::Reform(void)
{
	CVertexDeclaration::VertexElementList::const_iterator i;
	unsigned short c = 0;
	for (i = m_ElementList.begin(); i != m_ElementList.end(); ++i, ++c)
	{
		const CVertexElement& elem = *i;
		ModifyElement(c, 0, 0, i->GetType(), elem.GetSemantic(), elem.GetIndex());
	}

	Sort();
	size_t offSet = 0;
	c = 0;
	USHORT buffer = 0;
	CVertexElementSemantic prevSemantic = VES_POSITION;

	for (i = m_ElementList.begin(); i != m_ElementList.end(); ++i, ++c)
	{
		const CVertexElement& elem = *i;

		bool splitWithPrev = false;
		bool splitWithNext = false;
		switch (elem.GetSemantic())
		{
			case VES_POSITION:
				splitWithPrev = true;
				splitWithNext = true;
				break;
			case VES_NORMAL:
				// 法向量不能和混合权重共面
				splitWithPrev = (prevSemantic == VES_BLEND_WEIGHTS || prevSemantic == VES_BLEND_INDICES);
				// All animated meshes have to split after normal
				splitWithNext = true;
				break;
			case VES_BLEND_WEIGHTS:
				// Blend weights/indices can be sharing with their own buffer only
				splitWithPrev = true;
				break;
			case VES_BLEND_INDICES:
				// Blend weights/indices can be sharing with their own buffer only
				splitWithNext = true;
				break;
		}

		if (splitWithPrev && offSet)
		{
			++buffer;
			offSet = 0;
		}

		prevSemantic = elem.GetSemantic();
		ModifyElement(c, buffer, offSet,
		              elem.GetType(), elem.GetSemantic(), elem.GetIndex());

		if (splitWithNext)
		{
			++buffer;
			offSet = 0;
		}
		else
		{
			offSet += elem.GetSize();
		}
	}
}


USHORT CVertexDeclaration::GetMaxSource(void) const
{
	VertexElementList::const_iterator i, iend;
	iend = m_ElementList.end();
	USHORT ret = 0;
	for (i = m_ElementList.begin(); i != iend; ++i)
	{
		if (i->GetSource() > ret)
			ret = i->GetSource();
	}
	return ret;
}

CVertexElement& CVertexDeclaration::AddElement(USHORT source, size_t offSet,
        CVertexElementType theType, CVertexElementSemantic semantic, USHORT index )
{
	m_ElementList.push_back(
	    CVertexElement(source, offSet, theType, semantic, index)
	);
	m_bIsChanged = true;
	return m_ElementList.back();
}

CVertexElement& CVertexDeclaration::InsertElement(USHORT atPosition,
        USHORT source, size_t offSet, CVertexElementType theType,
        CVertexElementSemantic semantic, USHORT index )
{
	if (atPosition >= m_ElementList.size())
	{
		return AddElement(source, offSet, theType, semantic, index);
	}

	VertexElementList::iterator i = m_ElementList.begin();

	std::advance(i, atPosition);
	i = m_ElementList.insert(i,
	                         CVertexElement(source, offSet, theType, semantic, index));
	m_bIsChanged = true;
	return *i;
}

void CVertexDeclaration::RemoveElement(USHORT elem_index)
{
	Ast(elem_index < m_ElementList.size() && "CVertexDeclaration::removeElement 出界");
	VertexElementList::iterator i = m_ElementList.begin();
	std::advance(i, elem_index);
	m_bIsChanged = true;
	m_ElementList.erase(i);
}

void CVertexDeclaration::RemoveElement(CVertexElementSemantic semantic, USHORT index )
{
	VertexElementList::iterator ei, eiend;
	eiend = m_ElementList.end();
	for (ei = m_ElementList.begin(); ei != eiend; ++ei)
	{
		if (ei->GetSemantic() == semantic && ei->GetIndex() == index)
		{
			m_bIsChanged = true;
			m_ElementList.erase(ei);
			break;
		}
	}
}

void CVertexDeclaration::RemoveAllElements(void)
{
	m_bIsChanged = true;
	m_ElementList.clear();
}

void CVertexDeclaration::ModifyElement(USHORT elem_index, USHORT source, size_t offSet, CVertexElementType theType,
                                       CVertexElementSemantic semantic, USHORT index )
{
	Ast(elem_index < m_ElementList.size() && "CVertexDeclaration::modifyElement 出界");
	VertexElementList::iterator i = m_ElementList.begin();
	std::advance(i, elem_index);
	m_bIsChanged = true;
	(*i) = CVertexElement(source, offSet, theType, semantic, index);
}

const CVertexElement* CVertexDeclaration::FindElementBySemantic(CVertexElementSemantic sem, USHORT index )
{
	VertexElementList::const_iterator ei, eiend;
	eiend = m_ElementList.end();
	for (ei = m_ElementList.begin(); ei != eiend; ++ei)
	{
		if (ei->GetSemantic() == sem && ei->GetIndex() == index)
		{
			return &(*ei);
		}
	}
	return NULL;
}

CVertexDeclaration::VertexElementList CVertexDeclaration::FindElementsBySource(USHORT source)
{
	VertexElementList retList;
	VertexElementList::const_iterator ei, eiend;
	eiend = m_ElementList.end();
	for (ei = m_ElementList.begin(); ei != eiend; ++ei)
	{
		if (ei->GetSource() == source)
			retList.push_back(*ei);
	}
	return retList;
}

size_t CVertexDeclaration::GetVertexSize(USHORT source)
{
	VertexElementList::const_iterator i, iend;
	iend = m_ElementList.end();
	size_t sz = 0;

	for (i = m_ElementList.begin(); i != iend; ++i)
	{
		if (i->GetSource() == source)
			sz += i->GetSize();
	}
	return sz;
}

bool CVertexDeclaration::operator== (const CVertexDeclaration& rhs) const
{
	if (m_ElementList.size() != rhs.m_ElementList.size())
		return false;

	VertexElementList::const_iterator i, iend, rhsi, rhsiend;
	iend = m_ElementList.end();
	rhsiend = rhs.m_ElementList.end();
	rhsi = rhs.m_ElementList.begin();
	for (i = m_ElementList.begin(); i != iend && rhsi != rhsiend; ++i, ++rhsi)
	{
		if ( !(*i == *rhsi) )
			return false;
	}
	return true;
}

bool CVertexDeclaration::operator!= (const CVertexDeclaration& rhs) const
{
	return !(*this == rhs);
}

bool  CVertexDeclaration::IsChanged(void)
{
	return m_bIsChanged;
}

void CVertexDeclaration:: NeedRebuild()
{
	m_bIsChanged = true;
}



//-------------------------------------------------------------

CVertexDeclaration::VerDeclMap CVertexDeclaration::st_verDecls;

bool CVertexDeclaration::REGISTER_VERDECL(CVertexDeclaration* decl)
{
	if (NULL==decl)
		return false;

	VerDeclMap::iterator it = st_verDecls.find(decl->GetVertexFormatID());
	if (it!=st_verDecls.end())
	{
		if (it->second!=decl)
		{
			delete it->second;
			it->second = decl;
		}
	}
	else
		st_verDecls.insert(VerDeclMap::value_type(decl->GetVertexFormatID(),decl));
	return true;
}

CVertexDeclaration* CVertexDeclaration::GET_VERDECL(DWORD KEY)
{
	return st_verDecls[KEY];
}

void CVertexDeclaration::RELEASE_ALL_VERDECL(void)
{
	VerDeclMap::iterator it,eit = st_verDecls.end();
	for (it = st_verDecls.begin();it!=eit;++it)
		delete it->second;
}
//-------------------------------------------------------------
// CVertexBufferBinding部分
// 说明：
//-------------------------------------------------------------
CVertexBufferBinding::CVertexBufferBinding()
: m_HighIndex(0)
, m_bChanged(true)
{
}

CVertexBufferBinding::~CVertexBufferBinding()
{
	m_BindingMap.clear();
}

void CVertexBufferBinding::SetBinding(USHORT index,CVertexHardwareBuffer* buffer, size_t offSet)
{
	m_BindingMap[index] = BufferBindingInfo(buffer,buffer->getVertexSize(),offSet,buffer->getSize() - offSet);
	m_HighIndex =  max(m_HighIndex, index+1);
	m_bChanged = true;
}

void CVertexBufferBinding::SetBinding(USHORT index,CVertexHardwareBuffer* buffer, size_t offSet, size_t lenght, size_t versize)
{
	m_BindingMap[index] = BufferBindingInfo(buffer,versize,offSet,lenght);
	m_HighIndex =  max(m_HighIndex, index+1);
	m_bChanged = true;
}

void CVertexBufferBinding::UnsetBinding(USHORT index)
{
	VertexBufferBindingMap::iterator i = m_BindingMap.find(index);
	if (i == m_BindingMap.end())
	{
		Ast("CVertexBufferBinding::unSetBinding 查找失败");
		return;
	}
	m_bChanged = true;
	m_BindingMap.erase(i);
}

void CVertexBufferBinding::UnsetAllBindings(void)
{
	m_BindingMap.clear();
	m_HighIndex = 0;
	m_bChanged = false;
}

const CVertexBufferBinding::VertexBufferBindingMap& CVertexBufferBinding::GetBindings(void) const
{
	return m_BindingMap;
}

CVertexHardwareBuffer* CVertexBufferBinding::GetBuffer(USHORT index) const
{
	VertexBufferBindingMap::const_iterator i = m_BindingMap.find(index);
	if (i == m_BindingMap.end())
	{
		//Ast("CVertexBufferBinding::GetBuffer 查找失败");
		return NULL;
	}
	else
		return i->second.m_pBuffer;
}

bool CVertexBufferBinding::IsBufferBound(USHORT index) const
{
	return m_BindingMap.find(index) != m_BindingMap.end();
}

size_t CVertexBufferBinding::GetBufferCount(void) const
{
	return m_BindingMap.size();
}

USHORT CVertexBufferBinding::GetNextIndex(void) const
{
	return m_HighIndex;
}

USHORT CVertexBufferBinding::GetLastBoundIndex(void) const
{
	return m_BindingMap.empty() ? 0 : m_BindingMap.rbegin()->first + 1;
}

bool CVertexBufferBinding::HasGaps(void) const
{
	if (m_BindingMap.empty())
		return false;
	if (static_cast<size_t>(m_BindingMap.rbegin()->first + 1) == m_BindingMap.size())
		return false;
	return true;
}

void CVertexBufferBinding::CloseGaps(BindingIndexMap& bindingIndexMap)
{
	bindingIndexMap.clear();

	VertexBufferBindingMap newBindingMap;

	VertexBufferBindingMap::const_iterator it;
	USHORT tarGetIndex = 0;
	for (it = m_BindingMap.begin(); it != m_BindingMap.end(); ++it, ++tarGetIndex)
	{
		bindingIndexMap[it->first] = tarGetIndex;
		newBindingMap[tarGetIndex] = it->second;
	}

	m_BindingMap.swap(newBindingMap);
	m_HighIndex = tarGetIndex;
}

void CVertexBufferBinding::DestroyAllBuffer(void)
{
	VertexBufferBindingMap::iterator it = m_BindingMap.begin();
	VertexBufferBindingMap::iterator ite = m_BindingMap.end();
	for (;it!=ite;it++)
		CHardwareBufferMgr::GetInst()->DestroyBuffer(it->second.m_pBuffer);
	UnsetAllBindings();
	m_bChanged = true;
}
