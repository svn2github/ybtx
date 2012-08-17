#pragma once

const int NOT_FIND_IN_SEARCH_DIR = -1;

CString&				GetRootPath();
CString&				GetResPath();
CString&				GetOpenPath();
CString&				GetTexPath();
const vector<CString>	GetMutiFileNameTex(const wchar_t* Filter);
void					MakePathLower(wstring& path);