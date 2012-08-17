#pragma once

namespace EffectEditor
{
	// 完成特编自身一些全局单例的初始化
	class EditorMainFormImp
	{
	public:
		EditorMainFormImp();
		~EditorMainFormImp();

		void ChangeToEditEfxManager();
	};
}