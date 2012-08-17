#pragma once
#include "Dx9Base.h"
#include "DisplayMode.h"
#include "TSingleton.h"

//------------------------------------------------------------------------------
namespace sqr
{

/// 适配器.
class Dx9Adapter : public CGraphicMallocObject
{
public:
	/// 默认构造函数
	Dx9Adapter();
	/// 构造函数
	Dx9Adapter(uint no, const D3DADAPTER_IDENTIFIER9& identifier, const D3DDISPLAYMODE& mode);
	/// 枚举可用的显示模式
	void EnumerateDisplayModes(IDirect3D9* d3d);
	/// 返回设备描述字符串
	string GetDescription() const;
	/// 返回适配器编号
	index_t GetAdapterNo() const;
	/// 返回桌面显示模式
	const D3DDISPLAYMODE& GetDesktopMode() const;
	/// 返回可用的显示模式个数
	size_t GetDisplayModeNum() const;
	/// 返回可用显示模式
	const DisplayMode& GetDisplayMode(index_t index);
	/// 返回显示模式列表
	const DisplayModeArray& GetDisplayModeArray() const;

	/// 按期望取得最接近的显示模式
	bool GetBestDisplayMode(DisplayMode& mode) const;

private:
	uint adapterNo;				///< 适配器编号
	D3DADAPTER_IDENTIFIER9 d3dAdpaterIdentifier;
	D3DDISPLAYMODE	desktopMode;	///< 桌面显示模式

	DisplayModeArray modelist;		///< 可用的显示模式
};

typedef GVector<Dx9Adapter> AdapterArray;

////////////////////////////////////////////////////////////////////////////////
/// 可用适配器列表.
class AdapterList : public Singleton<AdapterList>
{
public:
	/// 默认构造函数
	AdapterList();
	/// 枚举适配器
	void EnumerateApdaters(IDirect3D9* d3d);
	/// 返回适配器个数
	size_t GetAdapterNum() const;
	/// 返回返回指定适配器
	const Dx9Adapter& GetAdapter(index_t index);
	/// 返回当前适配器
	const Dx9Adapter& GetCurrentAdapter() const;
	/// 返回当前适配器的索引
	const index_t GetCurrentAdapterIndex() const;
	/// 设置当前适配器
	void SetCurrentAdapterIndex(index_t index);

private:
	AdapterArray adapters;	///< 可用适配器
	uint curAdapter;		///< 当前适配器
};


//------------------------------------------------------------------------------
inline string Dx9Adapter::GetDescription() const
{
	return string(this->d3dAdpaterIdentifier.Description);
}

//------------------------------------------------------------------------------
inline index_t Dx9Adapter::GetAdapterNo() const
{
	return this->adapterNo;
}

//------------------------------------------------------------------------------
inline const D3DDISPLAYMODE& Dx9Adapter::GetDesktopMode() const
{
	return this->desktopMode;
}

//------------------------------------------------------------------------------
inline size_t Dx9Adapter::GetDisplayModeNum() const
{
	return this->modelist.size();
}

//------------------------------------------------------------------------------
inline const DisplayMode& Dx9Adapter::GetDisplayMode( index_t index )
{
	Ast(index < this->GetDisplayModeNum());
	return this->modelist[index];
}

//------------------------------------------------------------------------------
inline const DisplayModeArray&
Dx9Adapter::GetDisplayModeArray() const
{
	return this->modelist;
}

//------------------------------------------------------------------------------
inline size_t AdapterList::GetAdapterNum() const
{
	return this->adapters.size();
}

//------------------------------------------------------------------------------
inline const Dx9Adapter& AdapterList::GetAdapter( index_t index )
{
	Ast(index < this->GetAdapterNum());
	return this->adapters[index];
}

//------------------------------------------------------------------------------
inline const index_t AdapterList::GetCurrentAdapterIndex() const
{
	return this->curAdapter;
}

//------------------------------------------------------------------------------
inline void AdapterList::SetCurrentAdapterIndex( index_t index )
{
	this->curAdapter = index;
}

//------------------------------------------------------------------------------
inline const Dx9Adapter& AdapterList::GetCurrentAdapter() const
{
	Ast(this->curAdapter < this->GetAdapterNum());
	return this->adapters[this->curAdapter];
}

}// namespace sqr