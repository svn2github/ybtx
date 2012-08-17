#pragma once 

enum {
	eLua_CPP_Split_ID = 32767       //用来区分lua的rpc与c++的rpc，在lua中id小于这个值，而c++中则大于
};

#define RPC_CHECK_ID   32767         //这个宏用来区分lua的rpc与c++的rpc，在lua中id小于这个值，而c++中则大于

