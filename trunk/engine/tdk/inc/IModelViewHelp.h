#pragma once

namespace sqr
{

class IModelViewHelp
{
public:
	IModelViewHelp(void);
	~IModelViewHelp(void);

	virtual void Update(){};
	virtual void Render(){};
};

}
