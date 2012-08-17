#pragma once


namespace sqr
{

	class CCoreObjectFollower;

	class CFastObjCmd
	{
	protected:
		string	m_sData;
	public:
		CFastObjCmd(const void* pData,uint32 uSize);
		virtual ~CFastObjCmd(void);
		virtual void Call(CCoreObjectFollower* pObj)=0;
	};


	class CCoreObjectFollower;
	template<typename CmdType>
	class TFastObjCmd
		:public CFastObjCmd
	{
	public:
		TFastObjCmd(const CmdType*);
		void Call(CCoreObjectFollower* pObj);
	};

}