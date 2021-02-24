/**
 * @file exception_example.cxx logger interface definition
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

//  See https://atlas-tdaq-monitoring.web.cern.ch/OH/refman/ERSHowTo.html

const char *usage = R"foo(
 Usage: %s <num>
where <num> is a number: 0 - 6.
)foo""\n";
#define USAGE usage, basename(*argv)

#include <libgen.h>				// basename
#include <ers/SampleIssues.h>
#include <logging/Logging.hpp>
#include <string>

class XX
{
	~XX() { TLOG_DEBUG(1) << "dtor object after raise/throw"; }
};

void foo( int except_int )
{
	XX xx();
	switch( except_int ) {
	case 0: {
		TLOG_DEBUG(1) << "raising ers::PermissionDenied " << "somefilename";
		ers::PermissionDenied permdenied(ERS_HERE, "somefilename", 0644 );
		permdenied.raise();
	}
		break;
	case 1: {
		TLOG_DEBUG(1) << "raising ers::FileDoesNotExist " << "somenonfile";
		ers::FileDoesNotExist doesnotexist(ERS_HERE, "somenonfilename" );
		doesnotexist.raise();
	}
		break;
	case 2: {
		TLOG_DEBUG(1) << "raising ers::CantOpenFile " << "somelockedfile";
		ers::CantOpenFile( ERS_HERE, "somelockedfile" ).raise();
	}
		break;
	case 3:
		TLOG_DEBUG(1) << "throwing std::exception";
		throw std::exception();
		break;
	case 4:
		TLOG_DEBUG(1) << "throwing integer 4";
		throw 4;
		break;
	case 5: {
		TLOG_DEBUG(1) << "write to address 0 - cause segv";
		int *ptr=nullptr;
		*ptr = 0;
	}
		break;
	default:
		TLOG_DEBUG(1) << "not throwing anything";
	}
}

int main(  int	argc, char	*argv[] )
{
	if (argc != 2) { printf(USAGE); exit(1); }

	Logging().setup();

	TLOG_DEBUG(1) << "trying foo";
	try {
        foo( strtoul(argv[1],nullptr,0) );
    } catch ( ers::PermissionDenied & ex ) {
        ers::CantOpenFile issue( ERS_HERE, ex.get_file_name(), ex );
        ers::warning( issue );
    } catch ( ers::FileDoesNotExist & ex ) {
        ers::CantOpenFile issue( ERS_HERE, ex.get_file_name(), ex );
        ers::warning( issue );
    } catch ( ers::Issue & ex ) {
        TLOG_DEBUG( 0 ) << "Unknown issue caught: " << ex;
        ers::error( ex );
    } catch ( std::exception & ex ) {
        ers::CantOpenFile issue( ERS_HERE, "unknown", ex );
        ers::warning( issue );
    }
#	if 0
	catch (...) {
		TLOG() << "unhandle exceptions would not make it to the the TRACE memory buffer";
		// ErrorHandler::abort(...) does StandardStreamOutput::println(std::cerr, issue, 13); ::abort();
	}
#	endif
	
	return (0);
}   // main
