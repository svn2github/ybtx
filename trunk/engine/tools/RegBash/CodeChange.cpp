
#include "stdafx.h"
#include "CodeChange.h"

// 把英文字母的单字节格式转成双字节格式
int ANSItoUnicode(char str[], char NewStr[])
{
	int i;
	int flag = 1;
	int len = (int)strlen(str);
	int NewLen = (len+1)*2;

	for (i=0; i<NewLen; i++)
	{
		if (flag==1)
		{
			NewStr[i] = str[i/2];
			flag = 0;
		}
		else
		{
			NewStr[i] = '\0';
			flag = 1;
		}
	}

	return NewLen;
}


// 把英文字母的双字节格式转成单字节格式
void UnicodetoANSI(char str[], char NewStr[], int len)
{
	int i;
	int flag = 1;
	int NewLen = len/2;
	for (i=0; i<NewLen; i++)
	{
		NewStr[i] = str[i*2];
	}
}
