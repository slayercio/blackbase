#pragma once

#ifdef BLACKBASE_SYSTEM_WINDOWS
#include <blackbase/detail/cave/cave_finder_impl_win.hpp>
#elif defined(BLACKBASE_SYSTEM_LINUX)
#include <blackbase/detail/cave/cave_finder_impl_linux.hpp>
#elif defined(BLACKBASE_SYSTEM_MACOS)
#include <blackbase/detail/cave/cave_finder_impl_macos.hpp>
#endif