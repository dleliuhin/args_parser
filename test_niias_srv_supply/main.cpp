/****************************************************************************************
**
**  VLIBS codebase, NIIAS
**
**  GNU Lesser General Public License Usage
**  This file may be used under the terms of the GNU Lesser General Public License
**  version 3 as published by the Free Software Foundation and appearing in the file
**  LICENSE.LGPL3 included in the packaging of this file. Please review the following
**  information to ensure the GNU Lesser General Public License version 3 requirements
**  will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
****************************************************************************************/

#include "gtest/gtest.h"

#include "niias_arguments.h"
#include "vcat.h"
#include "vlog.h"
#include "vprocess.h"
#include <fstream>

using namespace std;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wweak-vtables"
class niias_srv_supply: public testing::Test
{};
#pragma GCC diagnostic pop

//=======================================================================================

void test_autoreplace() {}
TEST_F( niias_srv_supply, test_autoreplace )
{
    //  Prepare ini file with key 'same' and autoreplace macroses.
    {
        string sett_text = "same = $$APP_PATH/$$APP_NAME = $$FULL_APP\n";
        ofstream s;
        s.open("test.ini");
        assert( s.good() );
        s << sett_text;
    }

    //  Prepare arguments.
    std::vector<const char*> vargs;    
    vargs.push_back("/my/app/name");
    vargs.push_back("-ctest.ini");      // Use notation without space.

    niias::arguments args( vargs.size(), vargs.data(), "Test autoreplace", {} );

    EXPECT_EQ( args.app_name(), "name" );
    EXPECT_EQ( args.app_path(), "/my/app" );
    EXPECT_EQ( args.full_app(), "/my/app/name" );

    auto sett = args.settings();
    EXPECT_EQ( sett.get("same"), "/my/app/name = /my/app/name" );
}

//=======================================================================================

void test_pid_lock()
{
    //  Если запускать несколько программ без задержки, то можно получить блокировку.
    //  Как тест это не реализовано, чтобы не жрало время.
    //    "app -p lock -plock2";
        std::vector<const char*> vargs;
        vargs.push_back("/my/app/name");
        //vargs.push_b ack("-plock.test");
        vargs.push_back("-p");
        vargs.push_back("z_lock.pid");
        //vargs.push_back("-h");
        //vargs.push_back("ololo");

        niias::arguments args( vargs.size(), vargs.data(), "", {} );
        usleep(5e6);
}

//=======================================================================================

void test_print_conf()
{
    std::vector<const char*> vargs;
    vargs.push_back("/my/app/name");
    vargs.push_back("--print-conf");

    // will exit after print empty config.
    niias::arguments args( vargs.size(), vargs.data(), "Description of service", {} );
}

//=======================================================================================


//=======================================================================================
//  Settings example
//=======================================================================================
struct Settings
{
    struct ZCM
    {
        std::string target  = "ipc";
        std::string channel = "channel";
    } zcm;

    struct Logs
    {
        bool need_cout = true;

        bool need_shared = true;
        std::string shared_name = "$$FULL_APP.log";     // For autoreplace

        bool need_leveled = true;
        std::string leveled_path = "$$APP_PATH/logs";   // For autoreplace

        uint file_sizes = 1e6;
        uint rotates = 3;

        void setup()
        {
            if ( !need_cout )
                vlog::clear_executers();

            if ( need_shared )
                vlog::set_shared_log( shared_name, file_sizes, rotates );

            if ( need_leveled )
                vlog::set_leveled_log( leveled_path, file_sizes, rotates );
        }
    } logs;

    Settings()
    {
        _schema.subgroup( "ZCM" );
        _schema.add( "target",  &zcm.target  );
        _schema.add( "channel", &zcm.channel );
        _schema.end_subgroup();

        _schema.subgroup( "LOGS" );
        _schema.add( "need_cout",    &logs.need_cout    );

        _schema.add( "need_shared",  &logs.need_shared  );
        _schema.add( "shared_name",  &logs.shared_name  );

        _schema.add( "need_leveled", &logs.need_leveled );
        _schema.add( "leveled_path", &logs.leveled_path );

        _schema.add( "file_sizes", &logs.file_sizes );
        _schema.add( "rotates",    &logs.rotates    );
    }

    void capture( const vsettings& sett )
    {
        _schema.capture( sett );
    }

    static vsettings by_default()
    {
        return Settings()._schema.build();
    }

private:
    vsettings::schema _schema;
};
//=======================================================================================
//  Settings example
//=======================================================================================


//=======================================================================================
//  Main, do not delete...
//=======================================================================================
int main(int argc, char *argv[])
{
    niias::arguments args( argc, argv, "Example", Settings::by_default() );
    Settings settings;
    settings.capture( args.settings() );
    settings.logs.setup();

    //test_print_conf();

   // ::testing::InitGoogleTest(&argc, argv);
   // return RUN_ALL_TESTS();
}
//=======================================================================================
//  Main, do not delete...
//=======================================================================================
