#pragma once

#include <cstdint>

namespace fischer::deribit
{
    enum class OrderDirection : uint8_t
    {
        Buy = 0,
        Sell = 1
    };

    enum class OrderType : uint8_t
    {
        Limit = 0,
        Market = 1,
        StopLimit = 2,
        StopMarket = 3,
        TakeLimit = 4,
        TakeMarket = 5,
        MarketLimit = 6,
        TrailingStop = 7
    };

    enum class TimeInForce : uint8_t
    {
        GoodTilCancelled = 0,
        GoodTilDay = 1,
        FillOrKill = 2,
        ImmediateOrCancel = 3
    };

    enum class TriggerType : uint8_t
    {
        None = 0,
        IndexPrice = 1,
        MarkPrice = 2,
        LastPrice = 3
    };

    // Advanced Order Type (Options only)
    enum class AdvancedType : uint8_t
    {
        None = 0,
        USD = 1,
        ImpliedVolatility = 2
    };

    enum class LinkedOrderType : uint8_t
    {
        None = 0,
        OneTriggersOther = 1,
        OneCancelsOther = 2,
        OneTriggersOneCancelsOther = 3
    };

    enum class TriggerFillCondition : uint8_t
    {
        FirstHit = 0,
        CompleteFill = 1,
        Incremental = 2
    };

    enum class OrderState : uint8_t
    {
        Open = 0,
        Filled = 1,
        Rejected = 2,
        Cancelled = 3,
        Untriggered = 4,
        Archive = 5
    };

    enum class LogLevel : uint8_t
    {
        Debug = 0,
        Info = 1,
        Warning = 2,
        Error = 3,
        Critical = 4
    };

    enum class ParserState : uint8_t
    {
        NotLoaded = 0,
        Loading = 1,
        Loaded = 2,
        Parsing = 3,
        Complete = 4,
        Error = 5
    };

    enum class ProcessingStatus : uint8_t
    {
        Idle = 0,
        Parsing = 1,
        Building = 2,
        Writing = 3,
        Complete = 4,
        Failed = 5
    };

    // Field indices for O(1) CSV field dispatch
    enum class FieldIndex : int8_t
    {
        None = -1,
        Id = 0,
        Direction,
        Amount,
        Contracts,
        InstrumentName,
        Label,
        Type,
        Price,
        TimeInForce,
        PostOnly,
        RejectPostOnly,
        ReduceOnly,
        TriggerPrice,
        TriggerOffset,
        Trigger,
        DisplayAmount,
        Advanced,
        Mmp,
        ValidUntil,
        LinkedOrderType,
        TriggerFillCondition,
        MaxFields
    };
}