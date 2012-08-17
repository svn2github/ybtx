#pragma once
#include "Base.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#pragma warning(disable:4996)
#pragma warning(disable:4267)

#include <set>
#include <iostream>
#include <fstream>
#include <strstream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <algorithm>
#include <cstring>
#include <memory>
using namespace std;

#if defined(_WIN32) || defined(_USE_STLPORT)
#include <hash_map>
using namespace stdext;
#else
#include <ext/hash_map>
using namespace __gnu_cxx;
#endif
using namespace sqr;


// TODO: 在此处引用程序要求的附加头文件
