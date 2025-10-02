#pragma once

#include "FSHR_DERIBIT_Enums.h"
#include <string_view>

namespace fischer::deribit::utils
{
    inline constexpr bool ParseBool(char c) noexcept
    {
        return 't' == c || 'T' == c || '1' == c;
    }

    constexpr std::string_view OrderDirectionToString(OrderDirection direction)
    {
        switch (direction)
        {
        case OrderDirection::Buy:
            return "buy";
        case OrderDirection::Sell:
            return "sell";
        default:
            return "unknown";
        }
    }

    constexpr std::string_view OrderTypeToString(OrderType type)
    {
        switch (type)
        {
        case OrderType::Limit:
            return "limit";
        case OrderType::Market:
            return "market";
        case OrderType::StopLimit:
            return "stop_limit";
        case OrderType::StopMarket:
            return "stop_market";
        case OrderType::TakeLimit:
            return "take_limit";
        case OrderType::TakeMarket:
            return "take_market";
        case OrderType::MarketLimit:
            return "market_limit";
        case OrderType::TrailingStop:
            return "trailing_stop";
        default:
            return "unknown";
        }
    }

    constexpr std::string_view TimeInForceToString(TimeInForce tif)
    {
        switch (tif)
        {
        case TimeInForce::GoodTilCancelled:
            return "good_til_cancelled";
        case TimeInForce::GoodTilDay:
            return "good_til_day";
        case TimeInForce::FillOrKill:
            return "fill_or_kill";
        case TimeInForce::ImmediateOrCancel:
            return "immediate_or_cancel";
        default:
            return "good_til_cancelled";
        }
    }

    constexpr std::string_view TriggerTypeToString(TriggerType trigger)
    {
        switch (trigger)
        {
        case TriggerType::IndexPrice:
            return "index_price";
        case TriggerType::MarkPrice:
            return "mark_price";
        case TriggerType::LastPrice:
            return "last_price";
        case TriggerType::None:
        default:
            return "";
        }
    }

    constexpr std::string_view AdvancedTypeToString(AdvancedType advanced)
    {
        switch (advanced)
        {
        case AdvancedType::USD:
            return "usd";
        case AdvancedType::ImpliedVolatility:
            return "implv";
        case AdvancedType::None:
        default:
            return "";
        }
    }

    constexpr std::string_view LinkedOrderTypeToString(LinkedOrderType type)
    {
        switch (type)
        {
        case LinkedOrderType::OneTriggersOther:
            return "one_triggers_other";
        case LinkedOrderType::OneCancelsOther:
            return "one_cancels_other";
        case LinkedOrderType::OneTriggersOneCancelsOther:
            return "one_triggers_one_cancels_other";
        case LinkedOrderType::None:
        default:
            return "";
        }
    }

    constexpr std::string_view TriggerFillConditionToString(TriggerFillCondition condition)
    {
        switch (condition)
        {
        case TriggerFillCondition::FirstHit:
            return "first_hit";
        case TriggerFillCondition::CompleteFill:
            return "complete_fill";
        case TriggerFillCondition::Incremental:
            return "incremental";
        default:
            return "first_hit";
        }
    }

    constexpr std::string_view OrderStateToString(OrderState state)
    {
        switch (state)
        {
        case OrderState::Open:
            return "open";
        case OrderState::Filled:
            return "filled";
        case OrderState::Rejected:
            return "rejected";
        case OrderState::Cancelled:
            return "cancelled";
        case OrderState::Untriggered:
            return "untriggered";
        case OrderState::Archive:
            return "archive";
        default:
            return "unknown";
        }
    }

    constexpr std::string_view LogLevelToString(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warning:
            return "WARN";
        case LogLevel::Error:
            return "ERROR";
        case LogLevel::Critical:
            return "CRITICAL";
        default:
            return "UNKNOWN";
        }
    }

    constexpr OrderType StringToOrderType(std::string_view str)
    {
        if ("limit" == str) return OrderType::Limit;
        if ("market" == str) return OrderType::Market;
        if ("stop_limit" == str) return OrderType::StopLimit;
        if ("stop_market" == str) return OrderType::StopMarket;
        if ("take_limit" == str) return OrderType::TakeLimit;
        if ("take_market" == str) return OrderType::TakeMarket;
        if ("market_limit" == str) return OrderType::MarketLimit;
        if ("trailing_stop" == str) return OrderType::TrailingStop;
        return OrderType::Limit;
    }

    constexpr OrderDirection StringToOrderDirection(std::string_view str)
    {
        if ("buy" == str) return OrderDirection::Buy;
        if ("sell" == str) return OrderDirection::Sell;
        return OrderDirection::Buy;
    }

    constexpr TimeInForce StringToTimeInForce(std::string_view str)
    {
        if ("good_til_cancelled" == str) return TimeInForce::GoodTilCancelled;
        if ("good_til_day" == str) return TimeInForce::GoodTilDay;
        if ("fill_or_kill" == str) return TimeInForce::FillOrKill;
        if ("immediate_or_cancel" == str) return TimeInForce::ImmediateOrCancel;
        return TimeInForce::GoodTilCancelled;
    }

    constexpr TriggerType StringToTriggerType(std::string_view str)
    {
        if ("index_price" == str) return TriggerType::IndexPrice;
        if ("mark_price" == str) return TriggerType::MarkPrice;
        if ("last_price" == str) return TriggerType::LastPrice;
        return TriggerType::None;
    }

    constexpr AdvancedType StringToAdvancedType(std::string_view str)
    {
        if ("usd" == str) return AdvancedType::USD;
        if ("implv" == str) return AdvancedType::ImpliedVolatility;
        return AdvancedType::None;
    }

    constexpr LinkedOrderType StringToLinkedOrderType(std::string_view str)
    {
        if ("one_triggers_other" == str) return LinkedOrderType::OneTriggersOther;
        if ("one_cancels_other" == str) return LinkedOrderType::OneCancelsOther;
        if ("one_triggers_one_cancels_other" == str) return LinkedOrderType::OneTriggersOneCancelsOther;
        return LinkedOrderType::None;
    }

    constexpr TriggerFillCondition StringToTriggerFillCondition(std::string_view str)
    {
        if ("first_hit" == str) return TriggerFillCondition::FirstHit;
        if ("complete_fill" == str) return TriggerFillCondition::CompleteFill;
        if ("incremental" == str) return TriggerFillCondition::Incremental;
        return TriggerFillCondition::FirstHit;
    }
}