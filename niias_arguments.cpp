#include "niias_arguments.h"

#include <iostream>
#include <cassert>
#include <fcntl.h>

#include "vlog.h"
#include "vapplication.h"
#include "vcmdline_parser.h"
#include "vbyte_buffer.h"
#include "vgit.h"
#include "impl_vposix/wrap_fcntl.h"
#include "impl_vposix/wrap_unistd.h"
#include "impl_vposix/wrap_errno.h"
#include "impl_vposix/safe_fd.h"
#include "impl_vposix/wrap_sys_file.h"


using std::string;
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
    void print_help()
    {
        if ( help.empty() )
            print_phony_help();
        else
            std::cout << help;
    }
    //-----------------------------------------------------------------------------------
    _pimpl( int argc, const char * const * const argv )
        : args(argc,argv)
    {}
    //-----------------------------------------------------------------------------------
    ~_pimpl()
    {
        if ( !pid_fname.empty() )
            wrap_unistd::unlink_no_err( pid_fname );
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
        conf_fname = find_smth( "-c", "-c", "config=", "--config=" );
        return !conf_fname.empty();
    }
    //-----------------------------------------------------------------------------------
    bool find_pid()
    {
        pid_fname = find_smth( "-p", "-p", "pid=", "--pid=" );
        return !pid_fname.empty();
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

        print_help();
        vwarning << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
        vwarning << "Found unused arguments:" << unused;
        vwarning << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
        exit(EXIT_FAILURE);
    }
    //-----------------------------------------------------------------------------------
    void lock_pid()
    {
        assert( !pid_fname.empty() );
        pid_fd = wrap_fcntl::open( pid_fname.c_str(), O_CREAT|O_RDWR|O_LARGEFILE, 0644 );

        auto locked = wrap_sys_file::try_lock_exclusive( pid_fd );
        if ( !locked )
        {
            vbyte_buffer text = wrap_unistd::read( pid_fd );
            throw verror << "Cannot lock PID file '" << pid_fname << "', possible PID "
                         << "of locked process: '" << text.trim_spaces() << "'";
        }

        wrap_unistd::write(pid_fd, vcat().aligned(vapplication::pid(),10,' ')('\n'));
    }
    //-----------------------------------------------------------------------------------
    void autoreplace_setting_value( std::string * source );
    void autoreplace_recurse( vsettings * settings );
    vcmdline_parser args;

    std::string conf_fname;

    std::string pid_fname;
    safe_fd pid_fd;
    std::string help;
    //-----------------------------------------------------------------------------------
};
//=======================================================================================
arguments::arguments( int argc, const char * const * const argv, std::string help )
    : _p( new _pimpl(argc,argv) )
{
    vgit::print_and_exit_if_need( argc, argv );

    _p->help = help;

    if ( _p->find_help() )
    {
        _p->print_help();
        exit(EXIT_SUCCESS);
    }

    if ( _p->find_pid() )
        _p->lock_pid();

    _p->find_config();

    _p->check_remained_args();
}
//=======================================================================================
niias::arguments::~arguments()
{}
//=======================================================================================
std::string arguments::app_name() const
{
    return _p->args.app_name();
}
//=======================================================================================
std::string arguments::app_path() const
{
    return _p->args.app_path();
}
//=======================================================================================
std::string arguments::full_app() const
{
    return _p->args.full_app();
}
//=======================================================================================
bool arguments::has_config_name() const
{
    return !config_name().empty();
}
//=======================================================================================
const std::string& niias::arguments::config_name() const
{
    return _p->conf_fname;
}
//=======================================================================================
static void replace_all_entries( string * data,
                                 const string& from_what,
                                 const string& to_what )
{
    while (1)
    {
        auto pos = data->find( from_what );

        if ( pos == string::npos )
            return;

        data->replace( pos, from_what.size(), to_what );
    }
}
//---------------------------------------------------------------------------------------
void arguments::_pimpl::autoreplace_setting_value( std::string * source )
{
    replace_all_entries( source, "$$APP_NAME", args.app_name() );
    replace_all_entries( source, "$$APP_PATH", args.app_path() );
    replace_all_entries( source, "$$FULL_APP", args.full_app() );
}
//---------------------------------------------------------------------------------------
void arguments::_pimpl::autoreplace_recurse( vsettings * settings )
{
    auto keys = settings->keys();
    for ( auto && key: keys )
    {
        auto value = settings->get( key );
        autoreplace_setting_value( &value );
        settings->set( key, value );
    }

    auto subgroups = settings->subgroups();
    for ( auto && subgroup: subgroups )
    {
        autoreplace_recurse( &settings->subgroup(subgroup) );
    }
}
//---------------------------------------------------------------------------------------
vsettings niias::arguments::settings() const
{
    if ( !has_config_name() )
        throw verror << "Config name has not catched from arguments.";

    vsettings res;
    res.from_ini_file( _p->conf_fname );

    _p->autoreplace_recurse( &res );

    return res;
}
//=======================================================================================
