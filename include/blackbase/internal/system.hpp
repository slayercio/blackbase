#pragma once

#   if defined(_WIN32) || defined(_WIN64)
#       define BLACKBASE_SYSTEM_WINDOWS
#   elif defined(__linux__)
#       define BLACKBASE_SYSTEM_LINUX
#   elif defined(__APPLE__) || defined(__MACH__)
#       define BLACKBASE_SYSTEM_MACOS
#   else
#       error "Unsupported platform"
#   endif