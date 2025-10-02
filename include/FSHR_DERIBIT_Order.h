#pragma once

#include "FSHR_DERIBIT_Macro.h"
#include "FSHR_DERIBIT_ProtocolTraits.h"

#include <string>
#include <optional>
#include <cstdint>

namespace fischer::deribit
{
    template<typename Traits = DeribitTraits>
    struct alignas(64) Order
    {
        using OrderIdType = typename Traits::OrderIdType;
        using AmountType  = typename Traits::AmountType;
        using PriceType   = typename Traits::PriceType;

        // Required Fields - Always present in every order
        OrderIdType                     m_Id{0};
        AmountType                      m_Amount{0.0};
        AmountType                      m_Contracts{0.0};
        std::string                     m_InstrumentName;
        std::string                     m_Direction;
        std::string                     m_Type;
        std::string                     m_Label;

        // Optional Fields
        std::optional<PriceType>        m_Price;
        std::optional<PriceType>        m_TriggerPrice;
        std::optional<PriceType>        m_TriggerOffset;
        std::optional<AmountType>       m_DisplayAmount;
        std::optional<std::string>      m_TimeInForce;
        std::optional<std::string>      m_Trigger;
        std::optional<std::string>      m_Advanced;
        std::optional<bool>             m_PostOnly;
        std::optional<bool>             m_RejectPostOnly;
        std::optional<bool>             m_ReduceOnly;
        std::optional<bool>             m_Mmp;
        std::optional<int64_t>          m_ValidUntil;
        std::optional<std::string>      m_LinkedOrderType;
        std::optional<std::string>      m_TriggerFillCondition;

        Order() = default;
        RULE_OF_FIVE_TRIVIALLY_COPYABLE(Order);
    };
}