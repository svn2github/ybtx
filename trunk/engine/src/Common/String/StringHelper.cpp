#include "stdafx.h"
#include "StringHelper.h"
#include "jenkins.h"
#include <stdarg.h>
#include <cstdio>
#include <algorithm>
#include "CodeCvs.h"
#include "ExpHelper.h"
#include "ThreadHelper.h"
#include "CodeCvs.h"

// 干掉win下对max和min宏的定义
#undef max
#undef min

namespace sqr
{
	const std::string WHITE_SPACE = " \t\r\n\f\v";	///< 空白字符 依次为: 空格, 水平制表符, 回车, 换行, 走纸换行, 垂直制表符
	const int32 DEFAULT_BUFFER_SIZE = 1024;			///< vformat时默认的缓冲区大小

	char* get_last_slash( const std::string& path )
	{
		const char* s = path.c_str();
		const char* lastSlash = strrchr(s, '/');
		if (0 == lastSlash) lastSlash = strrchr(s, '\\');
		if (0 == lastSlash) lastSlash = strrchr(s, ':');
		return const_cast<char*>(lastSlash);
	}

	wchar_t* get_last_slash( const std::wstring& path )
	{
		const wchar_t* s = path.c_str();
		const wchar_t* lastSlash = wcsrchr(s, '/');
		if (0 == lastSlash) lastSlash = wcsrchr(s, '\\');
		if (0 == lastSlash) lastSlash = wcsrchr(s, ':');
		return const_cast<wchar_t*>(lastSlash);
	}

	std::string extract_parent_dir( const std::string& path )
	{
		string pathString(path);
		char* lastSlash = get_last_slash(pathString);

		// special case if path ends with a slash
		if (lastSlash)
		{
			if (0 == lastSlash[1])
			{
				*lastSlash = 0;
				lastSlash = get_last_slash(pathString);
			}
			if (lastSlash)
			{
				*lastSlash = 0;
			}
		}
		return pathString.c_str();
	}

	std::wstring extract_parent_dir( const std::wstring& path )
	{
		const wstring pathString(path);
		wchar_t* lastSlash = get_last_slash(pathString);

		// special case if path ends with a slash
		if (lastSlash)
		{
			if (0 == lastSlash[1])
			{
				*lastSlash = 0;
				lastSlash = get_last_slash(pathString);
			}
			if (lastSlash)
			{
				*lastSlash = 0;
			}
		}
		return pathString.c_str();
	}

	std::wstring format(std::wstring &result, const wchar_t *fmt, ...)
	{
		va_list vl;
		va_start(vl, fmt);
		vformat(result, fmt, vl);
		va_end(vl);
		return result;
	}

	std::string& format(std::string& resultOUT, const char* fmt, ...)
	{
		va_list vl;
		va_start(vl, fmt);
		vformat(resultOUT, fmt, vl);
		va_end(vl);
		return resultOUT;
	}
	std::string format(const char* fmt, ...)
	{
		va_list vl;
		va_start(vl, fmt);
		std::string resultOUT;
		vformat(resultOUT, fmt, vl);
		va_end(vl);
		return resultOUT;
	}

	std::wstring& vformat(std::wstring& resultOUT, const wchar_t *fmt, va_list argptr, int32 default_size /*= DEFAULT_BUFFER_SIZE*/)
	{
		std::vector<wchar_t> buffer(default_size, 0);
		for (;;)
		{
#ifndef _WIN32
			int32 len = vswprintf(&(buffer[0]), buffer.size(), fmt, argptr);
#else
			int32 len = vswprintf_s(&(buffer[0]), buffer.size(), fmt, argptr);
#endif
			if (len == -1)
			{
				buffer.resize(buffer.size() * 2, 0);
				continue;
			}
			if (len == static_cast<int32>(buffer.size()))
				buffer.push_back(0);
			break;
		}
		resultOUT.assign(&(buffer[0]));
		return resultOUT;
	}

	std::string& vformat(std::string& resultOUT, const char *fmt, va_list argptr, int32 default_size /*= DEFAULT_BUFFER_SIZE*/)
	{
		std::vector<char> buffer(default_size, 0);
		for (;;)
		{
#ifndef _WIN32
			int32 len = vsnprintf(&(buffer[0]), buffer.size(), fmt, argptr);
#else
			// MSDN: formats and writes up to count characters of the given data to the memory
			// pointed to by buffer and appends a terminating null.
			int32 len = vsnprintf_s(&(buffer[0]), buffer.size(), buffer.size() - 1, fmt, argptr);
#endif
			if (len == -1)
			{
				buffer.resize(buffer.size() * 2, 0);
				continue;
			}
			if (len == static_cast<int32>(buffer.size()))
				buffer.push_back(0);
			break;
		}
		resultOUT.assign(&(buffer[0]));
		return resultOUT;
	}
	bool format_index(std::string& resultOUT, const char* fmt, const std::vector<std::string>& argList)
	{
		// 解析状态机的状态
		enum FormatState
		{
			FS_NORMAL,
			FS_R_BRACKE,
			FS_L_BRACKE,
			FS_NUMBER,
		};
#ifndef unix
#pragma warning(push)
#pragma warning(disable: 4127) // C4127: 条件表达式是常数
#endif

		// 出错处理代码
#ifdef _DEBUG
#define FS_ERROR_MESSAGE(msg) \
	do{\
	resultOUT = (msg);\
	return false;\
	} while(0)
#else
#define FS_ERROR_MESSAGE(msg) \
	do{\
	resultOUT.clear();\
	return false;\
	} while(0)
#endif

		// 预处理
		resultOUT.clear();
		if (fmt == NULL || fmt[0] == 0)
			return true;
		// 正式解析
		const char* p = fmt;
		FormatState state = FS_NORMAL;
		std::string index;
		for (;;)
		{
			const char c = *p++;
			if (c == 0)
			{
				if (state == FS_NORMAL)
					break; // OK 算法执行完毕
				else
					FS_ERROR_MESSAGE("ERROR！格式化字符末尾可能有残缺，不能正确解析");
			}
			switch (state)
			{
			case FS_NORMAL:
				switch (c)
				{
				case '}':
					state = FS_R_BRACKE;
					break;
				case '{':
					state = FS_L_BRACKE;
					break;
				default:
					resultOUT += c;
					break;
				}
				break;
			case FS_R_BRACKE:
				if (c == '}')
				{
					state = FS_NORMAL;
					resultOUT += '}';
				}
				else
				{
					FS_ERROR_MESSAGE("ERROR！在格式化字符串中发现不成对的\'}\'字符");
				}
				break;
			case FS_L_BRACKE:
				if (c == '{')
				{
					resultOUT += '}';
					state = FS_NORMAL;
				}
				else if (is_numeric(c))
				{
					index = c;
					state = FS_NUMBER;
				}
				else
				{
					FS_ERROR_MESSAGE("ERROR！在格式化字符串中发现\'{\'后的非数字字符");
				}
				break;
			case FS_NUMBER:
				if (c == '}')
				{
					int32 idx = 0;
					if (integer(index, idx))
					{
						if (idx >= 0 && static_cast<size_t>(idx) < argList.size())
						{
							resultOUT += argList[idx];
							state = FS_NORMAL;
						}
						else
						{
							FS_ERROR_MESSAGE("ERROR！在格式化字符串中的参数下标超出参数列表范围");
						}
					}
					else
					{
						FS_ERROR_MESSAGE("ERROR！在格式化字符串中的参数下标不能正确转换为数字");
					}
				}
				else if (is_numeric(c))
				{
					index += c;
				}
				else
				{
					FS_ERROR_MESSAGE("ERROR！在格式化字符串中发现\'{\'后的非数字字符");
				}
				break;
			default:
				Ast(0);//所有状态签名已经包含，程序不应该执行到此处
				break;
			}
		}
		return true;

#undef FS_ERROR_MESSAGE

#ifndef unix
#pragma warning(pop)
#endif
	}
	std::string& append(std::string& str, const char* fmt, ...)
	{
		va_list vl;
		va_start(vl, fmt);
		std::string temp;
		vformat(temp, fmt, vl);
		va_end(vl);
		str += temp;
		return str;
	}
	void split(const std::string& str, std::string::value_type separator, std::vector<std::string>& resultOUT, size_t resultLimit /*= -1*/)
	{
		resultOUT.clear();
		if (resultLimit == 0)
			return;
		resultOUT.push_back(std::string());
		for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
		{
			if (*it == separator)
			{
				if (resultOUT.back().empty() == false)
				{
					if (resultOUT.size() == resultLimit)
						return;
					resultOUT.push_back(std::string());
				}
			}
			else
			{
				resultOUT.back().push_back(*it);
			}
		}
		if (resultOUT.back().empty())
			resultOUT.pop_back();
		Ast(resultOUT.size() <= resultLimit);
	}
	void split(const std::string& str, const std::string& separator, std::vector<std::string>& resultOUT, size_t resultLimit/* = -1*/)
	{
		resultOUT.clear();
		if (resultLimit == 0)
			return;
		if (separator.empty())
		{
			resultOUT.push_back(str);
			return;
		}
		std::string::size_type begin = 0, index;
		for (;;)
		{
			index = str.find(separator, begin);
			if (index == std::string::npos)
				break;
			size_t len = index - begin;
			if (len != 0)
			{
				resultOUT.push_back(str.substr(begin, len));
				if (resultOUT.size() == resultLimit)
					return;
			}
			begin = index + separator.size();
		}
		if (begin < str.size() && resultOUT.size() < resultLimit)
			resultOUT.push_back(str.substr(begin));
		Ast(resultOUT.size() <= resultLimit);
	}
	void split_white_space(const std::string& str, std::vector<std::string>& resultOUT, size_t resultLimit/* = -1*/)
	{
		std::vector<std::string> white_space;
		white_space.push_back(" ");
		white_space.push_back("\t");
		white_space.push_back("\r");
		white_space.push_back("\n");
		white_space.push_back("\f");
		white_space.push_back("\v");
		split(str, white_space, resultOUT, resultLimit);
	}
	bool split_fix_size(const std::string& str, std::string::size_type fix_size, std::vector<std::string>& resultOUT)
	{
		resultOUT.clear();
		if (fix_size == 0)//长度为0时无法处理
			return false;
		else if (fix_size == 1)// 当长度为1时不能处理汉字
		{
			for (std::string::const_iterator iter = str.begin(); iter != str.end(); ++iter)
			{
				if (*iter < 0)
					return false;
			}
		}
		else if (fix_size >= str.size())//长度足够大时不必要处理
		{
			resultOUT.push_back(str);
			return true;
		}
		resultOUT.reserve(str.size() / fix_size + 1);

		bool fullword = true;// 是否是完整字的标志，用于处理完整的汉字
		std::string::const_iterator it = str.begin();
		std::string::const_iterator last = it;
		for (; it != str.end(); ++it)
		{
			if (*it >= 0)
				fullword = true;
			else
				fullword = !fullword;
			if (it - last == static_cast<int32>(fix_size))
			{
				// 发现分界处有汉字断字现象则回退一个字节
				if (fullword && *it < 0)
				{
					--it;
					fullword = false;
				}
				resultOUT.push_back(std::string(last, it));
				last = it;
			}
		}

		if (last != it)
			resultOUT.push_back(std::string(last, it));
		return true;
	}
	std::string& replace(std::string& str, const std::string& from, const std::string& to)
	{
		if (from.empty())
			return str;
		std::string::size_type lastpos = 0, thispos;
		while ((thispos = str.find(from, lastpos)) != std::string::npos)
		{
			str.replace(thispos, from.size(), to);
			lastpos = thispos + to.size();
		}
		return str;
	}
	std::wstring& replace(std::wstring& str, const std::wstring& from, const std::wstring& to)
	{
		if (from.empty())
			return str;
		std::wstring::size_type lastpos = 0, thispos;
		while ((thispos = str.find(from, lastpos)) != std::wstring::npos)
		{
			str.replace(thispos, from.size(), to);
			lastpos = thispos + to.size();
		}
		return str;
	}
	std::string& replace(std::string& str, std::string::value_type from, std::string::value_type to)
	{
		for (std::string::iterator i = str.begin(); i != str.end(); i++)
		{
			if (*i == from)
			{
				*i = to;
			}
		}
		return str;
	}
	std::wstring& replace(std::wstring& str, std::wstring::value_type from, std::wstring::value_type to)
	{
		for (std::wstring::iterator i = str.begin(); i != str.end(); i++)
		{
			if (*i == from)
			{
				*i = to;
			}
		}
		return str;
	}
	int32 integer(const std::string& str)
	{
		int32 retval = 0;
		integer(str, retval);
		return retval;
	}

	bool integer(const std::string& str, int32& valueOut)
	{
		valueOut = 0;
		if (str.empty())
			return false;

		bool neg = false;
		bool error = false;
		for (std::string::const_iterator i = str.begin(); i != str.end(); i++)
		{
			if (i == str.begin())
			{
				if (*i == '-')
				{
					neg = true;
					continue;
				}
				else if (*i == '+')
					continue;
			}
			if (*i >= '0' && *i <= '9')
			{
				valueOut *= 10;
				valueOut += *i - '0';
			}
			else
			{
				error = true;
				break;
			}
		}
		if (neg)
			valueOut = -valueOut;
		return !error;
	}

	bool uinteger(const std::string& str, uint32& valueOut)
	{
		valueOut = 0;
		if (str.empty())
		{
			return false;
		}

		bool error = false;
		for (std::string::const_iterator i = str.begin(); i != str.end(); i++)
		{
			if (*i >= '0' && *i <= '9')
			{
				valueOut *= 10;
				valueOut += *i - '0';
			}
			else
			{
				error = true;
				break;
			}
		}

		return !error;
	}

	bool u64interger(const std::string& str, uint64& valueOut)
	{
		valueOut = 0;
		if (str.empty())
		{
			return false;
		}

		bool error = false;
		for (std::string::const_iterator i = str.begin(); i != str.end(); i++)
		{
			if (*i >= '0' && *i <= '9')
			{
				valueOut *= 10;
				valueOut += *i - '0';
			}
			else
			{
				error = true;
				break;
			}
		}

		return !error;
	}

	std::string& repeat(const std::string& str, std::string::size_type times, std::string& resultOUT)
	{
		resultOUT.clear();
		for (std::string::size_type i = 0; i < times; i++)
		{
			resultOUT.append(str);
		}
		return resultOUT;
	}

	std::string& tolower(std::string& str)
	{
		for (std::string::iterator i = str.begin(); i != str.end(); i++)
		{
			if (*i >= 'A' && *i <= 'Z')
				*i = (*i) + ('a' - 'A');
		}
		return str;
	}
	char* tolower(char* pstr)
	{
		if (!pstr)
			return pstr;
		char* s = pstr;
		for (; *s; ++s)
		{
			if (*s >= 'A' && *s <= 'Z')
				*s = (*s) + ('a' - 'A');
		}
		return pstr;
	}
	std::string& toupper(std::string& str)
	{
		for (std::string::iterator i = str.begin(); i != str.end(); i++)
		{
			if (*i >= 'a' && *i <= 'z')
				*i = (*i) - ('a' - 'A');
		}
		return str;
	}

	std::wstring& tolower(std::wstring& str)
	{
		for (std::wstring::iterator i = str.begin(); i != str.end(); i++)
		{
			if (*i >= L'A' && *i <= L'Z')
				*i = (*i) + (L'a' - L'A');
		}
		return str;
	}
	wchar_t* tolower(wchar_t* pstr)
	{
		if (!pstr)
			return pstr;
		wchar_t* s = pstr;
		for (; *s; ++s)
		{
			if (*s >= L'A' && *s <= L'Z')
				*s = (*s) + (L'a' - L'A');
		}
		return pstr;
	}
	std::wstring& toupper(std::wstring& str)
	{
		for (std::wstring::iterator i = str.begin(); i != str.end(); i++)
		{
			if (*i >= L'a' && *i <= L'z')
				*i = (*i) - (L'a' - L'A');
		}
		return str;
	}

	std::string& swapcase(std::string& str)
	{
		for (std::string::iterator i = str.begin(); i != str.end(); i++)
		{
			if (*i >= 'A' && *i <= 'Z')
				*i += ('a' - 'A');
			else if (*i >= 'a' && *i <= 'z')
				*i -= ('a' - 'A');
		}
		return str;
	}

	bool is_lower(const std::string& str)
	{
		if (str.empty())
			return false;

		for (std::string::const_iterator i = str.begin(); i != str.end(); i++)
		{
			if (*i < 'a' || *i < 'z')
				return false;
		}

		return true;
	}

	bool is_upper(const std::string& str)
	{
		if (str.empty())
			return false;

		for (std::string::const_iterator i = str.begin(); i != str.end(); i++)
		{
			if (*i < 'A' || *i > 'Z')
				return false;
		}

		return true;
	}

	bool is_alpha(const std::string& str)
	{
		if (str.empty())
			return false;

		for (std::string::const_iterator i = str.begin(); i != str.end(); i++)
		{
			if (*i < 'A' || (*i > 'Z' && (*i < 'a' || *i > 'z')))
				return false;
		}
		return true;
	}

	bool is_numeric(const std::string& str)
	{
		if (str.empty())
			return false;

		for (std::string::const_iterator i = str.begin(); i != str.end(); i++)
		{
			if (*i < '0' || *i > '9')
				return false;
		}
		return true;
	}
	bool is_alnum(const std::string& str)
	{
		if (str.empty())
			return false;

		for (std::string::const_iterator i = str.begin(); i != str.end(); i++)
		{
			if (*i < 'A' || *i > 'Z')
				if (*i < '0' || *i > '9')
					if (*i < 'a' || *i > 'z')
						return false;
		}
		return true;
	}

	std::string::size_type count(const std::string& str, const std::string &module)
	{
		std::string::size_type count = 0, last = 0, cur = 0;

		while ((cur = str.find(module, last)) != std::string::npos)
		{
			count++;
			last = cur + module.size();
		}
		return count;
	}

	int32 icompare(const std::string& str1, const std::string& str2)
	{
		if (str1.data() == str2.data())
			return 0;
		int32 ret = (int32)str1.size() - (int32)str2.size();
		if (ret != 0)
			return ret;
		std::string::const_iterator it1 = str1.begin();
		std::string::const_iterator it2 = str2.begin();
		for (; it1 < str1.end()/* && it2 < str2.end()*/; ++it1, ++it2)
		{
			if (*it1 == *it2)
				continue;
			char c1 = *it1;
			char c2 = *it2;
			if (c1 >= 'A' && c1 <= 'Z')
			{
				if (c2 >= 'a' && c2 <= 'z')
				{
					if (c1 - 'A' == c2 - 'a')
						continue;
				}
			}
			else if (c1 >= 'a' && c1 <= 'z')
			{
				if (c2 >= 'A' && c2 <= 'Z')
				{
					if (c1 - 'a' == c2 - 'A')
						continue;
				}
			}
			return c1 - c2;
		}
		return 0;
	}

	static int32 compare_right(const char *a, const char *b)
	{
		int32 bias = 0;

		/* The longest run of digits wins.  That aside, the greatest
		value wins, but we can't know that it will until we've scanned
		both numbers to know that they have the same magnitude, so we
		remember it in BIAS. */
		for (;; a++, b++)
		{
			if (!is_numeric(*a)  &&  !is_numeric(*b))
				return bias;
			else if (!is_numeric(*a))
				return -1;
			else if (!is_numeric(*b))
				return +1;
			else if (*a < *b)
			{
				if (!bias)
					bias = -1;
			}
			else if (*a > *b)
			{
				if (!bias)
					bias = +1;
			}
			else if (!*a  &&  !*b)
				return bias;
		}
	}


	static int32 compare_left(const char *a, const char *b)
	{
		/* Compare two left-aligned numbers: the first to have a
		different value wins. */
		for (;; a++, b++)
		{
			if (!is_numeric(*a)  &&  !is_numeric(*b))
				return 0;
			else if (!is_numeric(*a))
				return -1;
			else if (!is_numeric(*b))
				return +1;
			else if (*a < *b)
				return -1;
			else if (*a > *b)
				return +1;
		}
	}

	int32 compare_nature(const char *str1, const char *str2, bool case_sensitive /*= true*/)
	{
		if (str1 == str2)
			return 0;
		if (str1 == NULL)
			return -1;
		else if (str2 == NULL)
			return 1;

		int32 ai(0), bi(0);
		char ca, cb;
		int32 fractional, result;
		for (;;)
		{
			ca = str1[ai];
			cb = str2[bi];

			/* skip over leading spaces or zeros */
			while (is_whitespace(ca))
				ca = str1[++ai];

			while (is_whitespace(cb))
				cb = str2[++bi];

			/* process run of digits */
			if (is_numeric(ca)  &&  is_numeric(cb))
			{
				fractional = (ca == '0' || cb == '0');

				if (fractional)
				{
					if ((result = compare_left(str1+ai, str2+bi)) != 0)
						return result;
				}
				else
				{
					if ((result = compare_right(str1+ai, str2+bi)) != 0)
						return result;
				}
			}

			if (!ca && !cb)
			{
				/* The strings compare the same.  Perhaps the caller
				will want to call strcmp to break the tie. */
				return 0;
			}
			if (!case_sensitive)
			{
				ca = toupper(ca);
				cb = toupper(cb);
			}
			if (ca < cb)
				return -1;
			else if (ca > cb)
				return +1;

			++ai;
			++bi;
		}
	}
	bool match_wildcard(const char* str, const char* wildcard)
	{
		if (str == NULL || wildcard == NULL)
			return false;
		if (str == wildcard)
			return true;
		// Written by Jack Handy - jakkhandy@hotmail.com
		const char *cp = NULL, *mp = NULL;
		while ((*str) && (*wildcard != '*'))
		{
			if ((*wildcard != *str) && (*wildcard != '?'))
				return false;
			wildcard++;
			str++;
		}
		while (*str)
		{
			if (*wildcard == '*')
			{
				if (!*++wildcard)
					return true;
				mp = wildcard;
				cp = str+1;
			}
			else if ((*wildcard == *str) || (*wildcard == '?'))
			{
				wildcard++;
				str++;
			}
			else
			{
				wildcard = mp;
				str = cp++;
			}
		}

		while (*wildcard == '*')
		{
			wildcard++;
		}
		return (*wildcard) ? false : true;
	}

	uint32 String2ID(const char* str)
	{
		if (str == NULL)
			return 0;
		return sqr::hash::jenkins::hash((uint8*)str, (uint32)strlen(str), 0);
	}

	int32 LevenshteinDistance(const std::string& source, const std::string& target)
	{
		// Step 1
		const int32 n = (uint32)source.length();
		const int32 m = (uint32)target.length();
		if (n == 0)
			return m;
		if (m == 0)
			return n;

		// Good form to declare a TYPEDEF
		typedef std::vector< std::vector<int32> > Tmatrix;

		Tmatrix matrix(n+1);

		// Size the vectors in the 2.nd dimension. Unfortunately C++ doesn't
		// allow for allocation on declaration of 2.nd dimension of vec of vec
		for (int32 i = 0; i <= n; i++)
			matrix[i].resize(m+1);

		// Step 2
		for (int32 i = 0; i <= n; i++)
			matrix[i][0]=i;
		for (int32 j = 0; j <= m; j++)
			matrix[0][j]=j;

		// Step 3
		for (int32 i = 1; i <= n; i++)
		{
			const char s_i = source[i-1];
			// Step 4
			for (int32 j = 1; j <= m; j++)
			{
				const char t_j = target[j-1];
				// Step 5
				int32 cost;
				if (s_i == t_j)
					cost = 0;
				else
					cost = 1;

				// Step 6
				const int32 above = matrix[i-1][j];
				const int32 left = matrix[i][j-1];
				const int32 diag = matrix[i-1][j-1];
				int32 cell = std::min( above + 1, std::min(left + 1, diag + cost));

				// Step 6A: Cover transposition, in addition to deletion,
				// insertion and substitution. This step is taken from:
				// Berghel, Hal ; Roach, David : "An Extension of Ukkonen's
				// Enhanced Dynamic Programming ASM Algorithm"
				// (http://www.acm.org/~hlb/publications/asm/asm.html)
				if (i>2 && j>2)
				{
					int32 trans = matrix[i-2][j-2]+1;
					if (source[i-2]!=t_j) trans++;
					if (s_i!=target[j-2]) trans++;
					if (cell>trans) cell=trans;
				}
				matrix[i][j]=cell;
			}
		}
		// Step 7
		return matrix[n][m];
	}

	//------------------------------------------------------------------------------
	std::string& convert(std::string& dest, const std::wstring& src)
	{
#ifdef _WIN32
		int const mbs_len = WideCharToMultiByte(CP_ACP, 0, src.c_str(), static_cast<int32>(src.size()), NULL, 0, NULL, NULL);
		std::vector<char> tmp(mbs_len+1);
		WideCharToMultiByte(CP_ACP, 0, src.c_str(), static_cast<int32>(src.size()), &tmp[0], mbs_len, NULL, NULL);
		dest.assign(tmp.begin(), tmp.end() - 1);
#else
		locale loc = locale::global(locale(""));
		size_t const mbs_len = wcstombs(NULL, src.c_str(), 0);
		std::vector<char> tmp(mbs_len + 1);
		wcstombs(&tmp[0], src.c_str(), tmp.size());
		dest.assign(tmp.begin(), tmp.end() - 1);
		locale::global(loc);
#endif

		return dest;
	}

	//------------------------------------------------------------------------------
	std::wstring& convert(std::wstring& dest, const std::string& src)
	{
#ifdef _WIN32
		int32 const wcs_len = MultiByteToWideChar(CP_ACP, 0, src.c_str(), static_cast<int32>(src.size()), NULL, 0);
		std::vector<wchar_t> tmp(wcs_len + 1);
		MultiByteToWideChar(CP_ACP, 0, src.c_str(), static_cast<int32>(src.size()), &tmp[0], wcs_len);
#else
		const string old_loc = std::setlocale(LC_CTYPE, NULL);
		std::setlocale(LC_CTYPE, "");

		size_t const wcs_len = mbstowcs(NULL, src.c_str(), 0);
		std::vector<wchar_t> tmp(wcs_len + 1);
		mbstowcs(&tmp[0], src.c_str(), src.size());

		setlocale(LC_CTYPE, old_loc.c_str());
#endif

		dest.assign(tmp.begin(), tmp.end() - 1);

		return dest;
	}

	

	//------------------------------------------------------------------------------
	bool
	match_pattern( const string& path, const string& pattern )
	{
		const char* str = path.c_str();
		const char* pat = pattern.c_str();

		bool always = true;
		while (always != false)
		{
			if (*pat == 0) 
			{
				if (*str == 0) return true;
				else           return false;
			}
			if ((*str == 0) && (*pat != '*')) return false;
			if (*pat=='*') 
			{
				pat++;
				if (*pat==0) return true;
				while (always)
				{
					if (match_pattern(str, pat)) return true;
					if (*str==0) return false;
					str++;
				}
			}
			if (*pat=='?') goto match;

			if (*pat=='\\') 
			{
				pat++;
				if (*pat==0) return false;
			}
			if (*pat!=*str) return false;

match:
			pat++;
			str++;
		}
		// can't happen
		return false;
	}

	
	int StringFindBaseOnUTF8(const char* str, const char* subStr, int beginIndex)
	{
		if( !str || !subStr )
			return -1;

		wstring strUni = utf8_to_utf16(str);
		wstring substrUni = utf8_to_utf16(subStr);
		size_t pos =  strUni.find(substrUni.c_str(), beginIndex);

		int notFindIndex = -1;
		if (pos == string::npos)
			return notFindIndex;
		else
			return (int)pos;
	}

	bool is_sbc(uint32 nChar)
	{
		if (	( nChar >= 65296 && nChar <= 65305 ) ||	//数字
				( nChar >= 65313 && nChar <= 65338 ) ||	//大写字母
				( nChar >= 65345 && nChar <= 65370 ) )	//小写字母

				//( nChar >= 65281 && nChar <= 65374 )  //Latin (这个范围好像有问题)
				//( nChar >= 65298 && nChar <= 65312 )  //
				//( nChar >= 12340 && nChar <= 12447 )  //平仮名
				//( nChar >= 12448 && nChar <= 12543 )  //片仮名
				//( nChar >= 65504 && nChar <= 65509 )  //Symbol   
		{
			return true;
		}

		return false;
	}

	int32 find_sbc_on_utf8(const char* str)
	{
		wstring utf16_str;
		utf8_to_utf16(utf16_str, str);
		const wchar_t* Buf = utf16_str.c_str();

		if ( Buf )
		{
			uint32 i = 0;
			while ( *Buf != 0 )
			{
				if ( is_sbc( Buf[0] ) )
					return i;

				i += 1;
				Buf++;
			}
		}

		return -1;
	}

	int GetCharCount( const char* str )
	{
		wstring u16_str = utf8_to_utf16(str);

		int char_count = 0;
		for ( size_t i = 0; i<u16_str.length(); ++i )
		{
			if ( u16_str[i] >= 0 && u16_str[i] <= 127 )
			{
				++char_count;
			}
			else
			{
				char_count += 2;
			}
		}

		return char_count;
	}

	void substr_u8( string& result, const string& str, size_t start, size_t len )
	{
		wstring wstr,wstr2;
		utf8_to_utf16(wstr, str);
		wstr2 = wstr.substr(start, len);
		utf16_to_utf8(result, wstr2);
	}

}// namespace sqr
