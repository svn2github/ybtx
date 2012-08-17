#pragma once
#include <Winsock2.h>
#include <string>
using std::string;
/*
namespace sqr
{
const string ReturnWSError (void);

inline timeval addtimeval(const timeval& time1, const timeval& time2)
{
	timeval time;

	time.tv_sec = time1.tv_sec + time2.tv_sec;
	time.tv_usec = time1.tv_usec + time2.tv_usec;

	if(time.tv_usec < 0)
	{
		time.tv_sec--;
		time.tv_usec += + 1000000;
	}
	else if(time.tv_usec >= 1000000)
	{
		time.tv_sec++;
		time.tv_usec -= 1000000;
	}

	return time;
}

inline timeval subtimeval(const timeval& time1, const timeval& time2)
{
    timeval time;

    time.tv_sec = time1.tv_sec - time2.tv_sec;
    time.tv_usec = time1.tv_usec - time2.tv_usec;
   
 	if(time.tv_usec < 0)
	{
		time.tv_sec--;
		time.tv_usec += + 1000000;
	}
	else if(time.tv_usec >= 1000000)
	{
		time.tv_sec++;
		time.tv_usec -= 1000000;
	}
   
    return time;
}
// 大于返回  1
// 等于返回  0
// 小于返回 -1

inline int cmptimeval(const timeval& time1, const timeval& time2)   
{
    if(time1.tv_sec > time2.tv_sec)
    {
		return 1;
    }
    else if(time1.tv_sec < time2.tv_sec)
    {
		return -1;
    }
    else if(time1.tv_usec > time2.tv_usec)
    {
		return 1;
    }
    else if(time1.tv_usec < time2.tv_usec)
    {
		return -1;
    }
    return 0;
}

class SimpleHttp
{
protected:
	string	_host;
	string	_port;
	timeval	_timeout;
	timeval _time_left;
	timeval _timeout_after_connect;

	string	_request;
	string	_response;

	SOCKET	_socket;
	string	_lastErr;
	bool	_OK;

	void			ProcessRequest();
	virtual void	Connect() = 0;
	virtual void	SendRequest() = 0;
	virtual void	StoreResponse() = 0;
	virtual void	Disconnect() = 0;

public:
	SimpleHttp(const string& host, const string& port, const timeval& timeout, const timeval& timeout_after_connect);
	virtual ~SimpleHttp();

	bool			CheckHttpOK(void);
	void			SetRequest(const string& request);

	//const string*	GetResponse() const { return &_response; }
	string			GetResponse() const { return _response; }
	string			GetContent()  const;

	const string	GetErrMsg() const { return _lastErr; }
	const bool		IsFinishOK() const { return _OK; }
};

class CommHttp:public SimpleHttp
{
private:
	virtual	void	Connect();
	virtual void	SendRequest();
	virtual void	StoreResponse();
	virtual void	Disconnect();

public:
	CommHttp(const string& host, const string& port, const timeval& timeout, const timeval& timeout_after_connect);
	virtual ~CommHttp();
};

class AsyncSocket
{
public:
	enum Socket_Stat {
		STAT_READ	= 1<<0,
		STAT_WRITE	= 1<<1,
		STAT_EXCEPT	= 1<<2,
	};

private:
	string	_host;
	string	_port;

	timeval	_timeout;

	SOCKET	_socket;
	string	_lastErr;

	bool	_is_connected;

public:
	AsyncSocket();
	virtual ~AsyncSocket();

	bool			is_connected() const { return _is_connected; }
	bool			connect(const string& host, const string& port, const timeval& timeout);
	void			disconnect();

	int				select(const Socket_Stat stat, const timeval* time);
	virtual int		send(const TCHAR* buf, int len, int flags = 0);
	virtual int		recv(TCHAR* buf, int len, int flags = 0);

	const string	GetErrMsg() const { return _lastErr; }

};

}
*/
