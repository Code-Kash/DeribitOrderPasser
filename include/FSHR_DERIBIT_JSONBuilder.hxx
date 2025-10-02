#include "FSHR_DERIBIT_JSONBuilder.h"
#include "FSHR_DERIBIT_Constants.h"
#include "FSHR_DERIBIT_Logger.h"

#include <cstring>
#include <cstdio>
#include <algorithm>

namespace fischer::deribit
{
    template<typename Traits>
    JsonBuilder<Traits>::JsonBuilder()
        : m_Capacity{Traits::InitialJsonBufferSize}
        , m_Position{0}
    {
        m_Buffer = std::make_unique<char[]>(m_Capacity);
        LOG_DEBUG("JsonBuilder initialized with buffer size:", m_Capacity);
    }

    template<typename Traits>
    void JsonBuilder<Traits>::Reset()
    {
        m_Position = 0;
        LOG_DEBUG("JsonBuilder buffer reset");
    }

    template<typename Traits>
    void JsonBuilder<Traits>::BuildOrderMessage(const OrderType& order, MessageIdType messageId)
    {
        EnsureCapacity(Traits::EstimatedMessageSize);

        // Start JSON message
        AppendString(JsonPrefix.data(), JsonPrefix.size());
        AppendInt64(messageId);

        // Add JSON-RPC method
        AppendString(JsonRpcField.data(), JsonRpcField.size());
        AppendString(order.m_Direction.c_str(), order.m_Direction.length());
        AppendString(ParamsPrefix.data(), ParamsPrefix.size());

        bool isFirst = true;

        // Required fields
        if (0.0 < order.m_Amount)
        {
            AppendFieldName(FieldAmount.data(), isFirst);
            AppendDouble(order.m_Amount);
            isFirst = false;
        }

        if (0.0 < order.m_Contracts)
        {
            AppendFieldName(FieldContracts.data(), isFirst);
            AppendDouble(order.m_Contracts);
            isFirst = false;
        }

        if (false == order.m_InstrumentName.empty())
        {
            AppendFieldName(FieldInstrumentName.data(), isFirst);
            AppendQuotedString(order.m_InstrumentName);
            isFirst = false;
        }

        if (false == order.m_Label.empty())
        {
            AppendFieldName(FieldLabel.data(), isFirst);
            AppendQuotedString(order.m_Label);
            isFirst = false;
        }

        if (false == order.m_Type.empty())
        {
            AppendFieldName(FieldType.data(), isFirst);
            AppendQuotedString(order.m_Type);
            isFirst = false;
        }

        // Optional fields using std::optional
        if (order.m_Price.has_value())
        {
            AppendFieldName(FieldPrice.data(), isFirst);
            AppendDouble(order.m_Price.value());
            isFirst = false;
        }

        if (order.m_TimeInForce.has_value() && false == order.m_TimeInForce->empty())
        {
            AppendFieldName(TimeInForce.data(), isFirst);
            AppendQuotedString(order.m_TimeInForce.value());
            isFirst = false;
        }

        if (order.m_DisplayAmount.has_value())
        {
            AppendFieldName(DisplayAmount.data(), isFirst);
            AppendDouble(order.m_DisplayAmount.value());
            isFirst = false;
        }

        if (order.m_PostOnly.has_value())
        {
            AppendFieldName(PostOnly.data(), isFirst);
            AppendBoolean(order.m_PostOnly.value());
            isFirst = false;
        }

        if (order.m_RejectPostOnly.has_value())
        {
            AppendFieldName(RejectPostOnly.data(), isFirst);
            AppendBoolean(order.m_RejectPostOnly.value());
            isFirst = false;
        }

        if (order.m_ReduceOnly.has_value())
        {
            AppendFieldName(ReduceOnly.data(), isFirst);
            AppendBoolean(order.m_ReduceOnly.value());
            isFirst = false;
        }

        if (order.m_TriggerPrice.has_value())
        {
            AppendFieldName(TriggerPrice.data(), isFirst);
            AppendDouble(order.m_TriggerPrice.value());
            isFirst = false;
        }

        if (order.m_TriggerOffset.has_value())
        {
            AppendFieldName(TriggerOffset.data(), isFirst);
            AppendDouble(order.m_TriggerOffset.value());
            isFirst = false;
        }

        if (order.m_Trigger.has_value() && false == order.m_Trigger->empty())
        {
            AppendFieldName(Trigger.data(), isFirst);
            AppendQuotedString(order.m_Trigger.value());
            isFirst = false;
        }

        if (order.m_Advanced.has_value() && false == order.m_Advanced->empty())
        {
            AppendFieldName(Advanced.data(), isFirst);
            AppendQuotedString(order.m_Advanced.value());
            isFirst = false;
        }

        if (order.m_Mmp.has_value())
        {
            AppendFieldName(Mmp.data(), isFirst);
            AppendBoolean(order.m_Mmp.value());
            isFirst = false;
        }

        if (order.m_ValidUntil.has_value())
        {
            AppendFieldName(ValidUntil.data(), isFirst);
            AppendInt64(order.m_ValidUntil.value());
            isFirst = false;
        }

        if (order.m_LinkedOrderType.has_value() && false == order.m_LinkedOrderType->empty())
        {
            AppendFieldName(LinkedOrderType.data(), isFirst);
            AppendQuotedString(order.m_LinkedOrderType.value());
            isFirst = false;
        }

        if (order.m_TriggerFillCondition.has_value() && false == order.m_TriggerFillCondition->empty())
        {
            AppendFieldName(TriggerFillCondition.data(), isFirst);
            AppendQuotedString(order.m_TriggerFillCondition.value());
            isFirst = false;
        }

        // Close JSON message
        AppendString(JsonSuffix.data(), JsonSuffix.size());
        AppendString(NewLine.data(), NewLine.size());
    }

    template<typename Traits>
    std::string JsonBuilder<Traits>::GetResult() const
    {
        return std::string(m_Buffer.get(), m_Position);
    }

    template<typename Traits>
    void JsonBuilder<Traits>::EnsureCapacity(SizeType needed)
    {
        if (m_Position + needed > m_Capacity)
        {
            m_Capacity *= Traits::BufferGrowthFactor;

            auto newBuffer = std::make_unique<char[]>(m_Capacity);
            std::memcpy(newBuffer.get(), m_Buffer.get(), m_Position);

            m_Buffer = std::move(newBuffer);

            LOG_DEBUG("JsonBuilder buffer expanded to:", m_Capacity, "bytes");
        }
    }

    template<typename Traits>
    void JsonBuilder<Traits>::AppendChar(char c)
    {
        m_Buffer[m_Position++] = c;
    }

    template<typename Traits>
    void JsonBuilder<Traits>::AppendString(const char* str, SizeType length)
    {
        std::memcpy(m_Buffer.get() + m_Position, str, length);
        m_Position += length;
    }

    template<typename Traits>
    void JsonBuilder<Traits>::AppendQuotedString(const std::string& str)
    {
        AppendChar('"');
        AppendString(str.c_str(), str.length());
        AppendChar('"');
    }

    template<typename Traits>
    void JsonBuilder<Traits>::AppendFieldName(const char* name, bool isFirst)
    {
        if (false == isFirst)
        {
            AppendChar(',');
        }

        AppendChar('"');
        AppendString(name, std::strlen(name));
        AppendChar('"');
        AppendChar(':');
    }

    template<typename Traits>
    void JsonBuilder<Traits>::AppendInt64(int64_t value)
    {
        m_Position += static_cast<SizeType>(
            std::snprintf(m_Buffer.get() + m_Position,
                         Traits::MaxInt64StringLength, "%ld", value));
    }

    template<typename Traits>
    void JsonBuilder<Traits>::AppendDouble(double value)
    {
        m_Position += static_cast<SizeType>(
            std::snprintf(m_Buffer.get() + m_Position,
                         Traits::MaxDoubleStringLength, "%.10g", value));
    }

    template<typename Traits>
    void JsonBuilder<Traits>::AppendBoolean(bool value)
    {
        if (true == value)
        {
            AppendString("true", 4);
        }
        else
        {
            AppendString("false", 5);
        }
    }

    // Explicit template instantiation
    template class JsonBuilder<DeribitTraits>;
}