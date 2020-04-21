#ifndef NIIAS_ARGUMENTS_H
#define NIIAS_ARGUMENTS_H

#include <functional>
#include <memory>
#include "vsettings.h"

//=======================================================================================
namespace niias
{
    //===================================================================================
    using help_call = std::function<void()>;

    class arguments final
    {
    public:

        arguments( int argc, char const * const * const argv, help_call help = {} );
        ~arguments();

        std::string config_name() const;
        vsettings   settings()    const;

    private:
        class _pimpl; std::unique_ptr<_pimpl> _p;
    };
    //===================================================================================
} // namespace niias
//=======================================================================================


#endif // NIIAS_ARGUMENTS_H
