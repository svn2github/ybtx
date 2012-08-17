#ifndef _CGpuProgram_H_
#define _CGpuProgram_H_

#include "GraphicBase.h"

namespace sqr
{
	class CGpuProgramParameters;
	/// 着色器程序
	class CGpuProgram : public CGraphicMallocObject
	{
	public:
		CGpuProgram():m_isLoad(false) {};
		virtual ~CGpuProgram() {};

		virtual bool load(const GString str) = 0;
		virtual bool load(const char* buf,size_t fsize) = 0;
		virtual bool release(void) = 0;
		virtual bool bind(void)=0;
		virtual bool unbind(void) = 0;
	public:
		virtual CGpuProgramParameters* getParamPointer(void) = 0;
		virtual GpuProgramType getType(void) = 0;
		virtual bool _next(void) = 0; //这个接口是完全为了多Pass设计的
		inline	bool isLoaded();

	public:
		virtual void reset() = 0;
		virtual void lost() = 0;
		bool isNeedUpdateParameters();
		virtual void Update();
		
	protected:
		bool m_isNeedUpdateParameters;
		bool m_isLoad;
	};
		
	inline bool CGpuProgram::isLoaded()
	{
		return m_isLoad;
	}
}

#endif //_CGpuProgram_H_