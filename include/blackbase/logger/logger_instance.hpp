#pragma once

#include <blackbase/logger/logger.hpp>

std::shared_ptr<blackbase::Logger> blackbase::Logger::Create()
{
    if (!g_Logger)
    {
        g_Logger = std::make_shared<blackbase::Logger>();
    }
    
    return g_Logger;
}

void blackbase::Logger::Destroy()
{
    g_Logger.reset();
}

std::shared_ptr<blackbase::Logger> blackbase::g_Logger;