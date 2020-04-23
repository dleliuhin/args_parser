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

using namespace std;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wweak-vtables"
class niias_srv_supply: public testing::Test
{};
#pragma GCC diagnostic pop

//=======================================================================================
TEST_F( niias_srv_supply, _test_name )
{
    // type here
}
//
//  EXPECT_TRUE
//
//  EXPECT_EQ
//  EXPECT_FLOAT_EQ
//  EXPECT_DOUBLE_EQ
//
//  EXPECT_THROW
//  EXPECT_ANY_THROW
//  EXPECT_NO_THROW
//
//=======================================================================================


//=======================================================================================
//  Main, do not delete...
//=======================================================================================
int main(int argc, char *argv[])
{
//    "app -p lock -plock2";
    std::vector<const char*> vargs;
    vargs.push_back("/my/app/name");
    //vargs.push_back("-plock.test");
    vargs.push_back("-p");
    vargs.push_back("z_lock.pid");
    //vargs.push_back("-h");
    //vargs.push_back("ololo");

    niias::arguments args( vargs.size(), vargs.data() );
    usleep(5e6);
    return 0;

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
//=======================================================================================
//  Main, do not delete...
//=======================================================================================
