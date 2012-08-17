#include "StdAfx.h"
#include "CDx9Adapter.h"
#include "TSqrAllocator.inl"

#define MinWidth	800
#define MinHeight	450

//------------------------------------------------------------------------------
namespace sqr
{

#pragma region Dx9Adapter
//------------------------------------------------------------------------------
Dx9Adapter::Dx9Adapter()
		: adapterNo(0)
{
	ZeroMemory(&this->d3dAdpaterIdentifier, sizeof(this->d3dAdpaterIdentifier));
	ZeroMemory(&this->desktopMode, sizeof(this->desktopMode));
}

//------------------------------------------------------------------------------
Dx9Adapter::Dx9Adapter( uint no, const D3DADAPTER_IDENTIFIER9& identifier, const D3DDISPLAYMODE& mode )
		: adapterNo(no)
		, d3dAdpaterIdentifier(identifier)
		, desktopMode(mode)
{
	// empty
}

//------------------------------------------------------------------------------
void Dx9Adapter::EnumerateDisplayModes( IDirect3D9* d3d )
{
	GVector<D3DFORMAT> allowedFormat;
	allowedFormat.push_back(D3DFMT_A1R5G5B5);
	allowedFormat.push_back(D3DFMT_A2R10G10B10);
	allowedFormat.push_back(D3DFMT_A8R8G8B8);
	allowedFormat.push_back(D3DFMT_R5G6B5);
	allowedFormat.push_back(D3DFMT_X1R5G5B5);
	allowedFormat.push_back(D3DFMT_X8R8G8B8);
	for (GVector<D3DFORMAT>::iterator it = allowedFormat.begin()
	        ; it != allowedFormat.end(); ++it)
	{
		D3DFORMAT format = (*it);
		D3DDISPLAYMODE d3dMode;
		uint numModes = d3d->GetAdapterModeCount(this->adapterNo, format);
		for (uint i = 0; i < numModes; i++)
		{
			d3d->EnumAdapterModes(this->adapterNo, format, i, &d3dMode);
			// abort the smaller resolution ratio
			if (d3dMode.Width < MinWidth || d3dMode.Height < MinHeight)
			{
				continue;
			}
			DisplayMode displayMode(d3dMode.Width, d3dMode.Height, d3dMode.Format);
			// add to list if it is a new one
			if (std::find(this->modelist.begin(), this->modelist.end(), displayMode) == this->modelist.end())
			{
				this->modelist.push_back(displayMode);
			}
		}
	}

}

//------------------------------------------------------------------------------
bool Dx9Adapter::GetBestDisplayMode( DisplayMode& mode ) const
{
	const uint INVALID = 0xffffffff;
	DisplayMode newMode(INVALID, INVALID, mode.GetPixelFormat());
	for (DisplayModeArray::const_iterator it = this->modelist.begin()
	        ; it != this->modelist.end(); ++it)
	{
		const DisplayMode& displayMode = (*it);
		if (mode.GetPixelFormat() != displayMode.GetPixelFormat())
		{
			continue;
		}
		// greater or equal, but the differences are the least
		if (displayMode.GetWidth() >= mode.GetWidth()
		        && displayMode.GetHeight() >= mode.GetHeight()
		        && displayMode.GetWidth() - mode.GetWidth() <= newMode.GetWidth() - mode.GetWidth()
		        && displayMode.GetHeight() - mode.GetHeight() <= newMode.GetHeight() - mode.GetHeight())
		{
			newMode.SetWidth(displayMode.GetWidth());
			newMode.SetHeight(displayMode.GetHeight());
		}
	}
	if (newMode.GetWidth() != INVALID && newMode.GetHeight() != INVALID)
	{
		mode = newMode;
		return true;
	}
	return false;
}
#pragma endregion Dx9Adapter


#pragma region AdapterList
//------------------------------------------------------------------------------
AdapterList::AdapterList()
		: curAdapter(static_cast<uint>(-1))
{
	// empty
}

//------------------------------------------------------------------------------
void AdapterList::EnumerateApdaters( IDirect3D9* d3d )
{
	for (uint i = 0; i < d3d->GetAdapterCount(); ++i)
	{
		D3DADAPTER_IDENTIFIER9 identifier;
		D3DDISPLAYMODE desktopMode;
		// get adapter infomation
		d3d->GetAdapterIdentifier(i, 0, &identifier);
		d3d->GetAdapterDisplayMode(i, &desktopMode);

		Dx9Adapter adapter(i, identifier, desktopMode);
		adapter.EnumerateDisplayModes(d3d);

		// add to list
		this->adapters.push_back(adapter);
	}
	if (this->adapters.empty())
	{
		GenErr("No available adapters found!");
	}
}
#pragma endregion AdapterList

}// namespace sqr
