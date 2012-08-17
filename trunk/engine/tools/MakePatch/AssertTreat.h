#pragma once

#ifdef _DEBUG
#define  AstFile(p, name)                     \
	if (!p) {                                 \
		printf("Can't open file: %s", name);  \
		exit(1);                              \
	}

#else
#define  AstFile(p, name)

#endif

