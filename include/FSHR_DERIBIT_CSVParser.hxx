#include "FSHR_DERIBIT_CSVParser.h"
#include "FSHR_DERIBIT_Logger.h"
#include "FSHR_DERIBIT_Constants.h"

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

        const char* current = start;
        while (current < end)
        {
            const char* comma = std::strchr(current, FieldDelimiter);
            if (nullptr == comma || comma > end)
            {
                comma = end;
            }

            m_Headers.emplace_back(current, static_cast<SizeType>(comma - current));
            current = comma + 1;
        }

        LOG_DEBUG("Parsed", m_Headers.size(), "CSV headers");
    }

    template<typename Traits>
    bool CsvParser<Traits>::ParseDataLine(const char* start, const char* end, OrderType& order)
    {
        const char* current = start;
        SizeType fieldIndex = 0;

        while (current < end && fieldIndex < m_Headers.size())
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

            if (0 < length)
            {
                AssignFieldValue(order, m_Headers[fieldIndex], current, length);
            }

            current = comma + 1;
            fieldIndex++;
        }

        return true;
    }

    template<typename Traits>
    void CsvParser<Traits>::AssignFieldValue(OrderType& order, std::string_view fieldName,
                                             const char* value, SizeType length)
    {
        if (FieldId == fieldName)
        {
            std::from_chars(value, value + length, order.m_Id);
        }
        else if (FieldDirection == fieldName)
        {
            order.m_Direction.assign(value, length);
        }
        else if (FieldAmount == fieldName)
        {
            order.m_Amount = std::strtod(value, nullptr);
        }
        else if (FieldContracts == fieldName)
        {
            order.m_Contracts = std::strtod(value, nullptr);
        }
        else if (FieldInstrumentName == fieldName)
        {
            order.m_InstrumentName.assign(value, length);
        }
        else if (FieldLabel == fieldName)
        {
            order.m_Label.assign(value, length);
        }
        else if (FieldType == fieldName)
        {
            order.m_Type.assign(value, length);
        }
        else if (FieldPrice == fieldName)
        {
            order.m_Price = std::strtod(value, nullptr);
        }
        else if (TimeInForce == fieldName)
        {
            order.m_TimeInForce = std::string(value, length);
        }
        else if (PostOnly == fieldName)
        {
            order.m_PostOnly = (BooleanTrue1 == value[0] ||
                               BooleanTrue2 == value[0] ||
                               BooleanTrue3 == value[0]);
        }
        else if (RejectPostOnly == fieldName)
        {
            order.m_RejectPostOnly = (BooleanTrue1 == value[0] ||
                                      BooleanTrue2 == value[0] ||
                                      BooleanTrue3 == value[0]);
        }
        else if (ReduceOnly == fieldName)
        {
            order.m_ReduceOnly = (BooleanTrue1 == value[0] ||
                                  BooleanTrue2 == value[0] ||
                                  BooleanTrue3 == value[0]);
        }
        else if (TriggerPrice == fieldName)
        {
            order.m_TriggerPrice = std::strtod(value, nullptr);
        }
        else if (TriggerOffset == fieldName)
        {
            order.m_TriggerOffset = std::strtod(value, nullptr);
        }
        else if (Trigger == fieldName)
        {
            order.m_Trigger = std::string(value, length);
        }
        else if (DisplayAmount == fieldName)
        {
            order.m_DisplayAmount = std::strtod(value, nullptr);
        }
        else if (Advanced == fieldName)
        {
            order.m_Advanced = std::string(value, length);
        }
        else if (Mmp == fieldName)
        {
            order.m_Mmp = (BooleanTrue1 == value[0] ||
                          BooleanTrue2 == value[0] ||
                          BooleanTrue3 == value[0]);
        }
        else if (ValidUntil == fieldName)
        {
            int64_t validUntil = 0;
            std::from_chars(value, value + length, validUntil);
            order.m_ValidUntil = validUntil;
        }
        else if (LinkedOrderType == fieldName)
        {
            order.m_LinkedOrderType = std::string(value, length);
        }
        else if (TriggerFillCondition == fieldName)
        {
            order.m_TriggerFillCondition = std::string(value, length);
        }
    }

    // Explicit template instantiation
    template class CsvParser<DeribitTraits>;
}