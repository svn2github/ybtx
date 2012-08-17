/*
* 启动器进程操作
*/
#ifndef _PROCESS_H
#define _PROCESS_H

BOOL ProcessActive( const wchar_t* szProcessName );

BOOL StartupProcessAndQuit( LPCWSTR AppName, LPWSTR szCmdLine, LPCWSTR szDir, DWORD& err_code );

void StartLogUpload( wstring strUrlPath );

void CutString( wchar_t* szBuf );

void TransferBacklash( wchar_t* szBuf );

#endif