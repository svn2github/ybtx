#pragma once 
#include "CommonDefs.h"
#include "BaseTypes.h"

struct short_num_struct{
	uint8 first_byte;
	uint8 second_byte;
	uint8 third_byte;
	uint8 fourth_byte;
};

union short_num_union{
	uint16 num;
	short_num_struct s;
};

struct long_num_struct{
	uint8 first_byte;
	uint8 second_byte;
	uint8 third_byte;
	uint8 fourth_byte;
};

union long_num_union{
	int32 num;
	long_num_struct s;
};

struct long_long_num_struct{
	long_num_union first;
	long_num_union second;
};

union long_long_num_uion{
	uint64 num;
	long_long_num_struct s;
};

uint16 ChangeByteOrder_uint16(uint16 num)
{
	short_num_union u;
	u.num = num;
	return (uint16)((u.s.first_byte << 8) + u.s.second_byte);
}

int16 ChangeByteOrder_int16(int16 num)
{
	short_num_union u;
	u.num = num;
	return (int16)((u.s.first_byte << 8) + u.s.second_byte);
}

uint32 ChangeByteOrder_uint32(uint32 num)
{
	long_num_union u;
	u.num = num;
	return (uint32)((u.s.first_byte << 24) + (u.s.second_byte << 16) + (u.s.third_byte << 8) + u.s.fourth_byte);
}

int32 ChangeByteOrder_int32(int32 num)
{
	long_num_union u;
	u.num = num;
	return (int32)((u.s.first_byte << 24) + (u.s.second_byte << 16) + (u.s.third_byte << 8) + u.s.fourth_byte);
}

uint64 ChangeByteOrder_uint64(uint64 num)
{
	long_long_num_uion u;
	u.num = num;
	uint32 first	= ChangeByteOrder_uint32(u.s.second.num);
	uint32 second	= ChangeByteOrder_uint32(u.s.first.num);
	u.s.first.num	= first;
	u.s.second.num	= second;
	return (uint64)(u.num);
}

