#include "Stdafx.h"
#include "EditorUIHelper.h"

using namespace System::Reflection;

namespace sqr_tools
{
	void SetPropertyVisible( Object^ obj, String^ propName, bool visible )
	{
		PropertyDescriptorCollection^ props = TypeDescriptor::GetProperties(obj);
		AttributeCollection^ attrs = props[ propName ]->Attributes;
		Type^ t = BrowsableAttribute::typeid;
		FieldInfo^ fld = t->GetField("browsable", BindingFlags::Instance | BindingFlags::NonPublic);
		fld->SetValue(attrs[t], visible);
	}

	ref class PropertyOrderPair : public IComparable
	{
	public:
		PropertyOrderPair( String^ name, int order )
			: m_name(name)
			, m_order(order)
		{
		}

		virtual int CompareTo( Object^ other )
		{
			int otherOrder = ((PropertyOrderPair^)other)->m_order;
			if ( m_order == otherOrder )
			{
				String^ otherName = ((PropertyOrderPair^)other)->m_name;
				return String::Compare(m_name, otherName);
			}
			else if ( m_order > otherOrder )
			{
				return 1;
			}
			return -1;
		}

		property String^ Name 
		{ 
			String^ get()
			{
				return m_name;
			}
		}

	private:
		int		m_order;
		String^	m_name;
	};

	PropertyOrderAttribute::PropertyOrderAttribute( int order )
		: m_order(order)
	{
	}

	int PropertyOrderAttribute::Order::get()
	{
		return m_order;
	}
	
	bool PropertySorterConverter::GetPropertiesSupported( ITypeDescriptorContext^ context )
	{
		return true;
	}

	PropertyDescriptorCollection^ PropertySorterConverter::GetProperties( ITypeDescriptorContext^ context, Object^ value, array<Attribute^>^ attributes )
	{
		PropertyDescriptorCollection^ pdc = TypeDescriptor::GetProperties(value, attributes);
		ArrayList^ orderdProperties = gcnew ArrayList();
		for each ( PropertyDescriptor^ pd in pdc )
		{
			Attribute^ attr = pd->Attributes[PropertyOrderAttribute::typeid];
			if ( attr != nullptr )
			{
				PropertyOrderAttribute^ poa = (PropertyOrderAttribute^)attr;
				orderdProperties->Add(gcnew PropertyOrderPair(pd->Name, poa->Order));
			}
			else
			{
				orderdProperties->Add(gcnew PropertyOrderPair(pd->Name, 0));
			}

		}

		orderdProperties->Sort();

		ArrayList^ propertyNames = gcnew ArrayList();
		for each ( PropertyOrderPair^ pop in orderdProperties )
		{
			propertyNames->Add(pop->Name);
		}

		return pdc->Sort((array<String^>^)propertyNames->ToArray(String::typeid));
	}
}
