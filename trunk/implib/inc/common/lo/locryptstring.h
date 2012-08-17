#ifndef __lo_CRYPTSTRING_H__
#define __lo_CRYPTSTRING_H__

#include <string>
#include <common/lo/lodefine.h>
#include <common/lo/linked_ptr.h>
#include <common/lo/scoped_ptr.h>

DEFINE_NAMESPACE(locom)

class CloCryptStringImpl
{
public:
	virtual ~CloCryptStringImpl() {}
	virtual size_t GetLength() const = 0;
	virtual void CopyTo(char * dest, bool nullterminate) const = 0;
	virtual std::string UrlEncode() const = 0;
	virtual CloCryptStringImpl * Copy() const = 0;
};

class CloEmptyCryptStringImpl : public CloCryptStringImpl
{
public:
	virtual ~CloEmptyCryptStringImpl()
	{}
	virtual size_t GetLength() const { return 0; }
	virtual void CopyTo(char * dest, bool nullterminate) const
	{
		if (nullterminate)
		{
			*dest = '\0';
		}
	}
	virtual std::string UrlEncode() const { return ""; }
	virtual CloCryptStringImpl * Copy() const { return new CloEmptyCryptStringImpl(); }
};

class CloCryptString
{
public:
	CloCryptString()
		: impl_(new CloEmptyCryptStringImpl())
	{}
	CloCryptString(const CloCryptString & other) 
		: impl_(other.impl_->Copy())
	{}
	explicit CloCryptString(const CloCryptStringImpl & impl) 
		: impl_(impl.Copy())
	{}
	CloCryptString & operator=(const CloCryptString & other)
	{
		if (this != &other)
		{
			impl_.reset(other.impl_->Copy());
		}
		return *this;
	}
public:
	size_t GetLength() const { return impl_->GetLength(); }

	void CopyTo(char * dest, bool nullterminate) const
	{ impl_->CopyTo(dest, nullterminate); }

	void Clear() { impl_.reset(new CloEmptyCryptStringImpl()); }

	std::string UrlEncode() const { return impl_->UrlEncode(); }

private:
	scoped_ptr<const CloCryptStringImpl> impl_;
};


// Used for constructing strings where a password is involved and we
// need to ensure that we zero memory afterwards
class CloFormatCryptString
{
public:
	CloFormatCryptString()
	{
		storage_ = new char[32];
		capacity_ = 32;
		length_ = 0;
		storage_[0] = 0;
	}

	void Append(const std::string & text)
	{
		Append(text.data(), text.length());
	}

	void Append(const char * data, size_t length)
	{
		EnsureStorage(length_ + length + 1);
		memcpy(storage_ + length_, data, length);
		length_ += length;
		storage_[length_] = '\0';
	}

	void Append(const CloCryptString * password) 
	{
		size_t len = password->GetLength();
		EnsureStorage(length_ + len + 1);
		password->CopyTo(storage_ + length_, true);
		length_ += len;
	}

	size_t GetLength()
	{
		return length_;
	}

	const char * GetData()
	{
		return storage_;
	}


	// Ensures storage of at least n bytes
	void EnsureStorage(size_t n)
	{
		if (capacity_ >= n) {
			return;
		}

		size_t old_capacity = capacity_;
		char * old_storage = storage_;

		for (;;) 
		{
			capacity_ *= 2;
			if (capacity_ >= n)
				break;
		}

		storage_ = new char[capacity_];

		if (old_capacity)
		{
			memcpy(storage_, old_storage, length_);

			// zero memory in a way that an optimizer won't optimize it out
			old_storage[0] = 0;
			for (size_t i = 1; i < old_capacity; i++)
			{
				old_storage[i] = old_storage[i - 1];
			}
			delete[] old_storage;
		}
	}  

	~CloFormatCryptString()
	{
		if (capacity_)
		{
			storage_[0] = 0;
			for (size_t i = 1; i < capacity_; i++)
			{
				storage_[i] = storage_[i - 1];
			}
		}
		delete[] storage_;
	}
private:
	char * storage_;
	size_t capacity_;
	size_t length_;
};

class CloInsecureCryptStringImpl : public CloCryptStringImpl 
{
public:
	virtual ~CloInsecureCryptStringImpl() {}

public:
	std::string& password() { return password_; }
	const std::string& password() const { return password_; }

	virtual size_t GetLength() const { return password_.size(); }
	virtual void CopyTo(char * dest, bool nullterminate) const
	{
		memcpy(dest, password_.data(), password_.size());
		if (nullterminate) dest[password_.size()] = 0;
	}
	virtual std::string UrlEncode() const { return password_; }
	virtual CloCryptStringImpl * Copy() const
	{
		CloInsecureCryptStringImpl * copy = new CloInsecureCryptStringImpl;
		copy->password() = password_;
		return copy;
	}
private:
	std::string password_;
};

END_NAMESPACE(locom)

#endif  // __lo_CRYPTSTRING_H__
