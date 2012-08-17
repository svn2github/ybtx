namespace sqr
{

	class IdMaker
	{
	private:
		std::deque<uint32> m_queIdContainer;
		uint32 m_uMaxId;
	public:
		IdMaker(uint32 MaxId)
			:m_uMaxId(MaxId)
		{
			for (uint32 i = 0; i < MaxId; ++i)
			{
				m_queIdContainer.push_front(i);
			}
		}
		bool GetId(uint32 & id)
		{
			id = 0;
			if (m_queIdContainer.empty())
				return false;

			id = m_queIdContainer.back();
			m_queIdContainer.pop_back();
			return true;
		}
		uint32 GetCurrentMax()const
		{
			return m_uMaxId;
		}
		void Resize(uint32 uNewMax)
		{
			Ast(m_uMaxId < uNewMax);
			for (uint32 i = m_uMaxId; i < uNewMax; ++i)
				m_queIdContainer.push_front(i);
			m_uMaxId = uNewMax;
		}
		void ReleaseId(uint32 id)
		{
#ifndef NDEBUG
			Ast(!CheckIdInMap(id));
#endif
			m_queIdContainer.push_back(id);
		}
#ifndef NDEBUG
	private:
		std::map<uint32, bool> m_mapCheckIdValid;
		bool CheckIdInMap(uint32 id)
		{
			std::map<uint32, bool>::iterator it = m_mapCheckIdValid.find(id);
			return (it != m_mapCheckIdValid.end() && it->second);
		}
#endif
	};

}
