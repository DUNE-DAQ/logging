 // This file (exception_example.cxx) was created by Ron Rechenmacher <ron@fnal.gov> on
 // Oct 29, 2020. "TERMS AND CONDITIONS" governing this file are in the README
 // or COPYING file. If you do not have such a file, one can be obtained by
 // contacting Ron or Fermi Lab in Batavia IL, 60510, phone: 630-840-3000.
 // $RCSfile: .emacs.gnu,v $
 // rev="$Revision: 1.34 $$Date: 2019/04/22 15:23:54 $";
/*
    See https://atlas-tdaq-monitoring.web.cern.ch/OH/refman/ERSHowTo.html
 */
#define USAGE "\
 Usage: %s <num>\n\
where <num> is a number currently 0.\n\
", basename(argv[0])

#include <libgen.h>				// basename
#include <ers/SampleIssues.h>
#include <logging/Logger.hpp>

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
	}
}

int main(  int	argc, char	*argv[] )
{
	if (argc != 2) { printf(USAGE); exit(1); }

	std::vector<std::string> arguments(argv + 1, argv + argc);
	Logger().setup(arguments);

	TLOG_DEBUG(1) << "trying foo";
	try {
        foo( strtoul(argv[1],NULL,0) );
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
    }/* catch (...) {
		ers::fatal( ers::Message(ERS_HERE,"unhandle exceptions would not make it to the the TRACE memory buffer") );
		// ErrorHandler::abort(...) does StandardStreamOutput::println(std::cerr, issue, 13); ::abort();
		}*/
	
	return (0);
}   // main
