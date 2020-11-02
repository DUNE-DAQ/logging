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

void foo( int except_int )
{
	switch( except_int ) {
	case 0:
		ers::PermissionDenied permdenied(ERS_HERE, "somefilename", 0644 );
		permdenied.raise();
		break;
	}
}

int main(  int	argc, char	*argv[] )
{
	if (argc != 2) { printf(USAGE); exit(1); }
	try
    {
        foo( strtoul(argv[1],NULL,0) );
    }
    catch ( ers::PermissionDenied & ex )
    {
        ers::CantOpenFile issue( ERS_HERE, ex.get_file_name(), ex );
        ers::warning( issue );
    }
    catch ( ers::FileDoesNotExist & ex )
    {
        ers::CantOpenFile issue( ERS_HERE, ex.get_file_name(), ex );
        ers::warning( issue );
    }
    catch ( ers::Issue & ex )
    {
        ERS_DEBUG( 0, "Unknown issue caught: " << ex );
        ers::error( ex );
    }
    catch ( std::exception & ex )
    {
        ers::CantOpenFile issue( ERS_HERE, "unknown", ex );
        ers::warning( issue );
    }
	return (0);
}   // main
