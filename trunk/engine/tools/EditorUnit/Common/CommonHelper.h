#pragma once

const CString GetFileName( BOOL bOpenFileDialog, const TCHAR* Filter, 
						   OPENFILENAME* pOF = NULL, LPCTSTR InitName = NULL );
