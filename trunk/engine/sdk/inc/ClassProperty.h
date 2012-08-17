/*
 *	Property declarations and types,效率比较低，需要优化，顺序的读取缺少灵活
    性，但效率会高不少
 */

#ifndef __PROPERTY_H__
#define __PROPERTY_H__

#ifdef _WIN32
#pragma warning (disable:4786) // No truncation warnings, please..
#endif

#include <ctype.h>
#include <stdio.h>
#include "BaseDefs.h"

#if defined(_WIN32) || defined(_USE_STLPORT)
#include <hash_map>
#else
#include <ext/hash_map>
#endif

#if defined(_WIN32) && !defined(_USE_STLPORT)
using namespace stdext;
#endif

#include <string>

#include "CPos.h"
#include "CDir.h"
#include "CColor3.h"

/*!
Different property types.
*/

#ifdef GetProp
#undef GetProp
#endif


enum
{
	PT_STRING			= 0,
	PT_POS				= 1,
	PT_COLOR			= 2,
	PT_REAL				= 3,
	PT_FLAGS			= 4,
	PT_BOOL				= 5,
	PT_LONGINT			= 6,
	PT_BYTE				= 7,
	NUM_PROPERTYTYPES	= 8
};

/*!
GenericProp -
Here's a list of which property types map to which variables:

PT_STRING

- m_Pos and m_Color if it's formatted like '1 2 3', m_String, m_Long, m_Float, m_Bool.
- If this is 'true' or 'false', it will set m_Long, m_Float, and m_Bool to 0 or 1.

PT_POS		- m_Pos, m_String ('1 2 ')
PT_COLOR    - m_Color, m_String ('1 2 3')
PT_REAL     - m_String, m_Long, m_Float, m_Bool
PT_FLAGS    - m_String, m_Long, m_Float, m_Bool
PT_BOOL     - m_String, m_Long, m_Float, m_Bool
PT_LONGINT	- m_String, m_Long, m_Float, m_Bool
PT_BYTE		- m_Byte.
*/

#define MAX_GP_STRING_LEN	256
#define MAX_PROP_NAME_LEN	256

struct GenericProp
{
	GenericProp() {}
	GenericProp(const GenericProp &cOther)
	{
		memcpy(this, &cOther, sizeof(*this));
	}
	GenericProp &operator=(const GenericProp &cOther)
	{
		if (&cOther != this)
			memcpy(this, &cOther, sizeof(*this));
		return *this;
	}

	// These constructors create a GenericProp with as many of the member variables
	// filled out as possible.  This is emulates the old behavior of GetPropGeneric

	GenericProp(const CPos& pos, uint32 nType) : m_Type((uint8)nType), m_Pos(pos),
			m_Long(m_Pos.x),m_Float((float)m_Pos.x),m_Bool(m_Pos.x != 0)
	{
		sprintf(m_String, "%f %f", (float)pos.x, (float)pos.y);
	}

	GenericProp(const CColor3& cValue, uint32 nType) :
			m_Color(cValue), m_Type((uint8)nType),
			m_Long((int32)cValue.r), m_Float(cValue.r), m_Bool(cValue.r != 0.0f)
	{
		sprintf(m_String, "%f %f %f", (float)cValue.r, (float)cValue.g, (float)cValue.b);
	}

	GenericProp(const char *pValue, uint32 nType) : m_Type((uint8)nType)
	{
		strcpy(m_String, pValue);
		m_String[sizeof(m_String) - 1] = 0;
		sscanf(pValue, "%f %f %f", &m_Color.r, &m_Color.g, &m_Color.b);
		sscanf(pValue, "%d %d", (int32)&m_Pos.x, (int32)&m_Pos.y);

		if ((toupper(*pValue) == 'T') || (toupper(*pValue) == 'Y'))
			m_Long = 1;
		else
			m_Long = atoi(pValue);
		m_Float = (float)atof(pValue);
		m_Bool = (m_Long != 0);
	}

	GenericProp(float fValue, uint32 nType) :
			m_Type((uint8)nType),
			m_Long((int32)fValue), m_Float(fValue), m_Bool(fValue != 0.0f)
	{
		sprintf(m_String, "%f", fValue);
	}

	GenericProp(int32 nValue, uint32 nType = PT_LONGINT) :
			m_Type((uint8)nType),
			m_Long(nValue), m_Float((float)nValue), m_Bool(nValue != 0)
	{
		sprintf(m_String, "%d", nValue);
	}


	GenericProp(bool bValue, uint32 nType) :
			m_Type((uint8)nType),
			m_Long(bValue ? 1 : 0), m_Float(bValue ? 1.0f : 0.0f), m_Bool(bValue)
	{
		m_String[0] = (bValue) ? '1' : '0';
		m_String[1] = 0;
	}

	// These constructors create a GenericProp only the specific member variable filled out.
	// They should be used when the extra member variables are not required.

	GenericProp(const CColor3& cValue) : m_Color(cValue), m_Type(PT_COLOR) {}

	GenericProp(const CPos& pos) : m_Type(PT_POS), m_Pos(pos) {}

	GenericProp(const char *pValue) : m_Type(PT_STRING)
	{
		strcpy(m_String, pValue);
	}

	GenericProp(float fValue) : m_Type(PT_REAL), m_Float(fValue) {}

	GenericProp(int32 nValue) : m_Type(PT_LONGINT), m_Long(nValue) {}

	GenericProp(uint8 nValue) : m_Type(PT_BYTE), m_Byte(nValue) {}

	GenericProp(bool bValue) : m_Type(PT_BOOL), m_Bool(bValue) {}

public:
	// Storage for PT_COLOR
	CColor3		m_Color;
	// Type of the property
	uint8		m_Type;
	// Storage for PT_VECTOR
	CPos		m_Pos;
	// Storage for PT_LONGINT & PT_FLAGS
	int32		m_Long;
	// Storage for PT_REAL
	float		m_Float;
	// Storage for PT_BOOL
	bool		m_Bool;
	// Storage for PT_BYTE
	uint8		m_Byte;
	// Storage for PT_STRING
	char		m_String[MAX_GP_STRING_LEN+1];
};

class GenericPropList
{
public:
	GenericPropList() :
			m_nNumProps(0), m_nMaxProps(0), m_pProps(NULL), m_pPropIndices(NULL)
	{
	}

	GenericPropList(const GenericPropList &cOther) :
			m_nNumProps(0), m_nMaxProps(0), m_pProps(NULL), m_pPropIndices(NULL)
	{
		if (cOther.GetMaxProps())
		{
			if (ReserveProps(cOther.GetNumProps(), false))
			{
				//copy over the properties and init our indices
				for (uint32 nCurrProp = 0; nCurrProp < GetMaxProps(); nCurrProp++)
				{
					strcpy(m_pProps[nCurrProp].m_pszName, cOther.m_pPropIndices[nCurrProp]->m_pszName);
					m_pProps[nCurrProp].m_Prop = cOther.m_pPropIndices[nCurrProp]->m_Prop;

					m_pPropIndices[nCurrProp] = &m_pProps[nCurrProp];
				}
				m_nNumProps = cOther.GetNumProps();
			}
		}
	}

	~GenericPropList()
	{
		FreeProps();
	}

	GenericPropList &operator=(const GenericPropList &cOther)
	{
		if (this == &cOther)
			return *this;

		if (cOther.GetMaxProps())
		{
			if (ReserveProps(cOther.GetNumProps(), false))
			{
				//copy over the properties and init our indices
				for (uint32 nCurrProp = 0; nCurrProp < cOther.GetNumProps(); nCurrProp++)
				{
					strcpy(m_pProps[nCurrProp].m_pszName, cOther.m_pPropIndices[nCurrProp]->m_pszName);
					m_pProps[nCurrProp].m_Prop = cOther.m_pPropIndices[nCurrProp]->m_Prop;

					m_pPropIndices[nCurrProp] = &m_pProps[nCurrProp];
				}
				m_nNumProps = cOther.GetNumProps();
			}
		}
		else
		{
			Reset();
		}

		return *this;
	}
	/*!
	Get the PT_VECTOR property associated with pName.
	Returns vDefault if that property was not found.
	*/
	CPos GetPropPos(const char *pName, const CPos& vDefault) const
	{
		const GenericProp *pResult = GetProp(pName);
		if (!pResult || (pResult->m_Type != PT_POS))
			return vDefault;
		return pResult->m_Pos;
	}
	/*!
	Get the PT_COLOR property associated with pName.
	Returns vDefault if that property was not found.
	*/
	CColor3 GetPropColor(const char *pName, const CColor3 &vDefault) const
	{
		const GenericProp *pResult = GetProp(pName);
		if (!pResult || (pResult->m_Type != PT_COLOR))
			return vDefault;
		return pResult->m_Color;
	}
	/*!
	Get the PT_STRING property associated with pName.
	Returns pDefault if that property was not found.
	*/
	const char *GetPropString(const char *pName, const char *pDefault) const
	{
		const GenericProp *pResult = GetProp(pName);
		if (!pResult || (pResult->m_Type != PT_STRING))
			return pDefault;
		return pResult->m_String;
	}
	/*!
	Get the PT_ROTATION property associated with pName.
	Returns rDefault if that property was not found.
	*/
	uint8 GetPropByte(const char *pName, uint8 nDefault) const
	{
		const GenericProp *pResult = GetProp(pName);
		if (!pResult || (pResult->m_Type != PT_BYTE))
			return nDefault;
		return pResult->m_Byte;
	}
	/*!
	Get the PT_LONGINT property associated with pName.
	Returns nDefault if that property was not found.
	*/
	int32 GetPropLongInt(const char *pName, int32 nDefault) const
	{
		const GenericProp *pResult = GetProp(pName);
		if (!pResult || (pResult->m_Type != PT_LONGINT))
			return nDefault;
		return pResult->m_Long;
	}
	/*!
	Get the PT_FLAGS property associated with pName.
	Returns nDefault if that property was not found.
	*/
	int32 GetPropFlags(const char *pName, int32 nDefault) const
	{
		const GenericProp *pResult = GetProp(pName);
		if (!pResult || (pResult->m_Type != PT_FLAGS))
			return nDefault;
		return pResult->m_Long;
	}
	/*!
	Get the PT_REAL property associated with pName.
	Returns fDefault if that property was not found.
	*/
	float GetPropReal(const char *pName, float fDefault) const
	{
		const GenericProp *pResult = GetProp(pName);
		if (!pResult || (pResult->m_Type != PT_REAL))
			return fDefault;
		return pResult->m_Float;
	}
	/*!
	Get the PT_BOOL property associated with pName.
	Returns bDefault if that property was not found.
	*/
	bool GetPropBool(const char *pName, bool bDefault) const
	{
		const GenericProp *pResult = GetProp(pName);
		if (!pResult || (pResult->m_Type != PT_BOOL))
			return bDefault;
		return pResult->m_Bool;
	}

public:

	//This will reset the properties in the list. No memory is freed by this
	void	Reset()
	{
		m_nNumProps = 0;
	}

	//Determines the number of properties that this list can hold
	uint32	GetNumProps() const
	{
		return m_nNumProps;
	}

	//Determines the number of properties that this list can hold
	uint32	GetMaxProps() const
	{
		return m_nMaxProps;
	}

	//Sets the maximum number of properties that this list can hold
	bool	ReserveProps(uint32 nMax, bool bPreserveProps)
	{
		//see if we need to reset the number of properties
		if (!bPreserveProps)
			m_nNumProps = 0;

		//see if we can avoid the allocation
		if (nMax == m_nMaxProps)
		{
			return true;
		}

		SPropEntry*		pNewProps;
		SPropEntry**	pNewPropIndices;

		pNewProps = new SPropEntry[nMax];
		pNewPropIndices = new SPropEntry*[nMax];

		//check the allocation
		if (!pNewProps || !pNewPropIndices)
		{
			delete [] pNewProps;
			delete [] pNewPropIndices;
			return false;
		}

		//now copy over the properties (note that m_nNumProps will always be 0 if we are not
		//preserving)
		uint32 nNumToCopy = _MIN(m_nNumProps, nMax);

		for (uint32 nCurrProp = 0; nCurrProp < nNumToCopy; nCurrProp++)
		{
			pNewPropIndices[nCurrProp] = &pNewProps[nCurrProp];

			strcpy(pNewProps[nCurrProp].m_pszName, m_pPropIndices[nCurrProp]->m_pszName);
			pNewProps[nCurrProp].m_Prop = m_pPropIndices[nCurrProp]->m_Prop;
		}

		//clear out the old
		FreeProps();

		//this is important that this comes after the free props since that clears out the
		//number of properties.
		m_nNumProps = nNumToCopy;

		//setup the new
		m_pProps = pNewProps;
		m_pPropIndices = pNewPropIndices;

		m_nMaxProps = nMax;

		return true;
	}

	//adds a property into the list, this does an insertion sort
	bool	AddProp(const char* pszName, const GenericProp& Prop)
	{
		//see where this property should go using a binary search
		int32 nLower = 0;
		int32 nUpper = (int32)GetNumProps() - 1;

		int32 nMiddle;
		int32 nDiff;

		while (nLower <= nUpper)
		{
			nMiddle = (nLower + nUpper) / 2;
			nDiff = _stricmp(pszName, m_pPropIndices[nMiddle]->m_pszName);
			if (nDiff == 0)
			{
				//the item already exists
				m_pPropIndices[nMiddle]->m_Prop = Prop;
				return true;
			}

			if (nDiff < 0)
				nUpper = nMiddle - 1;
			else
				nLower = nMiddle + 1;
		}

		//insert it at the larger position
		uint32 nPropLocation = (uint32)_MAX(nUpper, nLower);

		//make sure we have room
		if (GetNumProps() >= GetMaxProps())
		{
			// No type.. flying commented.
			static const int knExtraReserveBuffer = 3;

			if (!ReserveProps(GetNumProps() + 1 + knExtraReserveBuffer, true))
				return false;
		}

		//alright, we now need to move everything down to create room
		for (uint32 nCurrProp = GetNumProps(); nCurrProp > nPropLocation; nCurrProp--)
			m_pPropIndices[nCurrProp] = m_pPropIndices[nCurrProp - 1];

		//now add our item to the list and hook up our index
		m_pPropIndices[nPropLocation] = &m_pProps[GetNumProps()];

		strcpy(m_pProps[GetNumProps()].m_pszName, pszName);
		m_pProps[GetNumProps()].m_Prop = Prop;

		//increment our property count
		m_nNumProps++;

		//success
		return true;
	}

	//gets the name of a property
	const char* GetPropName(uint32 nProp) const
	{
		if (nProp > GetNumProps())
			return "";

		return m_pProps[nProp].m_pszName;
	}

	//gets an actual property pointer
	const GenericProp* GetProp(uint32 nProp) const
	{
		if (nProp > GetNumProps())
			return NULL;

		return &m_pProps[nProp].m_Prop;
	}


	/*!
	Get the property associated with pName.
	Returns LTNULL if that property was not found.
	*/
	const GenericProp *GetProp(const char *pszName) const
	{
		//do a binary search
		int32 nLower = 0;
		int32 nUpper = (int32)GetNumProps() - 1;
		int32 nIndex;

		while (nLower <= nUpper)
		{
			nIndex = (nLower + nUpper) / 2;
			int32 nResult = _stricmp(pszName, m_pPropIndices[nIndex]->m_pszName);
			if (nResult < 0)
			{
				nUpper = nIndex - 1;
			}
			else if (nResult > 0)
			{
				nLower = nIndex + 1;
			}
			else
			{
				//we have a matching property
				return &m_pPropIndices[nIndex]->m_Prop;
			}
		}

		return NULL;
	}

protected:

	//frees the memory associated with this property list
	void FreeProps()
	{
		delete [] m_pProps;
		m_pProps = NULL;

		delete [] m_pPropIndices;
		m_pPropIndices = NULL;

		m_nNumProps = 0;
		m_nMaxProps = 0;
	}

	//a structure that represents a property and its name
	struct SPropEntry
	{
		char			m_pszName[MAX_PROP_NAME_LEN];
		GenericProp		m_Prop;
	};

	//the number of properties currently in the list
	uint32		m_nNumProps;

	//the maximum number of properties that we can have in the list
	uint32		m_nMaxProps;

	//the list of the actual properties and their names
	SPropEntry	*m_pProps;

	//pointers to elements in the property list so that insertion doesn't have to move so
	//much memory around,排序的索引
	SPropEntry	**m_pPropIndices;
};

#endif //__PROPERTY_H__

