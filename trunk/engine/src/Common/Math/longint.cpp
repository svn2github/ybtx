#include "stdafx.h"
#include "longint.h"
#include <cstring>
#include <cstdlib>
#include <cmath>
#include "TSqrAllocator.inl"

void longint::swap(longint*a,longint*b)
{     //交换函数，用于快速赋值
	unsigned temp=a->length;
	a->length=b->length;
	b->length=temp;
	unsigned * temp2=a->number;
	a->number=b->number;
	b->number=temp2;
	int temp3=a->sign;
	a->sign=b->sign;
	b->sign=temp3;
}

void longint::clear()
{
	memset(number,0,length*sizeof(unsigned));
	sign=0;
	length=1;
}

longint::~longint()
{
	delete[]number;
}

longint::longint()
{
	number=new unsigned [1];
	number[0]=0;
	sign=0;
	length=1;
}

longint::longint(const char*input)
{
	unsigned i;
	size_t inputlen=strlen(input);
	char*copy=new char[inputlen+1];
	strcpy(copy,input);
	char*temp=copy;
	sign=0;
	if(temp[0]=='-'||temp[0]=='+')
	{
		if(temp[0]=='-')sign=1;
		temp++;
		inputlen--;
	}
	for(i=0;i<inputlen;i++)
	{
		if(temp[i]<'0'||temp[i]>'9')
		{
			length=1;
			number=new unsigned [length];
			number[0]=0;
			delete[]copy;
			return;
		}
		temp[i]-='0';
	}
	length=(unsigned)(((unsigned(inputlen*log(float(10))/log(float(2))))>>5)+1);
	number=new unsigned [length];
	memset(number,0,length*sizeof(unsigned));
	unsigned  fill=1,jiewei=0,fillwhere=length-1;
	while(temp[0]==0&&inputlen>1){temp++;inputlen--;}
	while(inputlen)
	{
		for(i=0;i<inputlen;i++)
		{
			unsigned t=temp[i]&1;
			temp[i]=char( (temp[i]+10*jiewei)>>1 );
			jiewei=t;
		}
		if(temp[0]==0){temp++;inputlen--;}
		if(jiewei)number[fillwhere]|=fill;
		fill=fill<<1;
		jiewei=0;
		if(fill==0){fillwhere--;fill=1;}
	}
	cutzero();
	delete[]copy;
}
void longint::cutzero()
{
	unsigned j=0;
	while(number[j]==0&&length>1){j++;length--;}
	if(j)
	{
		memmove(number,number+j,length*sizeof(unsigned));
		/*for(i=0;i<length;i++)
		number[i]=number[i+j];*/
	}
	return;
}
longint::longint(const longint&other)
{
	length=other.length;
	number=new unsigned [length];
	memcpy(number,other.number,length*sizeof(unsigned));
	sign=other.sign;
}
const longint& longint::operator =(const longint&other)
{
	length=other.length;
	delete[]number;
	number=new unsigned [length];
	memcpy(number,other.number,length*sizeof(unsigned));
	sign=other.sign;
	return *this;
}

longint longint::operator <<(const unsigned a)const
{
	longint answer=*this;
	answer.shiftleft(a);
	return answer;
}
longint longint::operator >>(const unsigned a)const
{
	longint answer=*this;
	answer.shiftright(a);
	return answer;
}
longint longint::shiftright(const unsigned a)
{
	unsigned temp1=a>>5,temp2=a&0x1f;
	int t=length-temp1;
	if(t<=0){longint zero;return zero;}
	else length=t;
	if(!temp2)return *this;
	temp1=number[0];
	number[0]=number[0]>>temp2;
	for(unsigned i=1;i<length;i++)
	{
		unsigned temp3=number[i];
		number[i]=number[i]>>temp2|temp1<<(32-temp2);
		temp1=temp3;
	}
	cutzero();
	return *this;
}

longint longint::shiftleft(const unsigned a)
{
	unsigned temp1=a>>5,temp2=a&0x1f,i,key=0;
	if(int(log(float(number[0]))/log(2.0))+temp2>=32){temp1++;key=1;}
	if(temp1)
	{
		unsigned *t=new unsigned [length+temp1];
		for(i=key;i<length+key;i++)t[i]=number[i-key];
		if(key)t[0]=0;
		for(i=length+key;i<length+temp1;i++)t[i]=0;
		delete[]number;
		length+=temp1;
		number=new unsigned [length];
		if(temp2)
		{
			for(i=0;i<length-temp1;i++)
			{
				number[i]=t[i]<<temp2;
				number[i]|=t[i+1]>>(32-temp2);
			}
			number[length-temp1]=t[length-temp1]<<temp2;
			for(i=length-temp1+1;i<length;i++)
				number[i]=0;
		}
		else
		{
			memcpy(number,t,length*sizeof(unsigned));
		}
		delete[]t;
	}
	else
	{
		if(number[0]&0x80000000)
		{
			unsigned *t=new unsigned [length+1];
			memcpy(t+1,number,length*sizeof(unsigned));
			//for(i=0;i<length;i++)t[i+1]=number[i];
			t[0]=0;
			delete[]number;
			length++;
			number=new unsigned [length];
			memcpy(number,t,length*sizeof(unsigned));
			/*for(i=0;i<length;i++)
			number[i]=t[i];*/
			delete[]t;
		}
		for(i=0;i<length-1;i++)
		{
			number[i]=number[i]<<temp2;
			number[i]|=number[i+1]>>(32-temp2);
		}
		number[length-1]=number[length-1]<<temp2;
	}
	return *this;
}

longint longint::operator -()const
{
	if(iszero())return *this;
	longint answer=*this;
	answer.sign=(sign+1)%2;
	return answer;
}
longint longint::operator --(const int)
{
	if(sign)
	{
		sign=(sign+1)&1;
		(*this)++;
		sign=(sign+1)&1;
		return *this;
	}
	if(iszero())
	{
		sign=1;
		number[0]=1;
		return *this;
	}
	unsigned i=length-1;
	while(number[i]==0)
	{
		number[i]--;
		i--;
	}
	number[i]--;
	cutzero();
	return *this;
}
longint longint::operator --()
{
	return (*this)--;
}
longint longint::operator ++()
{
	return (*this)++;
}
longint longint::operator ++(const int)
{
	if(sign)
	{
		sign=(sign+1)&1;
		(*this)--;
		sign=(sign+1)&1;
		if(iszero())sign=0;
		return *this;
	}
	number[length-1]++;
	unsigned i=length-1;
	while(number[i]==0&&i>0)
	{
		number[i-1]++;
		i--;
	}
	if(number[0]==0)
	{
		length++;
		delete[]number;
		number=new unsigned [length];
		number[0]=1;
		memset(number,0,length*sizeof(unsigned));
	}
	return *this;
}
bool longint::operator ==(const longint&other)const
{
	if(length!=other.length)return 0;
	if(sign!=other.sign)return 0;
	return memcmp(number,other.number,length*sizeof(unsigned))==0;
	/*for(unsigned i=0;i<length;i++)
	if(number[i]!=other.number[i])return 0;
	return 1;*/
}
bool longint::operator >(const longint&other)const
{
	if(sign==0&&other.sign==1)return 1;
	if(sign==1&&other.sign==0)return 0;
	if(sign==1&&other.sign==1)return (-*this<-other);
	if(length>other.length)return 1;
	if(length<other.length)return 0;
	return memcmp(number,other.number,length*sizeof(unsigned))==1;
	/*for(unsigned i=0;i<length;i++)
	{
	if(number[i]>other.number[i])return 1;
	if(number[i]<other.number[i])return 0;
	}
	return 0;*/
}
bool longint::operator >=(const longint&other)const
{
	if(sign==0&&other.sign==1)return 1;
	if(sign==1&&other.sign==0)return 0;
	if(sign==1&&other.sign==1)return (-*this<-other);
	if(length>other.length)return 1;
	if(length<other.length)return 0;
	return memcmp(number,other.number,length*sizeof(unsigned))!=-1;

	//return(*this>other||*this==other);
}
bool longint::operator <(const longint&other)const
{
	if(sign==1&&other.sign==0)return 1;
	if(sign==0&&other.sign==1)return 0;
	if(sign==1&&other.sign==1)return (-*this<-other);
	if(length>other.length)return 0;
	if(length<other.length)return 1;
	return memcmp(number,other.number,length*sizeof(unsigned))==-1;
	/*for(unsigned i=0;i<length;i++)
	{
	if(number[i]>other.number[i])return 0;
	if(number[i]<other.number[i])return 1;
	}
	return 0;*/
}
bool longint::operator <=(const longint&other)const
{
	if(sign==1&&other.sign==0)return 1;
	if(sign==0&&other.sign==1)return 0;
	if(sign==1&&other.sign==1)return (-*this<-other);
	if(length>other.length)return 0;
	if(length<other.length)return 1;
	return memcmp(number,other.number,length*sizeof(unsigned))!=-1;
	/*return(!(*this>other));*/
}
bool longint::operator !=(const longint&other)const
{
	return(!(*this==other));
}
longint::longint(unsigned a)
{
	sign=0;
	number=new unsigned [a];
	memset(number,0,a*sizeof(unsigned));
	/*for(unsigned i=0;i<a;i++)number[i]=0;*/
	length=a;
}
longint longint::operator +(const longint&other)const
{
	if(sign==1&&other.sign==1)return -(-*this+(-other));
	if(sign==0&&other.sign==1)return *this-(-other);
	if(sign==1&&other.sign==0)return other-(-*this);
	unsigned i,ttt=0;
	const longint*longer,*shorter;
	if(length>other.length)
	{
		longer=this;shorter=&other;
	}
	else
	{
		longer=&other;shorter=this;
	}
	longint answer(longer->length+1);
	for(i=0;i<shorter->length;i++)
	{
		answer.number[answer.length-i-1]=number[length-i-1]+other.number[other.length-i-1];
		if(answer.number[answer.length-i-1]<number[length-i-1]||answer.number[answer.length-i-1]<other.number[other.length-i-1])
		{
			answer.number[answer.length-i-1]+=ttt;
			ttt=1;
		}
		else
		{
			answer.number[answer.length-i-1]+=ttt;
			ttt=(answer.number[answer.length-i-1]==0&&ttt);
		}
	}
	for(i=shorter->length;i<longer->length;i++)
	{
		answer.number[answer.length-i-1]=longer->number[longer->length-i-1]+ttt;
		ttt=(answer.number[answer.length-i-1]==0&&ttt);
	}
	answer.number[0]=ttt;
	answer.cutzero();
	return answer;
}
const longint& longint::operator -=(const longint&other)
{
	//    if(*this<other)throw(0);
	unsigned i,j=0,jj;
	for(i=0;i<other.length;i++)
	{
		if(number[length-i-1]<other.number[other.length-i-1]+j||(j&&other.number[other.length-i-1]==0xffffffff))jj=1;else jj=0;
		number[length-i-1]=number[length-i-1]-other.number[other.length-i-1]-j;
		j=jj;
	}
	for(i=other.length;i<length;i++)
	{
		if(number[length-i-1]<j)jj=1;else jj=0;
		number[length-i-1]=number[length-i-1]-j;
		if(!jj)break;
		j=jj;
	}
	cutzero();
	return *this;
}
longint longint::operator -(const longint&other)const
{
	if(sign==1&&other.sign==1)return -(-*this-(-other));
	if(sign==0&&other.sign==1)return *this+(-other);
	if(sign==1&&other.sign==0)return -(other+(-*this));
	if(*this<other)return -(other-*this);
	longint answer=*this;
	answer-=other;
	return answer;
}

longint longint::operator *(const longint&other)const
{
	unsigned i,j;
	unsigned temp1,temp2;
	longint answer(length+other.length);
	for(i=0;i<length;i++)
	{
		for(j=0;j<other.length;j++)
		{
			temp1=number[length-i-1];
			temp2=other.number[other.length-j-1];
			if(!temp1||!temp2)continue;
			__int64 wokao=(__int64)temp1*(__int64)temp2;
			temp1=(unsigned)(wokao>>32);
			temp2=(unsigned)(wokao&0xffffffff);
			unsigned k=answer.length-i-j-1;
			answer.number[k]+=temp2;
			temp1+=answer.number[k]<temp2;
			k--;
			while(temp1>0)
			{
				answer.number[k]+=temp1;
				temp1=(answer.number[k]<temp1);
				k--;
			}
		}
	}
	answer.sign=(sign+other.sign)&1;
	answer.cutzero();
	if(answer.iszero())answer.sign=0;
	return answer;
}

longint longint::operator /(const longint&other)const
{
	if(*this<other)
	{
		longint temp;
		return temp;
	}
	longint temp1=*this,temp2=other,answer(temp1.length-temp2.length+1);
	unsigned len=((length-other.length)<<5)+int(log(float(number[0]))/log(2.0f))-int(log(float(other.number[0]))/log(2.0f))+1;
	temp2.shiftleft(len);
	answer.length=0;
	unsigned mask=1<<((len&0x1f)-1);
	for(unsigned i=0;i<len;i++)
	{
		temp2.shiftright(1);
		if(!(temp1<temp2))
		{
			temp1-=temp2;
			answer.number[answer.length]|=mask;
		}
		mask=mask>>1;
		if(!mask){mask=unsigned(1)<<31;answer.length++;}
	}
	answer.cutzero();
	return answer;
}
longint longint::operator %(const longint&other)const
{
	if(*this<other)return *this;
	longint temp1=*this,temp2=other;
	unsigned len=((length-other.length)<<5)+int(log(float(number[0]))/log(2.0f))-int(log(float(other.number[0]))/log(2.0f))+1;
	temp2.shiftleft(len);
	for(unsigned i=0;i<len;i++)
	{
		temp2.shiftright(1);
		if(!(temp1<temp2))temp1-=temp2;
	}
	return temp1;
}
const longint& longint::operator %=(const longint &other)
{
	if(*this<other)
		return *this;

	longint temp2=other;
	unsigned len=((length-other.length)<<5)+int(log(float(number[0]))/log(2.0f))-int(log(float(other.number[0]))/log(2.0f))+1;
	temp2.shiftleft(len);
	for(unsigned i=0;i<len;i++)
	{
		temp2.shiftright(1);
		if(!(*this<temp2))*this-=temp2;
	}
	return *this;
}

longint longint::rand(unsigned a)
{
	delete[]number;
	sign=0;
	unsigned i;
	length=((unsigned(a*log(10.0f)/log(2.0f)))>>5)+1;
	number=new unsigned [length];
	a-=(unsigned(((length-1)<<5)*log(2.0f)/log(10.0f)));
	if(a>9)
	{
		number[0]=unsigned((((__int64)::rand()<<15)|((__int64)::rand()<<30)|(__int64)::rand())%0xffffffff);
	}
	else
	{
		unsigned temp=1;
		if(a==1)temp=5;
		else for(i=0;i<a;i++)temp*=10;
		number[0]=::rand()%temp+1;
	}
	for(i=1;i<length;i++)
		number[i]=unsigned((((__int64)::rand()<<15)|((__int64)::rand()<<30)|(__int64)::rand())%0xffffffff);
	number[length-1]|=1;
	return *this;
}
bool longint::iseven()const
{
	return(!(number[length-1]&1));
}
bool longint::isodd()const
{
	return(number[length-1]&1);
}
bool longint::isone()const
{
	if(length==1&&number[0]==1)return 1;
	return 0;
}
bool longint::iszero()const
{
	if(length==1&&number[0]==0)return 1;
	return 0;
}
bool longint::ispositive()const
{
	if(sign==0&&!iszero())return 1;
	return 0;
}
longint longint::abs()const
{
	longint answer=*this;
	answer.sign=0;
	return answer;
}

namespace sqr
{
	class monty // class for montgomery modular exponentiation
	{
		longint r,n,n1,M1,x1;
		longint T,k;   // work registers
		unsigned N;  // bits for R
		unsigned move;
		void mul(longint &x, const longint &y );
		longint mulmod(const longint&left,const longint&right)const;
	public:
		longint exp(const longint &x,const longint&e );
		monty(const longint&mod );
	};
}

monty::monty(const longint &mod )
{
	n = mod;
	unsigned t=n.number[0];
	N=(n.length-1)<<5;
	while(t){t=t>>1;N++;}
	r++;
	r.shiftleft(N);

	longint temp=(r*r.inverse(n));
	longint::swap(&n1,&temp);
	n1--;
	move=0xffffffff>>(32-N&0x1f);
	temp=(n1/n);
	longint::swap(&n1,&temp);
	temp=(r-n);
	longint::swap(&x1,&temp);
}
longint monty::mulmod(const longint &left,const longint&right)const
{
	unsigned i,j;
	unsigned temp1,temp2;
	longint answer(((N-1)>>5)+1);
	for(i=0;i<answer.length;i++)
	{
		for(j=0;j<answer.length-i;j++)
		{
			if(i+1>left.length||j+1>right.length)
				continue;
			temp1=left.number[left.length-i-1];
			temp2=right.number[right.length-j-1];
			if(!temp1||!temp2)continue;
			__int64 wokao=(__int64)temp1*(__int64)temp2;
			temp1=(unsigned)(wokao>>32);
			temp2=(unsigned)(wokao&0xffffffff);
			int k=answer.length-i-j-1;
			answer.number[k]+=temp2;
			temp1+=answer.number[k]<temp2;
			k--;
			while(temp1>0&&k>=0)
			{
				answer.number[k]+=temp1;
				temp1=(answer.number[k]<temp1);
				k--;
			}
		}
	}

	answer.cutzero();
	if((answer.length<<5)>N)
		answer.number[0]&=move;
	answer.cutzero();
	return answer;
}

void monty::mul( longint &x, const longint &y )
{
	longint temp=(x*y);
	longint::swap(&T,&temp);
	temp=(mulmod(T,n1));
	longint::swap(&k,&temp);
	temp=(k*n+T);
	longint::swap(&x,&temp);
	x.shiftright(N);
	if (x>=n)x-=n;
}
longint monty::exp(const longint&x,const longint &e )
{
	longint temp=(x*r%n);
	longint::swap(&M1,&temp);
	unsigned mask,i=e.number[0]>>1;
	mask=1;
	while(i){i=i>>1;mask=mask<<1;}
	for(i=0;i<e.length;i++)
	{
		while(mask)
		{
			mul(x1,x1);
			if(e.number[i]&mask)
				mul(x1, M1);
			mask=mask>>1;
		}
		mask=unsigned(1)<<31;
	}
	longint answer;
	answer++;
	mul(answer,x1);
	return answer;
}

longint longint::powermod(const longint&b,const longint&c)const
{
	monty a(c);
	return a.exp(*this,b);
}
bool longint::isprime(const unsigned&time)const
{
	longint n=*this;
	longint m=*this;
	m--;
	longint nn=m;
	m.shiftright(1);
	longint l;
	l++;
	while(m.iseven()){m.shiftright(1);l++;}
	longint b;
	b++;
	longint temp;
	for(unsigned i=0;i<time;i++)
	{
		b++;
		longint v;
		temp=b.powermod(m,n);
		swap(&v,&temp);
		if(v.isone())continue;
		longint ii;
		ii++;
		for(;;)
		{
			if(v==nn)break;
			if(ii==l)
				return 0;
			temp=(v*v%n);
			swap(&v,&temp);
			ii++;
		}
	}
	return 1;
}

longint longint::inverse(const longint&mod)const
{
	longint n2=*this,n1=mod,b1,b2;
	b2++;
	longint q,r,temp;
	temp=(n1/n2);
	swap(&q,&temp);
	temp=(n1-q*n2);
	swap(&r,&temp);
	while(!r.iszero())
	{
		swap(&n1,&n2);
		swap(&n2,&r);
		longint t=b2;
		temp=(b1-q*b2);
		swap(&b2,&temp);
		swap(&b1,&t);
		temp=(n1/n2);
		swap(&q,&temp);
		temp=(n1-q*n2);
		swap(&r,&temp);
	}
	temp=(b2+mod);
	if(b2.sign)swap(&b2,&temp);
	return b2;
}
longint longint::sqrt()const
{
	longint answer(length/2+1);
	answer.number[0]=1;
	longint temp,temp1=answer,swaptemp;
	temp1++;
	swaptemp=((answer+*this/answer)>>1);
	swap(&temp,&swaptemp);
	while(temp!=answer&&!(answer*answer<=*this&&temp1*temp1>*this))
	{
		swap(&answer,&temp);
		temp1=answer;
		temp1++;
		swaptemp=((answer+*this/answer)>>1);
		swap(&temp,&swaptemp);
	}
	return answer;
}
