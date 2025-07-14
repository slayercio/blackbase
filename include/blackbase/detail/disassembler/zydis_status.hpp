#pragma once
#include <Zydis/Zydis.h>
#include <type_traits>
#include <string_view>
#include <unordered_map>

namespace blackbase::detail::disassembler
{
    #define ADD_ZYDIS_STATUS(Status) \
        { Status, #Status }

    class ZydisMessage
    {
    private:
        inline static std::unordered_map<ZyanStatus, std::string_view> statusMessages = {
            ADD_ZYDIS_STATUS(ZYDIS_STATUS_NO_MORE_DATA),
            ADD_ZYDIS_STATUS(ZYDIS_STATUS_DECODING_ERROR),
            ADD_ZYDIS_STATUS(ZYDIS_STATUS_INSTRUCTION_TOO_LONG),
            ADD_ZYDIS_STATUS(ZYDIS_STATUS_BAD_REGISTER),
            ADD_ZYDIS_STATUS(ZYDIS_STATUS_ILLEGAL_LOCK),
            ADD_ZYDIS_STATUS(ZYDIS_STATUS_ILLEGAL_LEGACY_PFX),
            ADD_ZYDIS_STATUS(ZYDIS_STATUS_ILLEGAL_REX),
            ADD_ZYDIS_STATUS(ZYDIS_STATUS_INVALID_MAP),
            ADD_ZYDIS_STATUS(ZYDIS_STATUS_MALFORMED_EVEX),
            ADD_ZYDIS_STATUS(ZYDIS_STATUS_MALFORMED_MVEX),
            ADD_ZYDIS_STATUS(ZYDIS_STATUS_INVALID_MASK),
            ADD_ZYDIS_STATUS(ZYDIS_STATUS_SKIP_TOKEN),
            ADD_ZYDIS_STATUS(ZYDIS_STATUS_IMPOSSIBLE_INSTRUCTION),

            ADD_ZYDIS_STATUS(ZYAN_STATUS_SUCCESS),
            ADD_ZYDIS_STATUS(ZYAN_STATUS_FAILED),
            ADD_ZYDIS_STATUS(ZYAN_STATUS_TRUE),
            ADD_ZYDIS_STATUS(ZYAN_STATUS_FALSE),
            ADD_ZYDIS_STATUS(ZYAN_STATUS_INVALID_ARGUMENT),
            ADD_ZYDIS_STATUS(ZYAN_STATUS_INVALID_OPERATION),
            ADD_ZYDIS_STATUS(ZYAN_STATUS_ACCESS_DENIED),
            ADD_ZYDIS_STATUS(ZYAN_STATUS_NOT_FOUND),
            ADD_ZYDIS_STATUS(ZYAN_STATUS_OUT_OF_RANGE),
            ADD_ZYDIS_STATUS(ZYAN_STATUS_INSUFFICIENT_BUFFER_SIZE),
            ADD_ZYDIS_STATUS(ZYAN_STATUS_NOT_ENOUGH_MEMORY),
            ADD_ZYDIS_STATUS(ZYAN_STATUS_BAD_SYSTEMCALL),
            ADD_ZYDIS_STATUS(ZYAN_STATUS_OUT_OF_RESOURCES),
            ADD_ZYDIS_STATUS(ZYAN_STATUS_MISSING_DEPENDENCY),
            ADD_ZYDIS_STATUS(ZYAN_STATUS_ARG_NOT_UNDERSTOOD),
            ADD_ZYDIS_STATUS(ZYAN_STATUS_TOO_FEW_ARGS),
            ADD_ZYDIS_STATUS(ZYAN_STATUS_TOO_MANY_ARGS),
            ADD_ZYDIS_STATUS(ZYAN_STATUS_ARG_MISSES_VALUE),
            ADD_ZYDIS_STATUS(ZYAN_STATUS_REQUIRED_ARG_MISSING)
        };

    public:
        static constexpr std::string_view GetMessage(ZyanStatus status)
        {
            return statusMessages.contains(status) ? statusMessages[status] : "Unknown Status";
        }
    };

    
}