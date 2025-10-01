#pragma once

#include "FSHR_DERIBIT_Macro.h"
#include "FSHR_DERIBIT_Enums.h"
#include "FSHR_DERIBIT_ProtocolTraits.h"
#include "FSHR_DERIBIT_Order.h"

#include <string>
#include <vector>
#include <chrono>
#include <cstdint>

namespace fischer::deribit
{
    template<typename Traits = DeribitTraits>
    class OrderProcessor
    {
    public:
        using OrderType = Order<Traits>;
        using MessageIdType = typename Traits::MessageIdType;
        using SizeType = typename Traits::SizeType;

        OrderProcessor();
        RULE_OF_FIVE_NONMOVABLE(OrderProcessor);

        void ProcessOrders(const std::string& inputFile, const std::string& outputFile);

        SizeType GetProcessedOrderCount() const { return m_ProcessedOrderCount; }
        std::chrono::microseconds GetTotalProcessingTime() const { return m_TotalProcessingTime; }
        std::chrono::microseconds GetParseTime() const { return m_ParseTime; }
        std::chrono::microseconds GetBuildTime() const { return m_BuildTime; }
        std::chrono::microseconds GetWriteTime() const { return m_WriteTime; }

    protected:
        std::vector<OrderType> ParseOrderFile(const std::string& filename);
        std::string BuildJsonPayload(const std::vector<OrderType>& orders);
        void WriteOutputFile(const std::string& filename, const std::string& content);

    private:
        SizeType m_ProcessedOrderCount;
        std::chrono::microseconds m_TotalProcessingTime;
        std::chrono::microseconds m_ParseTime;
        std::chrono::microseconds m_BuildTime;
        std::chrono::microseconds m_WriteTime;
        MessageIdType m_MessageIdCounter;
        ProcessingStatus m_Status;
    };
}

#include <FSHR_DERIBIT_OrderProcessor.hxx>