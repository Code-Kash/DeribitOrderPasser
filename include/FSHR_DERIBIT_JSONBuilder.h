#pragma once

#include "FSHR_DERIBIT_Macro.h"
#include "FSHR_DERIBIT_ProtocolTraits.h"
#include "FSHR_DERIBIT_Order.h"

#include <string>
#include <cstdint>
#include <memory>

namespace fischer::deribit
{
    template<typename Traits = DeribitTraits>
    class JsonBuilder
    {
    public:
        using OrderType = Order<Traits>;
        using MessageIdType = typename Traits::MessageIdType;
        using SizeType = typename Traits::SizeType;

        JsonBuilder();
        RULE_OF_FIVE_MOVABLE(JsonBuilder);

        void Reset();
        void BuildOrderMessage(const OrderType& order, MessageIdType messageId);

        std::string GetResult() const;
        SizeType GetBufferPosition() const { return m_Position; }

    protected:
        void EnsureCapacity(SizeType needed);
        void AppendChar(char c);
        void AppendString(const char* str, SizeType length);
        void AppendQuotedString(const std::string& str);
        void AppendFieldName(const char* name, bool isFirst);
        void AppendInt64(int64_t value);
        void AppendDouble(double value);
        void AppendBoolean(bool value);

    private:
        std::unique_ptr<char[]> m_Buffer;
        SizeType m_Capacity;
        SizeType m_Position;
    };
}

#include <FSHR_DERIBIT_JSONBuilder.hxx>