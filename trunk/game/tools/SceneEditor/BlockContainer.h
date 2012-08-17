#pragma once
#include "stdafx.h"
#include "DataType.h"
#include "MyDefine.h"


//根据坐标分块存放数据的容器, (容器 及 迭代器 的使用方法与 STL容器相似)
//请尽量使用指针类型实例化容器模板, (避免不必要的数据复制拷贝,以提高容器的操作速度)
template <typename DataType>
class TBlockContainer
{
public:
	typedef pair <int, int>										BlockMap_Key;
	typedef pair <float, float>									DataMap_Key;
	typedef map<DataMap_Key, DataType>							Data_Map;
	typedef map<BlockMap_Key, Data_Map*>						Block_Map;
	typedef typename Data_Map::iterator							DataMap_Iter;
	typedef typename Block_Map::iterator						BlockMap_Iter;
	
	template <typename> class _iterator;
	typedef	_iterator<DataType>									iterator;
	friend iterator;



	template <typename DataType>//虽然概念上这里的 DataType 和 外面的 DataType 不是同一东西, 
								//但使用时候却是同一类型,所以取了个相同的名字,免得思维跳转麻烦
	class _iterator
	{
	public:
		typedef	typename TBlockContainer<DataType>				Owner_Type;
		typedef _iterator<DataType>								My_Type;
		typedef	typename Owner_Type::BlockMap_Iter					BlockMap_Iter;
		typedef typename Owner_Type::DataMap_Iter					DataMap_Iter;

		friend Owner_Type;

		_iterator(){}
		_iterator(const Owner_Type* pOwner, const BlockMap_Iter& blockIter, const DataMap_Iter& dataIter)
		{
			m_curBlockIter = blockIter;
			m_curDataIter = dataIter;
			m_pOwnerContainer = pOwner;
		}
		~_iterator(){}

		My_Type& operator =(const My_Type& another)
		{
			m_curBlockIter = another.m_curBlockIter;
			m_curDataIter = another.m_curDataIter;
			m_pOwnerContainer = another.m_pOwnerContainer;
			return *this;
		}

		DataType& operator *()const
		{
			return m_curDataIter->second;
		}

		DataType* operator ->()const
		{
			return &(m_curDataIter->second);
		}
		
		//重载前缀++
		My_Type& operator ++()
		{
			if (isBlockMapEnd())
			{
				GenErr("iterator 已处于 end 位置,无法使用 ++");
			}
			m_curDataIter++;
			if (isDataMapEnd())
			{
				m_curBlockIter++;
				if (!isBlockMapEnd())
				{
					m_curDataIter = m_curBlockIter->second->begin();
				}
			}
			return *this;
		}

		//重载后缀++
		My_Type operator ++(int)
		{
			My_Type old = *this;
			++(*this);
			return old;
		}

		bool operator ==(const My_Type& another) const
		{
			if ( m_curBlockIter == another.m_curBlockIter
				&& (isBlockMapEnd()	//MapIter 为end时候不需要比较DataIter(此时它无意义)
					|| m_curDataIter == another.m_curDataIter))
			{
				return true;
			}
			return false;
		}
		
		bool operator !=(const My_Type& another) const
		{
			return !(*this == another);
		}

		inline void GetBlockPos(OUT Pos& pos)
		{
			pos.x = m_curBlockIter->first.first;
			pos.y = m_curBlockIter->first.second;
		}

		inline void GetDataPos(OUT FPos& fPos)
		{
			fPos.x = m_curDataIter->first.first;
			fPos.y = m_curDataIter->first.second;
		}

	private:
		inline bool isBlockMapEnd() const
		{
			return m_curBlockIter == m_pOwnerContainer->m_mapBlock.end();
		}
		inline bool isDataMapEnd() const
		{
			return m_curDataIter == m_curBlockIter->second->end();
		}
	private:
		BlockMap_Iter											m_curBlockIter;
		DataMap_Iter											m_curDataIter;
		const Owner_Type *										m_pOwnerContainer;
	};




	TBlockContainer(int nBlockSize = 1):m_nBlockSize(nBlockSize),m_nSize(0){}
	~TBlockContainer(void) { clear(); }



	bool insert(float fX, float fY,const DataType& Data)
	{
		if (find(fX,fY) != end())
		{
			return false;
		}
		int iX, iY;
		GetBlockPos(iX, iY, fX, fY);
		DataMap_Key	dataKey(fX, fY);
		BlockMap_Key blockKey(iX,iY);
		BlockMap_Iter blockIter = m_mapBlock.find(blockKey);
		if (blockIter == m_mapBlock.end())
		{
			Data_Map*  pDataMap = new Data_Map();
			pDataMap->insert(make_pair(dataKey, Data));
			m_mapBlock.insert(make_pair(blockKey, pDataMap));
		}
		else
		{
			Data_Map*  pDataMap = blockIter->second;
			pDataMap->insert(make_pair(dataKey, Data));
		}
		m_nSize++;
		return true;
	}


	//仅当fX,fY 为存放时候的值时才能找到, 所以如果通过算术计算得到的值一般是找不到的. 请使用范围查找
	iterator find(float fX, float fY)
	{
		int iX, iY;
		GetBlockPos(iX, iY, fX, fY);
		BlockMap_Iter blockIter = m_mapBlock.find(BlockMap_Key(iX,iY));
		if (blockIter == m_mapBlock.end())
		{
			return end();
		}
		else
		{
			DataMap_Iter dataIter = blockIter->second->find(DataMap_Key(fX,fY));
			if (dataIter == blockIter->second->end())
			{
				return end();
			}
			else
			{
				return iterator(this, blockIter, dataIter);
			}
		}
	}
	
	//此操作会使迭代器失效
	bool erase(const iterator& iter)
	{
		if (iter.isBlockMapEnd())
		{
			GenErr("iterator 已处于 end 位置,无法删除");
		}
		iter.m_curBlockIter->second->erase(iter.m_curDataIter);
		if (iter.m_curBlockIter->second->size() == 0)
		{
			delete iter.m_curBlockIter->second;
			m_mapBlock.erase(iter.m_curBlockIter);
		}
		m_nSize --;
		return true;
	}

	void clear()
	{
		BlockMap_Iter blockIter = m_mapBlock.begin();
		while (blockIter != m_mapBlock.end())
		{
			delete blockIter->second;
			m_mapBlock.erase(blockIter);
			blockIter = m_mapBlock.begin();
		}
		m_nSize = 0;
	}

	iterator end()
	{
		return iterator(this, m_mapBlock.end(), DataMap_Iter());
	}
	iterator begin()
	{
		BlockMap_Iter blockIter = m_mapBlock.begin();
		if (blockIter == m_mapBlock.end())
		{
			return end();
		}
		else
		{
			return iterator(this, blockIter, blockIter->second->begin());
		}
	}
	
	//此操作会使迭代器失效
	bool Move(const iterator& iter, float fDestX, float fDestY)
	{

		DataMap_Key pair = iter.m_curDataIter->first;
		float oldX = pair.first, oldY = pair.second;
		if(fDestX == oldX && fDestX == oldY)//目标位置
		{
			return false;
		}
		if (find(fDestX,fDestY) != end())
		{
			return false;
		}
		DataType temp = *iter;
		if(erase(iter))
		{
			if(insert(fDestX, fDestY, temp))
			{
				return true;
			}
			else
			{
				Ast(insert(oldX, oldY, temp));//企图恢复
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	
	//isClearVec 是否清理vector
	//返回指定矩形区域内找到的元素个数
	int GetElement(OUT vector<DataType>& vec,const FRect& frect, bool isClearVec = true)
	{
		COUT(TBlockContainer::GetElement)
		FRect rect = frect.Correct();
		if (isClearVec)
		{
			vec.clear();
		}
		int iStartX, iStartY, iEndX, iEndY;
		GetBlockPos(iStartX, iStartY, rect.left, rect.top);
		GetBlockPos(iEndX, iEndY, rect.right, rect.bottom);
		int nCount = 0;

		BlockMap_Iter	blockIter;
		BlockMap_Key	blockKey;
		DataMap_Iter	dataIter;
		DataMap_Iter	dataEndIter;
		DataMap_Key		dataKey;
		FPos			dataPos;
		int x, y;

		//以下if-else 进行搜索方法选择
		if ((iEndX - iStartX) * (iEndY - iStartY) < (int)m_mapBlock.size())//搜索范围较小,通过块搜索
		{
			for (x = iStartX; x <= iEndX; x++)
			{
				for (y = iStartY; y <= iEndY;y++)
				{
					blockKey.first = x;
					blockKey.second = y;
					blockIter = m_mapBlock.find(blockKey);
					if (blockIter != m_mapBlock.end())
					{
						dataIter = blockIter->second->begin();
						dataEndIter = blockIter->second->end();
						while (dataIter != dataEndIter)
						{
							dataPos.x = dataIter->first.first;
							dataPos.y = dataIter->first.second;
							//if (dataPos.isInRect(rect))
							if(rect.IsContainPos(dataPos))
							{
								vec.push_back(dataIter->second);
								nCount++;
							}
							dataIter++;
						}
					}
				}
			}
		}
		else //搜索范围较大,通过Map搜索
		{
			blockIter = m_mapBlock.begin();
			while (blockIter != m_mapBlock.end())
			{	
				x = blockIter->first.first;
				y = blockIter->first.second;
				if (x >= iStartX && x <= iEndX && y >= iStartY && y <= iEndY)
				{
					dataIter = blockIter->second->begin();
					dataEndIter = blockIter->second->end();
					while (dataIter != dataEndIter)
					{
						dataPos.x = dataIter->first.first;
						dataPos.y = dataIter->first.second;
						//if (dataPos.isInRect(rect))
						if(rect.IsContainPos(dataPos))
						{
							vec.push_back(dataIter->second);
							nCount++;
						}
						dataIter++;
					}
				}
				blockIter++;
			}
		}
		COUT(GetElement   end)
		return nCount;
	}
	
	inline int size() const {return m_nSize;}
	inline bool empty() const {return m_nSize == 0;}
	inline void GetBlockPos(OUT int& iX, OUT int& iY, float fX, float fY) const
	{
		iX = (int) (fX/m_nBlockSize);
		iY = (int) (fY/m_nBlockSize);
	}
private:
	Block_Map	m_mapBlock;					//数据存放MAP
	uint		m_nBlockSize;				//分块大小
	int			m_nSize;					//容器内数据个数
};
