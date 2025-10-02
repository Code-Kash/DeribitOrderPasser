#pragma once

#include <cstdint>
#include <cstddef>
#include <string_view>

namespace fischer::deribit
{
    // JSON Protocol
    constexpr std::string_view JsonRpcVersion = "2.0";
    constexpr std::string_view JsonPrefix = R"({"id":)";
    constexpr std::string_view JsonRpcField = R"(,"jsonrpc":"2.0","method":"private/)";
    constexpr std::string_view ParamsPrefix = R"(","params":{)";
    constexpr std::string_view JsonSuffix = "}}";
    constexpr std::string_view NewLine = "\n";
    constexpr std::string_view MethodBuy = "buy";
    constexpr std::string_view MethodSell = "sell";
    constexpr std::string_view PrivatePrefix = "private/";

    // Buffer and Memory
    constexpr size_t InitialBufferSize = 40960;
    constexpr size_t GrowthFactor = 2;
    constexpr size_t MaxOrderCount = 10000;
    constexpr size_t MaxFieldCount = 32;
    constexpr size_t MaxDoubleStringLength = 32;
    constexpr size_t MaxInt64StringLength = 20;
    constexpr size_t EstimatedMessageSize = 512;

    // CSV Parsing
    constexpr char FieldDelimiter = ',';
    constexpr char LineDelimiter = '\n';
    constexpr char CarriageReturn = '\r';
    constexpr char Space = ' ';
    constexpr char NullTerminator = '\0';

    // Field Names
    constexpr std::string_view FieldId = "id";
    constexpr std::string_view FieldDirection = "direction";
    constexpr std::string_view FieldAmount = "amount";
    constexpr std::string_view FieldContracts = "contracts";
    constexpr std::string_view FieldInstrumentName = "instrument_name";
    constexpr std::string_view FieldLabel = "label";
    constexpr std::string_view FieldType = "type";
    constexpr std::string_view FieldPrice = "price";
    constexpr std::string_view TimeInForce = "time_in_force";
    constexpr std::string_view PostOnly = "post_only";
    constexpr std::string_view RejectPostOnly = "reject_post_only";
    constexpr std::string_view ReduceOnly = "reduce_only";
    constexpr std::string_view TriggerPrice = "trigger_price";
    constexpr std::string_view TriggerOffset = "trigger_offset";
    constexpr std::string_view Trigger = "trigger";
    constexpr std::string_view DisplayAmount = "display_amount";
    constexpr std::string_view Advanced = "advanced";
    constexpr std::string_view Mmp = "mmp";
    constexpr std::string_view ValidUntil = "valid_until";
    constexpr std::string_view LinkedOrderType = "linked_order_type";
    constexpr std::string_view TriggerFillCondition = "trigger_fill_condition";


    // File I/O
    constexpr std::string_view DefaultInputFile = "deribit_orders.txt";
    constexpr std::string_view DefaultOutputFile = "output.txt";
    constexpr std::string_view DefaultLogFile = "deribit_processor.log";

    // Performance and Metrics
    constexpr double MicrosecondsToMilliseconds = 1000.0;
    constexpr double MicrosecondsToSeconds = 1000000.0;
    constexpr int64_t InitialMessageId = 5275;

    // Logger
    constexpr size_t MaxLogMessageLength = 1024;
    constexpr std::string_view TimestampFormat = "%Y-%m-%d %H:%M:%S";
    constexpr size_t TimestampBufferSize = 32;
}