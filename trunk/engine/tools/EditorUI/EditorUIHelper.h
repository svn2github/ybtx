#pragma once

using namespace System;
using namespace System::Collections;
using namespace System::ComponentModel;

namespace sqr_tools
{
	void SetPropertyVisible( Object^ obj, String^ propName, bool visible );

	[AttributeUsage(AttributeTargets::Property)]
	public ref class PropertyOrderAttribute : public Attribute
	{
	public:
		PropertyOrderAttribute( int order );

		property int Order { int get(); }

	private:
		int m_order;
	};

	public ref class PropertySorterConverter : public ExpandableObjectConverter
	{
	public:
		virtual bool 
				GetPropertiesSupported( ITypeDescriptorContext^ context ) override;

		virtual PropertyDescriptorCollection^ 
				GetProperties( ITypeDescriptorContext^ context, Object^ value, array<Attribute^>^ attributes ) override;
	};
}