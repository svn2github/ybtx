#pragma once
#include <boost/shared_ptr.hpp>

#define DefineCfgSharedPtr(CfgType) \
class CfgType;\
class CfgType##SharedPtr : public boost::shared_ptr<CfgType> \
{\
	typedef boost::shared_ptr<CfgType> Parent_t;\
public:\
	typedef CfgType ContainedPointerType;\
	CfgType##SharedPtr(){}\
	CfgType##SharedPtr(CfgType* pCfg):Parent_t(pCfg){}\
};
