
#include "stdafx.h"

#include "CHTTPDownloader.h"
#include "net_model.h"


CHTTPDownloader::CHTTPDownloader()
{
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD( 2, 0 );
	WSAStartup( wVersionRequested, &wsaData );
}

CHTTPDownloader::~CHTTPDownloader()
{
	WSACleanup();
}

bool CHTTPDownloader::Download(string& host, string& file)
{
	string port("80");

	timeval tv;
	tv.tv_sec = 15;
	tv.tv_usec = 0;

	timeval tv_after_connect;
	tv_after_connect.tv_sec = 60;
	tv_after_connect.tv_usec = 0;

	CommHttp http(host, port, tv, tv_after_connect); // 初始化http函数数值

	string request = "GET ";
	request += file;
	request += " HTTP/1.1\r\nHost:";
	request += host;
	request += "\r\nAccept-Encoding:*/*\r\nConnection:Close\r\n\r\n";
	http.SetRequest(request); // 下载请求

	if(!http.IsFinishOK())
		return false;

	m_strContent = http.GetContent();

	return true;
}

