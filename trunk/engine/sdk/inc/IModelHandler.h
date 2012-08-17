#pragma once
#include "CDynamicObject.h"
#include "CallBackX.h"

namespace sqr
{
	class IModelHandler : public virtual CDynamicObject
	{
	public:
		virtual void OnSetVisable(bool isVisable) {CALL_CLASS_CALLBACK_1(isVisable)};
		virtual void OnResLoaded() { CALL_CLASS_CALLBACK() }
		virtual void OnSepKeyFrame( const char * szName ) {CALL_CLASS_CALLBACK_1(szName)}
		virtual void OnKeyFrame( const char * szName ) {CALL_CLASS_CALLBACK_1(szName)}
		virtual void OnSepAnimationEnd( const char * szName ) {CALL_CLASS_CALLBACK_1(szName)}
		virtual void OnAnimationEnd( const char * szName ) {CALL_CLASS_CALLBACK_1(szName)}
		virtual void OnLinkAnimationEnd(const char * szName) {CALL_CLASS_CALLBACK_1(szName)}
		virtual void BeSelected(){CALL_CLASS_CALLBACK()}
	};
};
