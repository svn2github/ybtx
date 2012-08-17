#ifndef _CFour_CC_H_
#define _CFour_CC_H_

namespace sqr
{
/// 四字符码.
/// Four Character Code
class FourCC
{
public:
	/// 默认构造函数
	FourCC();
	/// 从32位值构造 (如 FourCC('ABCD'))
	FourCC(DWORD id);
	/// 从字符串构造
	FourCC(const std::string& s);
	/// 等于
	bool operator==(const FourCC& rhs) const;
	/// 不等于
	bool operator!=(const FourCC& rhs) const;
	/// 小于
	bool operator<(const FourCC& rhs) const;
	/// 小于等于
	bool operator<=(const FourCC& rhs) const;
	/// 大于
	bool operator>(const FourCC& rhs) const;
	/// 大于等于
	bool operator>=(const FourCC& rhs) const;
	/// 是否合法
	bool IsValid() const;
	/// 以32位值赋值
	void SetFromDWORD(DWORD id);
	/// 返回32位值格式
	DWORD AsDWORD() const;
	/// 用字符串赋值
	void SetFromString(const std::string& s);
	/// 返回字符串格式
	std::string AsString() const;
	/// 转换一个FourCC到字符串
	static std::string ToString(const FourCC& f);
	/// 转换一个字符串到FourCC
	static FourCC FromString(const std::string& s);

private:
	DWORD fourCC;
};

//------------------------------------------------------------------------------
inline
FourCC::FourCC() :
		fourCC(0)
{
	// empty
}

//------------------------------------------------------------------------------
inline
FourCC::FourCC(DWORD id) :
		fourCC(id)
{
	// empty
}

//------------------------------------------------------------------------------
inline
FourCC::FourCC(const std::string& s)
{
	this->SetFromString(s);
}

//------------------------------------------------------------------------------
inline bool
FourCC::operator==(const FourCC& rhs) const
{
	return (this->fourCC == rhs.fourCC);
}

//------------------------------------------------------------------------------
inline bool
FourCC::operator!=(const FourCC& rhs) const
{
	return (this->fourCC != rhs.fourCC);
}

//------------------------------------------------------------------------------
inline bool
FourCC::operator>(const FourCC& rhs) const
{
	return (this->fourCC > rhs.fourCC);
}

//------------------------------------------------------------------------------
inline bool
FourCC::operator>=(const FourCC& rhs) const
{
	return (this->fourCC >= rhs.fourCC);
}

//------------------------------------------------------------------------------
inline bool
FourCC::operator<(const FourCC& rhs) const
{
	return (this->fourCC < rhs.fourCC);
}

//------------------------------------------------------------------------------
inline bool
FourCC::operator<=(const FourCC& rhs) const
{
	return (this->fourCC <= rhs.fourCC);
}

//------------------------------------------------------------------------------
inline bool
FourCC::IsValid() const
{
	return (0 != this->fourCC);
}

//------------------------------------------------------------------------------
inline void
FourCC::SetFromDWORD(DWORD id)
{
	//assert(0 != id);
	this->fourCC = id;
}

//------------------------------------------------------------------------------
inline DWORD
FourCC::AsDWORD() const
{
	return this->fourCC;
}

//------------------------------------------------------------------------------
inline void
FourCC::SetFromString(const std::string& s)
{
	*this = FromString(s);
}

//------------------------------------------------------------------------------
inline std::string
FourCC::AsString() const
{
	return ToString(*this);
}

//------------------------------------------------------------------------------
inline std::string
FourCC::ToString(const FourCC& f)
{
	//assert(0 != f.fourCC);
	std::string str("xzyw");
	str[0] = (char) ((f.fourCC & 0xFF000000) >> 24);
	str[1] = (char) ((f.fourCC & 0x00FF0000) >> 16);
	str[2] = (char) ((f.fourCC & 0x0000FF00) >> 8);
	str[3] = (char) (f.fourCC & 0x000000FF);
	return str;
}

//------------------------------------------------------------------------------
inline FourCC
FourCC::FromString(const std::string& s)
{
	//assert(s.length() == 4);
	return FourCC(uint(s[0] | s[1]<<8 | s[2]<<16 | s[3]<<24));
}
}

#endif //_CFour_CC_H_