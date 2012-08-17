#pragma once
#include "SQRGuiMallocObject.h"
namespace sqr
{
	/*!
	\brief
		Text utility support class.  This class is all static members.  You do not create instances of this class.
	*/
	class GUITextUtils : public SQRGuiMallocObject
	{
	public:
		/*************************************************************************
			Constants
		*************************************************************************/
		static const UIWString DefaultWhitespace;      //!< The default set of whitespace
		static const UIWString DefaultAlphanumerical;  //!< default set of alphanumericals.
		static const UIWString DefaultWrapDelimiters;  //!< The default set of word-wrap delimiters


		/*************************************************************************
			Methods
		*************************************************************************/
		/*!
		\brief
			return a UIWString containing the the next word in a UIWString.

			This method returns a UIWString object containing the the word, starting at index \a start_idx, of UIWString \a str
			as delimited by the code points specified in UIString \a delimiters (or the ends of the input UIString).

		\param str
			UIWString object containing the input data.

		\param start_idx
			index into \a str where the search for the next word is to begin.  Defaults to start of \a str.

		\param delimiters
			UIWString object containing the set of delimiter code points to be used when determining the start and end
			points of a word in UIString \a str.  Defaults to whitespace.

		\return
			UIWString object containing the next \a delimiters delimited word from \a str, starting at index \a start_idx.
		*/
		static  UIWString  getNextWord(const UIWString& str, UIWString::size_type start_idx = 0, const UIWString& delimiters = DefaultWhitespace);


		/*!
		\brief
			Return the index of the first character of the word at \a idx.

		/note
			This currently uses DefaultWhitespace and DefaultAlphanumerical to determine groupings for what constitutes a 'word'.

		\param str
			UIWString containing text.

		\param idx
			Index into \a str where search for start of word is to begin.

		\return
			Index into \a str which marks the begining of the word at index \a idx.
		*/
		static  UIWString::size_type   getWordStartIdx(const UIWString& str, UIWString::size_type idx);


		/*!
		\brief
			Return the index of the first character of the word after the word at \a idx.

		/note
			This currently uses DefaultWhitespace and DefaultAlphanumerical to determine groupings for what constitutes a 'word'.

		\param str
			UIWString containing text.

		\param idx
			Index into \a str where search is to begin.

		\return
			Index into \a str which marks the begining of the word at after the word at index \a idx.
			If \a idx is within the last word, then the return is the last index in \a str.
		*/
		static  UIWString::size_type   getNextWordStartIdx(const UIWString& str, UIWString::size_type idx);


		/*!
		\brief
			Trim all characters from the set specified in \a chars from the begining of \a str.

		\param str
			UIWString object to be trimmed.

		\param chars
			UIWString object containing the set of code points to be trimmed.
		*/
		static  void    trimLeadingChars(UIWString& str, const UIWString& chars);


		/*!
		\brief
			Trim all characters from the set specified in \a chars from the end of \a str.

		\param str
			UIWString object to be trimmed.

		\param chars
			UIWString object containing the set of code points to be trimmed.
		*/
		static  void    trimTrailingChars(UIWString& str, const UIWString& chars);

	private:
		/*************************************************************************
			Data
		*************************************************************************/
		static  UIWString      d_delimiters;           //!< Current set of delimiters.
		static  UIWString      d_whitespace;           //!< Current set of whitespace.


		/*************************************************************************
			Construction / Destruction
		*************************************************************************/
		/*!
		\brief
			Constructor and Destructor are private.  This class has all static members.
		*/
		GUITextUtils(void);
		~GUITextUtils(void);
	};

}
