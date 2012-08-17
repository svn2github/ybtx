#pragma	once
#include "BaseMath.h"

namespace sqr
{
	template <class Type>
	class CBaseHermiteCurve
	{
	public:
		CBaseHermiteCurve();
		~CBaseHermiteCurve();

		void  Clear();
		bool  Add( Type key, const float keyTimeDelta );
		void  Insert( Type key, const float keyTime );
		Type  Get( float time );

		void  Save( FILE * fp );
		void  Load( FILE * fp, int nCnt );
		void  BuildCardinal( const float bias = 0.5f );


		int   GetNumberOfKeys();
		Type  GetKeyValue( const int keyNumber );
		float GetKeyTime( const int keyNumber );
		void  SetKeyValue( const int keyNumber, Type data );
		void  SetKeyTime( const int keyNumber, const float time );
	protected:

	private:
		inline void BuildBasis( const float h );
		int GetFirst( const float time );
		int GetFirstLoop( const float time );
		float basis[ 4 ];
		struct keyStruct
		{
			Type data;
			Type tangentIn;
			Type tangentOut;
			float xTime;
		};
		vector<keyStruct> keyFrame;
		int keyFramesCount;
		float loopStart, loopTime;
		bool looping;
	};


	template <class Type>
	CBaseHermiteCurve<Type>::CBaseHermiteCurve()
	{
		Clear();
	}

	template <class Type>
	CBaseHermiteCurve<Type>::~CBaseHermiteCurve()
	{
		Clear();
	}

	template <class Type>
	void CBaseHermiteCurve<Type>::Clear()
	{
		keyFrame.clear();
		keyFramesCount = 0;
		looping = false;
	}

	template <class Type>
	bool CBaseHermiteCurve<Type>::Add( Type key, const float keyTimeDelta )
	{
		float keyTime = 0;

		if ( !keyFrame.empty() )
			keyTime = keyFrame.back().xTime + keyTimeDelta;

		if ( keyFramesCount > 0 )
		{
			if ( keyFrame[ keyFramesCount-1 ].xTime >= keyTime )
				return false;
		}

		keyStruct strTmp;
		strTmp.data = key;
		strTmp.xTime = keyTime;

		keyFrame.push_back( strTmp );
		++keyFramesCount;

		return true;
	}

	template <class Type>
	void CBaseHermiteCurve<Type>::Insert( Type key, const float keyTime )
	{
		keyStruct strTmp;
		strTmp.data = key;
		strTmp.xTime = keyTime;

		keyFrame.push_back( strTmp );
		++keyFramesCount;
	}

	template <class Type>
	inline void CBaseHermiteCurve<Type>::BuildBasis( const float h )
	{
		const float h2 = h*h;
		const float h3 = h2*h;
		basis[ 0 ] =  2*h3 - 3*h2 + 1.0f;
		basis[ 1 ] = -2*h3 + 3*h2;
		basis[ 2 ] =  h3   - 2*h2 + h;
		basis[ 3 ] =  h3   - h2;
	}

	template <class Type>
	int CBaseHermiteCurve<Type>::GetFirst( const float time )
	{
		if ( keyFrame[0].xTime >= time ) return -1;
		if ( keyFrame[keyFramesCount-1].xTime <= time ) 
			return keyFramesCount;

		int sIndex = 0;
		int eIndex = keyFramesCount-1;
		while ( (eIndex - sIndex) > 1 )
		{
			const int mIndex = (sIndex + eIndex) / 2;
			if ( keyFrame[ mIndex ].xTime <= time )
				sIndex = mIndex;
			else
				eIndex = mIndex;
		}
		return sIndex;
	}

	template <class Type>
	int CBaseHermiteCurve<Type>::GetFirstLoop( const float time )
	{
		if ( keyFrame[keyFramesCount-1].xTime <= time ) return ( keyFramesCount-1 );

		int sIndex = 0;
		int eIndex = keyFramesCount-1;
		while ( (eIndex - sIndex) > 1 )
		{
			const int mIndex = (sIndex + eIndex) / 2;
			if ( keyFrame[ mIndex ].xTime <= time )
			{
				sIndex = mIndex;
			}
			else
			{
				eIndex = mIndex;
			}
		}
		return sIndex;
	}

#pragma warning( disable : 4700 )
	template <class Type>
	Type CBaseHermiteCurve<Type>::Get( float time )
	{
		if ( keyFramesCount < 2 )
		{
			if ( keyFramesCount == 1 )
			{
				return keyFrame[0].data;
			}
			else
			{
				Type noVal;
				return noVal;
			}
		};

		int first, next;

		first = GetFirst( time );
		next = first + 1;

		if ( first < 0 ) 
			return keyFrame[ 0 ].data;

		if ( first == keyFramesCount ) 
			return keyFrame[ keyFramesCount-1 ].data;

		float keyNextTime   = keyFrame[ next ].xTime;
		float keyFirstTime  = keyFrame[ first ].xTime;
		float timeDelta		= keyNextTime - keyFirstTime;
		float inSegmentTime = keyNextTime - time;

		BuildBasis( 1.0f - (inSegmentTime / timeDelta) );

		Type returnValue;
		returnValue = keyFrame[ first ].data * basis[ 0 ];
		returnValue += keyFrame[ next ].data * basis[ 1 ];
		returnValue += keyFrame[ first ].tangentOut * basis[ 2 ];
		returnValue += keyFrame[ next ].tangentIn * basis[ 3 ];

		return returnValue;
	}

#pragma warning( default : 4311 )

	template <class Type>
	void CBaseHermiteCurve<Type>::BuildCardinal( const float bias )
	{
		if ( keyFramesCount < 2 ) return;
		for ( int i=0; i<keyFramesCount; ++i )
		{
			Type ray;
			if ( i<keyFramesCount-1 )
			{
				ray = keyFrame[ i+1 ].data;
			}
			else
			{
				if ( looping )
				{
					ray = keyFrame[ 0 ].data;
				}
				else
				{
					ray = keyFrame[ i ].data;
				}
			}
			if ( i > 0 )
			{
				ray -= keyFrame[i-1].data;
			}
			else
			{
				if ( looping )
				{
					ray -= keyFrame[ keyFramesCount-1 ].data;
				}
				else
				{
					ray -= keyFrame[ i ].data;
				}
			}
			ray *= bias;
			keyFrame[i].tangentIn = ray;
			keyFrame[i].tangentOut = ray;
		}
	}

	template <class Type>
	void CBaseHermiteCurve<Type>::Save( FILE * fp )
	{
		for ( size_t i = 0; i < keyFrame.size(); ++i )
		{
			fwrite(&keyFrame[i].data,sizeof(Type),1,fp);

			if ( i != 0 )
			{
				float fTimeDelta = keyFrame[i].xTime - keyFrame[i-1].xTime;
				fwrite(&fTimeDelta,sizeof(float),1,fp);
			}
			else
			{
				float fTimeDelta = keyFrame[i].xTime;
				fwrite(&fTimeDelta,sizeof(float),1,fp);
			}
		}
	}

	template <class Type>
	void CBaseHermiteCurve<Type>::Load(FILE * fp,int nCnt)
	{
		for ( int i = 0; i < nCnt; ++i )
		{
			Type data;
			float time;
			fread(&data,sizeof(Type),1,fp);
			fread(&time,sizeof(float),1,fp);

			this->Add(data,time);
		}
	}

	template <class Type>
	int CBaseHermiteCurve<Type>::GetNumberOfKeys()
	{
		return keyFramesCount;
	}

	template <class Type>
	Type CBaseHermiteCurve<Type>::GetKeyValue( const int keyNumber )
	{
		return keyFrame[ Clamp<int>( keyNumber, 0, keyFramesCount-1 ) ].data;
	}

	template <class Type>
	float CBaseHermiteCurve<Type>::GetKeyTime( const int keyNumber )
	{
		return keyFrame[ Clamp<int>( keyNumber, 0, keyFramesCount-1 ) ].xTime;
	}

	template <class Type>
	void CBaseHermiteCurve<Type>::SetKeyValue( const int keyNumber, Type data )
	{
		this->keyFrame[ Clamp<int>( keyNumber, 0, keyFramesCount-1 ) ].data = data;
	}

	template <class Type>
	void CBaseHermiteCurve<Type>::SetKeyTime( const int keyNumber, const float time )
	{
		this->keyFrame[ Clamp<int>( keyNumber, 0, keyFramesCount-1 ) ].xTime = time;
	}
}