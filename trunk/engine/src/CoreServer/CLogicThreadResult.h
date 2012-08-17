#pragma once

namespace sqr
{
	class CLogicThreadResult
	{
	public:	
		static void* operator new(size_t stSize);
		static void operator delete(void* pMem){}

		void* AllocMem(size_t stSize);
		char* CloneString(const char* szStr);
		void* CloneData(const void* pData, size_t stSize);

		void Add();
		virtual ~CLogicThreadResult(){}
	};

	class CAoiRelationChangedResultToLogic : public CLogicThreadResult
	{
	protected:
		CAoiRelationChangedResultToLogic(uint32 uMyCoreObjID, uint32 uOtCoreObjID, bool bSee, uint32 uDimension)
			: m_uMyCoreObjID(uMyCoreObjID), m_uOtCoreObjID(uOtCoreObjID), m_bSee(bSee), m_uDimension(uDimension) {}
	protected:
		uint32	m_uMyCoreObjID;
		uint32	m_uOtCoreObjID;
		bool	m_bSee;
		uint32	m_uDimension;
	};

	class CAoiViewChangedResultToLogic 
		: public CAoiRelationChangedResultToLogic
	{
	public:
		CAoiViewChangedResultToLogic(uint32 uMyCoreObjID, uint32 uOtCoreObjID, bool bSee, uint32 uDimension);
		~CAoiViewChangedResultToLogic();
	};

	class CAoiObjHaltedResultToLogic
		:public CLogicThreadResult
	{
	public:
		CAoiObjHaltedResultToLogic(uint32 uCoreObjID);
		~CAoiObjHaltedResultToLogic();
	private:
		uint32	m_uCoreObjID;
	};
}
