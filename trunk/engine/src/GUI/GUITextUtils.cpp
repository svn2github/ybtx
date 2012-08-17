#include "stdafx.h"
#include "GUITextUtils.h"

using namespace std;


namespace sqr
{
	/*************************************************************************
		Constants
	*************************************************************************/
	const UIWString    GUITextUtils::DefaultWhitespace(L" \n\t\r");
	const UIWString    GUITextUtils::DefaultAlphanumerical(L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890");
	const UIWString    GUITextUtils::DefaultWrapDelimiters(L" \n\t\r");


	/*************************************************************************
		return a UIWString containing the the next word in a UIWString.
	*************************************************************************/
	UIWString GUITextUtils::getNextWord(const UIWString& str, UIWString::size_type start_idx, const UIWString& delimiters)
	{
		UIWString::size_type   word_start = str.find_first_not_of(delimiters, start_idx);

		if (word_start == UIWString::npos)
		{
			word_start = start_idx;
		}

		UIWString::size_type   word_end = str.find_first_of(delimiters, word_start);

		if (word_end == UIWString::npos)
		{
			word_end = str.length();
		}

		return str.substr(start_idx, (word_end - start_idx));
	}


	/*************************************************************************
		Return the index of the first character of the word at 'idx'.
	*************************************************************************/
	UIWString::size_type GUITextUtils::getWordStartIdx(const UIWString& str, UIWString::size_type idx)
	{
		UIWString  temp = str.substr(0, idx);

		trimTrailingChars(temp, DefaultWhitespace);

		if (temp.length() <= 1)
		{
			return 0;
		}

		// identify the type of character at 'pos'
		if (UIWString::npos != DefaultAlphanumerical.find(temp[temp.length() - 1]))
		{
			idx = temp.find_last_not_of(DefaultAlphanumerical);
		}
		// since whitespace was stripped, character must be a symbol
		else
		{
			idx = temp.find_last_of(DefaultAlphanumerical + DefaultWhitespace);
		}

		// make sure we do not go past end of UIString (+1)
		if (idx == UIWString::npos)
		{
			return 0;
		}
		else
		{
			return idx + 1;
		}

	}


	/*************************************************************************
		Return the index of the first character of the word after the word
		at 'idx'.
	*************************************************************************/
	UIWString::size_type GUITextUtils::getNextWordStartIdx(const UIWString& str, UIWString::size_type idx)
	{
		UIWString::size_type str_len = str.length();

		// do some checks for simple cases
		if ((idx >= str_len) || (str_len == 0))
		{
			return str_len;
		}

		// is character at 'idx' alphanumeric
		if (UIWString::npos != DefaultAlphanumerical.find(str[idx]))
		{
			// find position of next character that is not alphanumeric
			idx = str.find_first_not_of(DefaultAlphanumerical, idx);
		}
		// is character also not whitespace (therefore a symbol)
		else if (UIWString::npos == DefaultWhitespace.find(str[idx]))
		{
			// find index of next character that is either alphanumeric or whitespace
			idx = str.find_first_of(DefaultAlphanumerical + DefaultWhitespace, idx);
		}

		// check result at this stage.
		if (UIWString::npos == idx)
		{
			idx = str_len;
		}
		else
		{
			// if character at 'idx' is whitespace
			if (UIWString::npos != DefaultWhitespace.find(str[idx]))
			{
				// find next character that is not whitespace
				idx = str.find_first_not_of(DefaultWhitespace, idx);
			}

			if (UIWString::npos == idx)
			{
				idx = str_len;
			}

		}

		return idx;
	}


	/*************************************************************************
		Trim all characters from the set specified in \a chars from the
		begining of 'str'.
	*************************************************************************/
	void GUITextUtils::trimLeadingChars(UIWString& str, const UIWString& chars)
	{
		UIWString::size_type idx = str.find_first_not_of(chars);

		if (idx != UIWString::npos)
		{
			str.erase(0, idx);
		}
		else
		{
			str.erase();
		}

	}


	/*************************************************************************
		Trim all characters from the set specified in \a chars from the end
		of 'str'.
	*************************************************************************/
	void GUITextUtils::trimTrailingChars(UIWString& str, const UIWString& chars)
	{
		UIWString::size_type idx = str.find_last_not_of(chars);

		if (idx != UIWString::npos)
		{
			str.resize(idx + 1);
		}
		else
		{
			str.erase();
		}
	}

}
