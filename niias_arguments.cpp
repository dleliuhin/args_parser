#include "niias_arguments.h"

#include <iostream>
#include <cassert>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "vlog.h"
#include "vapplication.h"
#include "impl_vposix/wrap_fcntl.h"
#include "impl_vposix/wrap_unistd.h"
#include "impl_vposix/wrap_errno.h"
#include "impl_vposix/wrap_signal.h"
#include "impl_vposix/safe_fd.h"

using namespace niias;
using namespace impl_vposix;

//=======================================================================================
class arguments::_pimpl
{
public:
    using str = std::string;
    //-----------------------------------------------------------------------------------
    static void print_phony_help()
    {
        std::cout << "\n======== NIIAS service =======================================\n"
                  << " -h, --help                           -- print this help;\n"
                  << " -v, --vgit-*                         -- print git fixes;\n"
                  << " -c, config=, --config=               -- set config filename;\n"
                  << " -p, pid=,    --pid=                  -- set pid filename;\n"
                  << "=============================================================="
                  << std::endl;
    }
    //-----------------------------------------------------------------------------------
    void print_help_and_exit( int retcode )
    {
        help();
        exit( retcode );
    }
    //-----------------------------------------------------------------------------------
    _pimpl( int argc, const char * const * const argv )
        : args(argc,argv)
    {}
    //-----------------------------------------------------------------------------------
    ~_pimpl()
    {
        unlock_pid();
    }
    //-----------------------------------------------------------------------------------
    bool find_help()
    {
        if ( args.take("-h") )
            return true;

        return args.take( "--help" );
    }
    //-----------------------------------------------------------------------------------
    bool find_config()
    {
        conf_name = find_smth( "-c", "-c", "config=", "--config=" );
        return !conf_name.empty();
    }
    //-----------------------------------------------------------------------------------
    bool find_pid()
    {
        pid_name = find_smth( "-p", "-p", "pid=", "--pid=" );
        return !pid_name.empty();
    }
    //-----------------------------------------------------------------------------------
    str find_smth( str next1, str start1, str start2, str start3 )
    {
        auto res = args.safe_next( next1, {} );
        if ( !res.empty() ) return res;

        res = args.safe_starts_with( start1, {} );
        if ( !res.empty() ) return res;

        res = args.safe_starts_with( start2, {} );
        if ( !res.empty() ) return res;

        res = args.safe_starts_with( start3, {} );
        return res;
    }
    //-----------------------------------------------------------------------------------
    void check_remained_args()
    {
        auto unused = args.unused();
        if ( unused.empty() ) return;

        vwarning << "Found unused arguments:" << unused;
        print_help_and_exit(1);
    }
    //-----------------------------------------------------------------------------------
    bool lock_pid()
    {
        assert( !pid_name.empty() );
        pid_fd = wrap_fcntl::open( pid_name.c_str(), O_CREAT|O_RDWR|O_LARGEFILE, 0644 );

        auto blocked = wrap_unistd::lockf_test_lock_no_err( pid_fd );
        if (blocked == 0)
        {
            wrap_unistd::write(pid_fd, vcat().aligned(vapplication::pid(),10,' ')('\n'));
            return true;
        }
        ErrNo err;
        if ( !err.resource_unavailable_try_again() )
            err.do_throw( vcat("Unexpected error during locking PID file '",
                               pid_name,"'") );
        //-------------------------------------------------------------------------------
        // Need to search process with pid in file.
        // 1. read the pid:
        auto str_pid = wrap_unistd::read( pid_fd );
        pid_t other_pid = -1;
        try
        {
            other_pid = vcat::from_text<pid_t>( str_pid );
        }
        catch ( const std::exception& e )
        {
            throw verror << "Cannot lock and check PID lockfile '" << pid_name
                         << "', err: " << e.what();
        }

        if ( wrap_signal::has_process(other_pid) )
        {
            throw verror( "Process, locked file '",pid_name,"' with PID ", other_pid,
                          " is active, cannot lock.");
        }

        wrap_unistd::lockf_unlock_no_err( pid_fd );
        return false;
    }
    //-----------------------------------------------------------------------------------
    void unlock_pid()
    {
        //wrap_unistd::lockf_unlock_no_err( pid_fd );
    }
    //-----------------------------------------------------------------------------------
    vapplication::args_parser args;

    std::string conf_name;

    std::string pid_name;
    safe_fd pid_fd;
    help_call help;
    //-----------------------------------------------------------------------------------
};
//=======================================================================================
arguments::arguments( int argc, const char * const * const argv, help_call help )
    : _p( new _pimpl(argc,argv) )
{
    _p->help = help ? help : &_pimpl::print_phony_help;

    if ( _p->find_help() )
        _p->print_help_and_exit(0);

    _p->find_config();
    _p->find_pid();
    //_p->lock_pid();

    _p->check_remained_args();
}
//=======================================================================================
niias::arguments::~arguments()
{}
//=======================================================================================
std::string niias::arguments::config_name() const
{
    return _p->conf_name;
}
//=======================================================================================
vsettings niias::arguments::settings() const
{
    vsettings res;
    res.from_ini_file( _p->conf_name );
    return res;
}
//=======================================================================================
