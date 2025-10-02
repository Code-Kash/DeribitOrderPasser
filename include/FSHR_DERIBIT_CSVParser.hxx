#include "FSHR_DERIBIT_CSVParser.h"
#include "FSHR_DERIBIT_Logger.h"
#include "FSHR_DERIBIT_Constants.h"
#include "FSHR_DERIBIT_Utils.h"

#include <fstream>
#include <cstring>
#include <algorithm>
#include <charconv>

namespace fischer::deribit
{
    template<typename Traits>
    CsvParser<Traits>::CsvParser()
        : m_FileBuffer{nullptr}
        , m_FileSize{0}
        , m_State{ParserState::NotLoaded}
    {
        m_Headers.reserve(Traits::MaxFieldCount);
        m_FieldMapping.fill(FieldIndex::None);
    }

    template<typename Traits>
    bool CsvParser<Traits>::LoadFile(const std::string& filename)
    {
        m_State = ParserState::Loading;

        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (false == file.is_open())
        {
            LOG_ERROR("Failed to open CSV file:", filename);
            m_State = ParserState::Error;
            return false;
        }

        m_FileSize = static_cast<SizeType>(file.tellg());
        file.seekg(0);

        m_FileBuffer = std::make_unique<char[]>(m_FileSize + 1);
        file.read(m_FileBuffer.get(), static_cast<std::streamsize>(m_FileSize));
        m_FileBuffer[m_FileSize] = NullTerminator;

        if (false == file.good())
        {
            LOG_ERROR("Failed to read CSV file:", filename);
            m_State = ParserState::Error;
            return false;
        }

        m_State = ParserState::Loaded;
        LOG_DEBUG("CSV file loaded successfully. Size:", m_FileSize, "bytes");
        return true;
    }

    template<typename Traits>
    std::vector<typename CsvParser<Traits>::OrderType> CsvParser<Traits>::ParseOrders()
    {
        if (ParserState::Loaded != m_State)
        {
            LOG_ERROR("Cannot parse orders - file not loaded");
            return {};
        }

        m_State = ParserState::Parsing;

        std::vector<OrderType> orders;
        orders.reserve(Traits::MaxOrderCount);

        const char* current = m_FileBuffer.get();
        const char* end = m_FileBuffer.get() + m_FileSize;

        // Parse header line
        const char* lineEnd = std::strchr(current, LineDelimiter);
        if (nullptr == lineEnd)
        {
            LOG_ERROR("No header line found in CSV");
            m_State = ParserState::Error;
            return orders;
        }

        ParseHeaders(current, lineEnd);
        current = lineEnd + 1;

        // Parse data lines
        while (current < end)
        {
            lineEnd = std::strchr(current, LineDelimiter);
            if (nullptr == lineEnd)
            {
                lineEnd = end;
            }

            if (lineEnd > current)
            {
                OrderType order;
                if (true == ParseDataLine(current, lineEnd, order))
                {
                    orders.push_back(std::move(order));
                }
            }

            current = lineEnd + 1;
        }

        m_State = ParserState::Complete;
        LOG_INFO("Parsed", orders.size(), "orders from CSV");
        return orders;
    }

    template<typename Traits>
    void CsvParser<Traits>::ParseHeaders(const char* start, const char* end)
    {
        m_Headers.clear();
        m_FieldMapping.fill(FieldIndex::None);

        const char* current = start;
        size_t columnIndex = 0;

        while (current < end && columnIndex < Traits::MaxFieldCount)
        {
            const char* comma = std::strchr(current, FieldDelimiter);
            if (nullptr == comma || comma > end)
            {
                comma = end;
            }

            std::string_view header(current, static_cast<SizeType>(comma - current));
            m_Headers.emplace_back(header);

            // Map column index to field type for O(1) lookup during parsing
            m_FieldMapping[columnIndex] = GetFieldIndex(header);

            current = comma + 1;
            columnIndex++;
        }

        LOG_DEBUG("Parsed", m_Headers.size(), "CSV headers");
    }

    template<typename Traits>
    bool CsvParser<Traits>::ParseDataLine(const char* start, const char* end, OrderType& order)
    {
        const char* current = start;
        SizeType columnIndex = 0;

        while (current < end && columnIndex < m_Headers.size())
        {
            const char* comma = std::strchr(current, FieldDelimiter);
            if (nullptr == comma || comma > end)
            {
                comma = end;
            }

            SizeType length = static_cast<SizeType>(comma - current);

            // Trim trailing whitespace
            while (0 < length &&
                   (Space == current[length - 1] || CarriageReturn == current[length - 1]))
            {
                length--;
            }

            // Trim leading whitespace
            while (0 < length && Space == *current)
            {
                current++;
                length--;
            }

            // Use pre-computed field index for O(1) dispatch
            if (0 < length && columnIndex < Traits::MaxFieldCount)
            {
                const FieldIndex fieldIdx = m_FieldMapping[columnIndex];
                if (FieldIndex::None != fieldIdx)
                {
                    AssignFieldValue(order, m_Headers[columnIndex], current, length);
                }
            }

            current = comma + 1;
            columnIndex++;
        }

        return true;
    }

    template<typename Traits>
    void CsvParser<Traits>::AssignFieldValue(OrderType& order, std::string_view fieldName,
                                             const char* value, SizeType length)
    {
        const FieldIndex fieldIdx = GetFieldIndex(fieldName);

        switch (fieldIdx)
        {
        case FieldIndex::Id:
            std::from_chars(value, value + length, order.m_Id);
            break;

        case FieldIndex::Direction:
            order.m_Direction.assign(value, length);
            break;

        case FieldIndex::Amount:
            order.m_Amount = std::strtod(value, nullptr);
            break;

        case FieldIndex::Contracts:
            order.m_Contracts = std::strtod(value, nullptr);
            break;

        case FieldIndex::InstrumentName:
            order.m_InstrumentName.assign(value, length);
            break;

        case FieldIndex::Label:
            order.m_Label.assign(value, length);
            break;

        case FieldIndex::Type:
            order.m_Type.assign(value, length);
            break;

        case FieldIndex::Price:
            order.m_Price = std::strtod(value, nullptr);
            break;

        case FieldIndex::TimeInForce:
            order.m_TimeInForce = std::string(value, length);
            break;

        case FieldIndex::PostOnly:
            order.m_PostOnly = utils::ParseBool(value[0]);
            break;

        case FieldIndex::RejectPostOnly:
            order.m_RejectPostOnly = utils::ParseBool(value[0]);
            break;

        case FieldIndex::ReduceOnly:
            order.m_ReduceOnly = utils::ParseBool(value[0]);
            break;

        case FieldIndex::TriggerPrice:
            order.m_TriggerPrice = std::strtod(value, nullptr);
            break;

        case FieldIndex::TriggerOffset:
            order.m_TriggerOffset = std::strtod(value, nullptr);
            break;

        case FieldIndex::Trigger:
            order.m_Trigger = std::string(value, length);
            break;

        case FieldIndex::DisplayAmount:
            order.m_DisplayAmount = std::strtod(value, nullptr);
            break;

        case FieldIndex::Advanced:
            order.m_Advanced = std::string(value, length);
            break;

        case FieldIndex::Mmp:
            order.m_Mmp = utils::ParseBool(value[0]);
            break;

        case FieldIndex::ValidUntil:
            {
                int64_t validUntil = 0;
                std::from_chars(value, value + length, validUntil);
                order.m_ValidUntil = validUntil;
            }
            break;

        case FieldIndex::LinkedOrderType:
            order.m_LinkedOrderType = std::string(value, length);
            break;

        case FieldIndex::TriggerFillCondition:
            order.m_TriggerFillCondition = std::string(value, length);
            break;

        case FieldIndex::None:
        case FieldIndex::MaxFields:
            // Unknown field - ignore
            break;
        }
    }

    template<typename Traits>
    FieldIndex CsvParser<Traits>::GetFieldIndex(std::string_view fieldName) const noexcept
    {
        if (fieldName.empty()) return FieldIndex::None;

        const char first = fieldName[0];
        const size_t len = fieldName.length();

        // Lookup using first character and length to minimize comparisons
        switch (first)
        {
        case 'a':
            if (6 == len && "amount" == fieldName) return FieldIndex::Amount;
            if (8 == len && "advanced" == fieldName) return FieldIndex::Advanced;
            break;
        case 'c':
            if (9 == len && "contracts" == fieldName) return FieldIndex::Contracts;
            break;
        case 'd':
            if (9 == len && "direction" == fieldName) return FieldIndex::Direction;
            if (14 == len && "display_amount" == fieldName) return FieldIndex::DisplayAmount;
            break;
        case 'i':
            if (2 == len && "id" == fieldName) return FieldIndex::Id;
            if (15 == len && "instrument_name" == fieldName) return FieldIndex::InstrumentName;
            break;
        case 'l':
            if (5 == len && "label" == fieldName) return FieldIndex::Label;
            if (17 == len && "linked_order_type" == fieldName) return FieldIndex::LinkedOrderType;
            break;
        case 'm':
            if (3 == len && "mmp" == fieldName) return FieldIndex::Mmp;
            break;
        case 'p':
            if (5 == len && "price" == fieldName) return FieldIndex::Price;
            if (9 == len && "post_only" == fieldName) return FieldIndex::PostOnly;
            break;
        case 'r':
            if (11 == len && "reduce_only" == fieldName) return FieldIndex::ReduceOnly;
            if (16 == len && "reject_post_only" == fieldName) return FieldIndex::RejectPostOnly;
            break;
        case 't':
            if (4 == len && "type" == fieldName) return FieldIndex::Type;
            if (7 == len && "trigger" == fieldName) return FieldIndex::Trigger;
            if (13 == len)
            {
                if ("time_in_force" == fieldName) return FieldIndex::TimeInForce;
                if ("trigger_price" == fieldName) return FieldIndex::TriggerPrice;
            }
            if (14 == len && "trigger_offset" == fieldName) return FieldIndex::TriggerOffset;
            if (23 == len && "trigger_fill_condition" == fieldName) return FieldIndex::TriggerFillCondition;
            break;
        case 'v':
            if (11 == len && "valid_until" == fieldName) return FieldIndex::ValidUntil;
            break;
        }

        return FieldIndex::None;
    }

    template class CsvParser<DeribitTraits>;
}