#pragma once
#include <cassert>
#include "CPos.h"

namespace sqr
{

	template< class TMAP,class Key >
	inline const typename TMAP::mapped_type& GetMapState( const TMAP& DataMap, Key Pos )
	{
		typename TMAP::const_iterator next = DataMap.upper_bound( Pos );

		//*************************下面两行代码不应该存在，但是去掉会造成crash，有一个未知原因的bug存在******************************
		if ( next==DataMap.begin() )
			return DataMap.begin()->second;
		//*************************上面两行代码不应该存在，但是去掉会造成crash，有一个未知原因的bug存在******************************
		return ( --next )->second;
	}

	template< class TMAP,class Key >
	inline typename TMAP::mapped_type GetMapValue( const TMAP& DataMap, Key Pos )
	{
		typename TMAP::const_iterator next = DataMap.upper_bound( Pos );
		if ( next == DataMap.end() )
			return ( --next )->second;
		typename TMAP::const_iterator pre = next;
		--pre;

		if ( pre != DataMap.end() )
		{
			float weight = ( next->first - (float)Pos )/( next->first - pre->first );
			return (typename TMAP::mapped_type)( pre->second*weight + next->second*( 1.0f - weight ) );
		}
		else
			return next->second;
	}

	template< class WF >
	inline void SaveStringT( WF& iFile, const std::string& str )
	{
		int Size = str.length();
		iFile.write( (char*)&Size, sizeof(int) );
		iFile.write( (char*)str.c_str(), Size );
	}

	template< class WF >
	inline void SaveStringWDIT( WF& iFile, const std::string& str )
	{
		int Size = str.length();
		iFile.Write( (char*)&Size, sizeof(int) );
		iFile.Write( (char*)str.c_str(), Size );
	}

	template< class RF,class TString >
	inline void LoadStringT( RF& oFile, TString& str )
	{
		int Size = 0;

		oFile.read(&Size, sizeof(int));
		vector<char> temp(Size + 1, 0);
		oFile.read(&temp[0], Size);
		str = &temp[0];
	}

	template< class RF,class TString >
	inline void LoadStringSafeT( RF& oFile, TString& str )
	{
		int Size = 0;

		oFile.SafeRead(&Size, sizeof(int));
		vector<char> temp(Size + 1, 0);
		oFile.SafeRead(&temp[0], Size);
		str = &temp[0];
	}

	template<class T>
	struct DistPos : public TPos<T>
	{
		bool operator < ( const DistPos& arg ) const
		{
			return int32(x)*int32(x)+ int32(y)*int32(y) < int32(arg.x)*int32(arg.x) + int32(arg.y)*int32(arg.y);
		}
	};

	template< class _VecPos, class T = int >
	class TRangeSearch
	{
		_VecPos m_DisPos;

	public:
		TRangeSearch( T Layer/*查询步长*/ )
		{
			int it = 0;
			int nLen = Layer*2 + 1;
			m_DisPos.resize( nLen*nLen );
			for ( T i = -Layer; i <= Layer; i++ )
			{
				for ( T j = -Layer; j <= Layer; j++ )
				{
					m_DisPos[it].x = j;
					m_DisPos[it].y = i;
					it++;
				}
			}
			sort( m_DisPos.begin(), m_DisPos.end() );
		}

		const _VecPos& GetArr()const
		{
			return m_DisPos;
		};
	};

	/// 夹取
	template<typename T>
	inline T clamp(const T& val, const T& lowser, const T& upper)
	{
		if (val < lowser)		return lowser;
		else if (val > upper)	return upper;
		else					return val;
	}

	/// 插值
	template<typename T>
	inline T lerp(const T& x, const T& y, const T& l)
	{
		return x + (y - x) * l;
	}

	template<typename T>
	inline T sign(const T& value)
	{
		if (value > 0)
			return 1;
		else if (value < 0)
			return -1;
		else
			return 0;
	}
}