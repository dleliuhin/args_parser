#ifndef NIIAS_ARGUMENTS_H
#define NIIAS_ARGUMENTS_H

#include <memory>
#include "vsettings.h"

//=======================================================================================
namespace niias
{
    //===================================================================================
    class arguments final
    {
    public:

        arguments( int argc, char const * const * const argv,
                   const std::string& app_description,
                   const vsettings&   default_settings );

        ~arguments();

        std::string app_name() const;
        std::string app_path() const;
        std::string full_app() const;

        vsettings settings() const;

    private:
        class _pimpl; std::unique_ptr<_pimpl> _p;
    };
    //===================================================================================
} // namespace niias
//=======================================================================================


#endif // NIIAS_ARGUMENTS_H
