#include "stdafx.h"
#include "net_model.h"
#include <cstdio>
#include "zlib/zlib.h"
/*

#ifdef WIN32

inline int gettimeofday(struct timeval *tp, struct timezone *tzp)
{
	DWORD tick = GetTickCount();
	tp->tv_sec = tick/1000;
	tp->tv_usec = (tick % 1000) * 1000;
	return 0;
}

#endif


namespace sqr
{

bool SimpleHttp::CheckHttpOK()
{
        string::size_type pos;
        pos = _response.find("\n");

        if(_response.rfind("200 OK", pos) != string::npos) 
        {
			_OK = true;
			return true;
        }
		
		string::size_type first_space_pos = _response.find(" ");
		if(first_space_pos != string::npos && pos != string::npos && first_space_pos+1<pos)
		{
			_lastErr.assign(_response, first_space_pos, pos - first_space_pos - 1);
		}

		_OK = false;
        return false;
}

SimpleHttp::SimpleHttp(const string& host, const string& port, const timeval& timeout, const timeval& timeout_after_connect):
_host(host),
_port(port),
_timeout(timeout),
_time_left(timeout),
_timeout_after_connect(timeout_after_connect),
_socket(INVALID_SOCKET),
_OK(false)
{

}

SimpleHttp::~SimpleHttp()
{
	if( _socket != INVALID_SOCKET )
	{
		::closesocket(_socket);
		_socket = INVALID_SOCKET;
	}
}

void SimpleHttp::SetRequest(const string& request)
{
	_request = request;
	ProcessRequest();
}


void SimpleHttp::ProcessRequest()
{
	Connect();
	if( _socket == INVALID_SOCKET ) return;

	if(_timeout_after_connect.tv_sec != 0 || _timeout_after_connect.tv_usec != 0)
	{
		_time_left = _timeout_after_connect;
	}
	
	SendRequest();
	if( _socket == INVALID_SOCKET ) return;
	StoreResponse();
	if( _socket == INVALID_SOCKET ) return;
	Disconnect();
	CheckHttpOK();
}

string SimpleHttp::GetContent() const
{
	if(!_OK)
	{
		return "";
	}

	string::size_type pos = _response.find("\r\n\r\n");

	if(pos == string::npos)
	{
		return "";
	}

	string header(_response, 0, pos);
	string content(_response, pos + 4, _response.size() - pos - 4);

	string::size_type gzip = header.find("Content-Encoding: gzip", 0);

	if(gzip != string::npos)
	{
		wchar_t szTempName[MAX_PATH+32];
		wchar_t szTempPath[MAX_PATH+32];
		char	gzbuf[4096];

		GetTempPathW(sizeof(szTempPath), szTempPath);

		GetTempFileNameW(szTempPath, // directory for temp files 
		L"gp",						// temp file name prefix 
		0,							// create unique name 
		szTempName);				// buffer for name 

		Bytef* psrc = (Bytef*)(content.data());

		FILE* f = _wfopen(szTempName, L"wb");

		if(f == NULL)
		{
			return "";
		}

		fwrite(psrc, content.size(), 1, f);
		fclose(f);

		gzFile zf = gzopen(szTempName, L"rb");
		if(zf == NULL)
		{
			_wunlink(szTempName);
			return "";
		}

		content = "";

		for(;;)
		{
			int rc = gzread(zf, gzbuf, sizeof(gzbuf));
			if(rc > 0)
			{
				content.append(gzbuf, rc);
			}
			else if(rc == 0)
			{
				break;		
			}
			else
			{
				gzclose(zf);
				_wunlink(szTempName);
				return false;
			}
		}

		gzclose(zf);
		_wunlink(szTempName);
		return content;
	}
	else
	{
		return content;
	}
	
	return "";
}

///////////////////////////////////////////////////////////////////////
//
// CommHttp
//
///////////////////////////////////////////////////////////////////////
	
CommHttp::CommHttp(const string& host, const string& port, const timeval& timeout, const timeval& timeout_after_connect):
SimpleHttp(host, port, timeout, timeout_after_connect)
{

}

CommHttp::~CommHttp()
{

}

void CommHttp::Connect()
{
	LPHOSTENT hp;
	SOCKADDR_IN ServerAddr;

	_socket = ::socket(AF_INET, SOCK_STREAM, 0); // Open a socket
	if (_socket == INVALID_SOCKET ) {
		_lastErr += ReturnWSError();
		return;
	}

	unsigned long noblock = 1;
	int rc = ::ioctlsocket(_socket, FIONBIO, &noblock);

	if(rc !=  0)
	{
		_lastErr += ReturnWSError();
		::closesocket(_socket);
		_socket = INVALID_SOCKET;
		return;
	}

	::memset(&ServerAddr, 0, sizeof(ServerAddr));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = static_cast<u_short>(::htons(atoi(_port.c_str())));

	if(_host.find_first_not_of(".0123456789") != string::npos) 
	{	// server address is a name 
		hp = ::gethostbyname(_host.c_str());
		if (hp == NULL) {
			_lastErr += ReturnWSError();
			::closesocket(_socket);
			_socket = INVALID_SOCKET;
			return;
		}
		::memcpy((TCHAR *)&(ServerAddr.sin_addr), hp->h_addr,
			hp->h_length);
	} 
	else
	{	// Convert nnn.nnn address to a usable one
		ServerAddr.sin_addr.s_addr = ::inet_addr(_host.c_str());
	} 

	if( ::connect(_socket, (LPSOCKADDR)&ServerAddr, sizeof(ServerAddr) ) == SOCKET_ERROR )
	{
		if( WSAGetLastError() ==  WSAEWOULDBLOCK )
		{
			fd_set	writeset;
			FD_ZERO(&writeset);
			FD_SET(_socket, &writeset);

			timeval tv1;
			gettimeofday(&tv1, NULL);
			if( ::select((int)_socket+1, NULL, &writeset, NULL, &_time_left) == SOCKET_ERROR)
			{
				_lastErr += ReturnWSError();
				::closesocket(_socket);
				_socket = INVALID_SOCKET;
				return;
			}
			if(!FD_ISSET(_socket, &writeset))
			{
				_lastErr += ReturnWSError();
				::closesocket(_socket);
				_socket = INVALID_SOCKET;
				return;
			}

			timeval tv2;
			gettimeofday(&tv2, NULL);

			timeval tuse;

			tuse = subtimeval(tv2, tv1);
			if( cmptimeval( _time_left, tuse ) == 1 )
			{
				_time_left = subtimeval(_time_left, tuse);
			}
			else
			{
				_lastErr += "Time Out!!!";
				::closesocket(_socket);
				_socket = INVALID_SOCKET;
				return;
			}
		}
		else
		{
			_lastErr += ReturnWSError();
			::closesocket(_socket);
			_socket = INVALID_SOCKET;
			return;
		}
	}
}

void CommHttp::SendRequest()
{
	int len = static_cast<int>(_request.size());
	if(len == 0) return;

	int rc = ::send(_socket, _request.c_str(), len, 0);
	if(rc != len)
	{
		_lastErr += ReturnWSError();
		::closesocket(_socket);
		_socket = INVALID_SOCKET;
		return;
	}

}

void CommHttp::StoreResponse()
{
	fd_set	readset;
	FD_ZERO(&readset);
	FD_SET(_socket, &readset);

	for(;;)
	{
		timeval tv1;
		gettimeofday(&tv1, NULL);
		if( ::select((int)_socket+1, &readset, NULL, NULL, &_time_left) == SOCKET_ERROR)
		{
			_lastErr += ReturnWSError();
			::closesocket(_socket);
			_socket = INVALID_SOCKET;
			return;
		}
		if(!FD_ISSET(_socket, &readset))
		{
			_lastErr += ReturnWSError();
			::closesocket(_socket);
			_socket = INVALID_SOCKET;
			return;
		}

		timeval tv2;
		gettimeofday(&tv2, NULL);

		TCHAR buf[4096];

		int rc = ::recv(_socket, (TCHAR*)buf, sizeof(buf), 0);

		if( rc == SOCKET_ERROR )
		{
			_lastErr += ReturnWSError();
			::closesocket(_socket);
			_socket = INVALID_SOCKET;
			return;
		}
		else if( rc == 0 )
		{
			return;
		}
		else if( rc > 0)
		{
			_response.append(buf, rc);

			timeval tuse;

			tuse = subtimeval(tv2, tv1);
			if( cmptimeval( _time_left, tuse ) == 1 )
			{
				_time_left = subtimeval(_time_left, tuse);
			}
			else
			{
				_lastErr += "Time Out!!!";
				::closesocket(_socket);
				_socket = INVALID_SOCKET;
				return;
			}

			continue;
		}
	}
}

void CommHttp::Disconnect()
{
	::closesocket(_socket);
	_socket = INVALID_SOCKET;
}

///////////////////////////////////////////////////////////////////////
//
// AsyncSocket
//
///////////////////////////////////////////////////////////////////////

AsyncSocket::AsyncSocket():
_is_connected(false),
_socket(INVALID_SOCKET)
{

}

AsyncSocket::~AsyncSocket()
{
	disconnect();
}

bool AsyncSocket::connect(const string& host, const string& port, const timeval& timeout)
{
	_host = host;
	_port = port;
	_timeout = timeout;

	LPHOSTENT hp;
	SOCKADDR_IN ServerAddr;

	disconnect();

	_socket = ::socket(AF_INET, SOCK_STREAM, 0); // Open a socket
	if (_socket == INVALID_SOCKET ) {
		_lastErr += ReturnWSError();
		return false;
	}

	unsigned long noblock = 1;
	int rc = ::ioctlsocket(_socket, FIONBIO, &noblock);

	if(rc !=  0)
	{
		_lastErr += ReturnWSError();
		disconnect();
		return false;
	}

	::memset(&ServerAddr, 0, sizeof(ServerAddr));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = static_cast<u_short>(::htons(atoi(_port.c_str())));

	if(_host.find_first_not_of(".0123456789") != string::npos) 
	{	// server address is a name
		hp = ::gethostbyname(_host.c_str());
		if (hp == NULL) {
			_lastErr += ReturnWSError();
			disconnect();
			return false;
		}
		::memcpy((TCHAR *)&(ServerAddr.sin_addr), hp->h_addr,
			hp->h_length);
	} 
	else
	{	// Convert nnn.nnn address to a usable one
		ServerAddr.sin_addr.s_addr = ::inet_addr(_host.c_str());
	} 

	if( ::connect(_socket, (LPSOCKADDR)&ServerAddr, sizeof(ServerAddr) ) == SOCKET_ERROR )
	{
		if( WSAGetLastError() ==  WSAEWOULDBLOCK )
		{
			fd_set	writeset;
			FD_ZERO(&writeset);
			FD_SET(_socket, &writeset);

			if( ::select((int)_socket+1, NULL, &writeset, NULL, &_timeout) == SOCKET_ERROR)
			{
				_lastErr += ReturnWSError();
				disconnect();
				return false;
			}
			if(!FD_ISSET(_socket, &writeset))
			{
				_lastErr += ReturnWSError();
				disconnect();
				return false;
			}
			
			_is_connected = true;
			return true;
		}
		else
		{
			_lastErr += ReturnWSError();
			disconnect();
			return false;
		}
	}
	return false;
}

void AsyncSocket::disconnect()
{
	if(_is_connected)
	{
		if(_socket != INVALID_SOCKET)
		{
			::closesocket(_socket);
			_socket = INVALID_SOCKET;
		}
		_is_connected = false;
	}
}

int AsyncSocket::select(const Socket_Stat stat, const timeval* time)
{
	fd_set	set;
	FD_ZERO(&set);
	FD_SET(_socket, &set);

	int rc;

	if(stat == STAT_READ)
	{
		rc = ::select((int)_socket+1, &set, NULL, NULL, time);
	}
	else if(stat == STAT_WRITE)
	{
		rc = ::select((int)_socket+1, NULL, &set, NULL, time);
	}
	else//(stat == STAT_EXCEPT)
	{
		rc = ::select((int)_socket+1, NULL, NULL, &set, time);
	}

	if(rc > 0)
	{
		if(FD_ISSET(_socket, &set))
			return 1;
		else
			return -1;
	}
	else if(rc == 0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

int AsyncSocket::send(const TCHAR* buf, int len, int flags)
{
	return ::send(_socket, buf, len, flags);
}

int AsyncSocket::recv(TCHAR* buf, int len, int flags)
{
	return ::recv(_socket, buf, len, flags);
}

const string ReturnWSError (void)
{
	string errMsg;
	int err = ::WSAGetLastError();
	switch(err)
	{
	case WSAVERNOTSUPPORTED:
	  errMsg = "version of WinSock not supported";
	  break;
	case WSASYSNOTREADY:
	  errMsg =  "WinSock not present or not responding";
	  break;
	case WSAEINVAL:
	  errMsg = "app version not supported by DLL";
	  break;
	case WSAHOST_NOT_FOUND:
	  errMsg =  "Authoritive: Host not found";
	  break;
	case WSATRY_AGAIN:
	  errMsg =  "Non-authoritive: host not found or server failure";
	  break;
	case WSANO_RECOVERY:
	  errMsg =  "Non-recoverable: refused or not implemented";
	  break;
	case WSANO_DATA:
	  errMsg =  "Valid name, no data record for type";
	  break;
//	case WSANO_ADDRESS:
//	    errMsg =  "Valid name, no MX record";
//	  break;
	case WSANOTINITIALISED:
	  errMsg =  "WSA Startup not initialized";
	  break;
	case WSAENETDOWN:
	  errMsg =  "Network subsystem failed";
	  break;
	case WSAEINPROGRESS:
	  errMsg =  "Blocking operation in progress";
	  break;
	case WSAEINTR:
	  errMsg =  "Blocking call cancelled";
	  break;
	case WSAEAFNOSUPPORT:
	  errMsg =  "address family not supported";
	  break;
	case WSAEMFILE:
	  errMsg =  "no file descriptors available";
	  break;
	case WSAENOBUFS:
	  errMsg =  "no buffer space available";
	  break;
	case WSAEPROTONOSUPPORT:
	  errMsg =  "specified protocol not supported";
	  break;
	case WSAEPROTOTYPE:
	  errMsg =  "protocol wrong type for this socket";
	  break;
	case WSAESOCKTNOSUPPORT:
	  errMsg =  "socket type not supported for address family";
	  break;
	case WSAENOTSOCK:
	  errMsg =  "descriptor is not a socket";
	  break;
	case WSAEWOULDBLOCK:
	  errMsg =  "socket marked as non-blocking and SO_LINGER set not 0";
	  break;
	case WSAEADDRINUSE:
	  errMsg =  "address already in use";
	  break;
	case WSAECONNABORTED:
	  errMsg =  "connection aborted";
	  break;
	case WSAECONNRESET:
	  errMsg =  "connection reset";
	  break;
	case WSAENOTCONN:
	  errMsg =  "not connected";
	  break;
	case WSAETIMEDOUT:
	  errMsg =  "connection timed out";
	  break;
	case WSAECONNREFUSED:
	  errMsg =  "connection refused";
	  break;
	case WSAEHOSTDOWN:
	  errMsg =  "host down";
	  break;
	case WSAEHOSTUNREACH:
	  errMsg =  "host unreachable";
	  break;
	case WSAEADDRNOTAVAIL:
	  errMsg =  "address not available";
	  break;
	default:
		{
			TCHAR code[64];
			::_snprintf(code, sizeof(code), "Err code: %d", err);
			errMsg =  code;
		}
		break;
	}
	return errMsg;
}

}	// end of namespace

*/