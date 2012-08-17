#include "stdafx.h"
#include "CheckPkgFlow.h"

int CheckParameter( int argc, char* argv[] )
{
	if ( argc < 3 )
	{
		cerr<<"usage:"<<argv[0]<<" SrcPath DesPath [options1] [options2]...\n";
		cerr<<"Available options are:\n";
		cerr<<"  -d dir							process director\n";
		cerr<<"  -t type						exclude file type\n";
		cerr<<"  -f name						exclude file name\n";
		cerr<<"  -u ver svrlst updturl ntfile	create update.ini file\n";
		return EXIT_FAILURE;
	};
	return EXIT_SUCCESS;
}

int main( int argc, char* argv[] )
{
	if ( CheckParameter( argc, argv ) == EXIT_FAILURE )
		return EXIT_SUCCESS;		// 参数不满足条件

	EnableCatchError(true);
	setlocale(LC_CTYPE, "");

	CheckPkgFlow	PkgFlow;
	PkgFlow.ProcessFlow( argc, argv );

	return EXIT_SUCCESS;
};

