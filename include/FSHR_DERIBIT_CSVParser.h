#pragma once

#include "FSHR_DERIBIT_Macro.h"
#include "FSHR_DERIBIT_Enums.h"
#include "FSHR_DERIBIT_ProtocolTraits.h"
#include "FSHR_DERIBIT_Order.h"

#include <string>
#include <vector>
#include <string_view>
#include <memory>

namespace fischer::deribit
{
    template<typename Traits = DeribitTraits>
    class CsvParser
    {
    public:
        using OrderType = Order<Traits>;
        using SizeType = typename Traits::SizeType;

        CsvParser();
        RULE_OF_FIVE_MOVABLE(CsvParser);

        bool LoadFile(const std::string& filename);
        std::vector<OrderType> ParseOrders();

        bool IsFileLoaded() const { return nullptr != m_FileBuffer; }
        SizeType GetFileSize() const { return m_FileSize; }
        ParserState GetState() const { return m_State; }

    protected:
        void ParseHeaders(const char* start, const char* end);
        bool ParseDataLine(const char* start, const char* end, OrderType& order);
        void AssignFieldValue(OrderType& order, std::string_view fieldName,
                              const char* value, SizeType length);
        FieldIndex GetFieldIndex(std::string_view fieldName) const noexcept;

    private:
        std::unique_ptr<char[]> m_FileBuffer;
        SizeType m_FileSize;
        ParserState m_State;
        std::vector<std::string_view> m_Headers;
        std::array<FieldIndex, Traits::MaxFieldCount> m_FieldMapping;
    };
} 

#include <FSHR_DERIBIT_CSVParser.hxx>