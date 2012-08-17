#pragma once

namespace sqr
{
	class CPixelPath;
	template<class T> struct TPos;
	typedef TPos<float> CFPos;

	class CPathTransformer
	{
	public:
		CPathTransformer(const CPixelPath* pPath);
		CPathTransformer(const vector<CFPos>& vecPos);	//ÏñËØ×ø±ê
		~CPathTransformer();

		const int16* GetData()const;
		uint16 GetDataLenInByte()const;
		void PrintPath()const;
	private:
		const int16* Transform(const CPixelPath* pPath);
		const int16* Transform(const vector<CFPos>& vecPos);
		CPathTransformer(const CPathTransformer&);
		CPathTransformer& operator=(const CPathTransformer&);

		void GridPos32To16(const int32& x32, const int32& y32, int16& x16, int16& y16)const;

		const int16* m_pPathData;
		uint16 m_uDataLen;
	};
}

