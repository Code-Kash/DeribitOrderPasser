#pragma once

#include <cstdint>
#include <cstddef>
#include <string_view>
#include <type_traits>

namespace fischer::deribit
{
    struct DeribitTraits
    {
        using OrderIdType = int64_t;
        using MessageIdType = int64_t;
        using AmountType = double;
        using PriceType = double;
        using TimestampType = int64_t;
        using SizeType = size_t;

        // Buffer Configuration
        static constexpr SizeType InitialJsonBufferSize = 40960;
        static constexpr SizeType BufferGrowthFactor = 2;
        static constexpr SizeType MaxOrderCount = 10000;
        static constexpr SizeType MaxFieldCount = 32;
        static constexpr SizeType MaxLabelLength = 64;
        static constexpr SizeType EstimatedMessageSize = 512;

        // Numeric Formatting
        static constexpr SizeType MaxDoubleStringLength = 32;
        static constexpr SizeType MaxInt64StringLength = 20;
        static constexpr int DoublePrecision = 10;

        // Protocol Configuration
        static constexpr std::string_view ProtocolName = "Deribit";
        static constexpr std::string_view JsonRpcVersion = "2.0";
        static constexpr MessageIdType InitialMessageId = 5275;

        // Performance Tuning
        static constexpr bool EnableMemoryMapping = false;  // Can be enabled later
        static constexpr bool EnableVectorReserve = true;
        static constexpr bool EnableBufferPreallocation = true;

        // Logger Configuration
        static constexpr SizeType MaxLogMessageLength = 1024;
        static constexpr SizeType LogBufferSize = 8192;
        static constexpr bool EnableFileLogging = true;
        static constexpr bool EnableConsoleLogging = true;

        // CSV Parser Configuration
        static constexpr char FieldDelimiter = ',';
        static constexpr char LineDelimiter = '\n';
        static constexpr bool TrimWhitespace = true;
        static constexpr bool SkipEmptyLines = true;

        // Compile-time Checks
        static_assert(InitialJsonBufferSize > 0, "Buffer size must be positive");
        static_assert(BufferGrowthFactor >= 2, "Growth factor must be at least 2");
        static_assert(MaxOrderCount > 0, "Max order count must be positive");
        static_assert(DoublePrecision > 0 && DoublePrecision <= 17, "Invalid double precision");
    };

    template<typename Traits>
    using OrderId = typename Traits::OrderIdType;

    template<typename Traits>
    using MessageId = typename Traits::MessageIdType;

    template<typename Traits>
    using Amount = typename Traits::AmountType;

    template<typename Traits>
    using Price = typename Traits::PriceType;

    template<typename Traits>
    using Size = typename Traits::SizeType;
}