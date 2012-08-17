#include "StdAfx.h"
#include "FileStream.h"
#include "IoSystem.h"
#include "Memory.h"

namespace sqr
{

//------------------------------------------------------------------------------
FileStream::FileStream()
		: handle(INVALID_HANDLE_VALUE)
{
	// empty
}

//------------------------------------------------------------------------------
FileStream::~FileStream()
{
	Ast(INVALID_HANDLE_VALUE == this->handle);
}

//------------------------------------------------------------------------------
bool FileStream::Open()
{
	Ast(INVALID_HANDLE_VALUE == this->handle);

	if (Stream::Open())
	{
		string fullPath = IoSystem::GetInst()->GetFullPath(this->path);
		DWORD access = 0;
		DWORD disposition = 0;
		DWORD shareMode = 0;
		switch (this->GetAccessMode())
		{
		case Stream::ReadAccess:
			access = GENERIC_READ;
			disposition = OPEN_EXISTING;
			shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
			break;
		case Stream::WriteAccess:
			access = GENERIC_WRITE;
			disposition = CREATE_ALWAYS;
			shareMode = FILE_SHARE_READ;
			break;
		case Stream::ReadWriteAccess:
			access = GENERIC_READ | GENERIC_WRITE;
			disposition = OPEN_ALWAYS;
			shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
			break;
		}

		// open/create the file
		this->handle = CreateFile(fullPath.c_str(),			// lpFileName
		                          access,					// dwDesiredAccess
		                          shareMode,				// dwShareMode
		                          0,						// lpSecurityAttributes
		                          disposition,				// dwCreationDisposition,
		                          FILE_FLAG_SEQUENTIAL_SCAN,// dwFlagsAndAttributes
		                          NULL);					// hTemplateFile
		if (INVALID_HANDLE_VALUE != this->handle)
			return true;
	}
	// failed
	Stream::Close();
	return false;
}

//------------------------------------------------------------------------------
void FileStream::Close()
{
	Ast(INVALID_HANDLE_VALUE != this->handle);

	Stream::Close();
	CloseHandle(this->handle);
	this->handle = INVALID_HANDLE_VALUE;
}

//------------------------------------------------------------------------------
index_t FileStream::GetPosition() const
{
	Ast(INVALID_HANDLE_VALUE != this->handle);

	return SetFilePointer(this->handle, 0, NULL, FILE_CURRENT);
}

//------------------------------------------------------------------------------
size_t
FileStream::GetSize() const
{
	Ast(INVALID_HANDLE_VALUE != this->handle);

	return GetFileSize(this->handle, NULL);
}

//------------------------------------------------------------------------------
size_t FileStream::Read( void* data, size_t size )
{
	Ast(INVALID_HANDLE_VALUE != this->handle);
	Stream::Read(data, size);

	DWORD bytesRead;
	BOOL result = ReadFile(handle, data, size, &bytesRead, NULL);
	if (0 == result)
	{
		GenErr("FileStream::Read() failed!");
	}
	return bytesRead;
}

//------------------------------------------------------------------------------
void FileStream::Write( void* data, size_t size )
{
	Ast(INVALID_HANDLE_VALUE != this->handle);
	Stream::Write(data, size);

	DWORD bytesWritten;
	BOOL result = WriteFile(handle, data, size, &bytesWritten, NULL);
	if ((0 == result) || (size != bytesWritten))
	{
		GenErr("FileStream::Write() failed!");
	}
}

//------------------------------------------------------------------------------
void FileStream::Seek( int offset, SeekOrigin dir )
{
	Ast(INVALID_HANDLE_VALUE != this->handle);
	Stream::Seek(offset, dir);

	DWORD moveMethod;
	switch (dir)
	{
	case Stream::Begin:
		moveMethod = FILE_BEGIN;
		break;
	case Stream::Current:
		moveMethod = FILE_CURRENT;
		break;
	case Stream::End:
		moveMethod = FILE_END;
		break;
	}
	SetFilePointer(handle, offset, NULL, moveMethod);
}

//------------------------------------------------------------------------------
bool FileStream::Eof() const
{
	Ast(INVALID_HANDLE_VALUE != this->handle);

	DWORD fpos = SetFilePointer(this->handle, 0, NULL, FILE_CURRENT);
	DWORD size = GetFileSize(this->handle, NULL);

	return fpos >= size;
}

//------------------------------------------------------------------------------
void*
FileStream::Map()
{
	Stream::Map();

	this->mappedData = malloc(this->GetSize());

	index_t originPos = this->GetPosition();
	this->Seek(0, Stream::Begin);

	this->Read(this->mappedData, this->GetSize());

	this->Seek(originPos, Stream::Begin);

	return this->mappedData;
}

//------------------------------------------------------------------------------
void
FileStream::UnMap()
{
	free(this->mappedData);
	//Del(this->mappedData);
	this->mappedData = NULL;

	Stream::UnMap();
}

}// namespace sqr
