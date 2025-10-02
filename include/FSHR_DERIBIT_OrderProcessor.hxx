#include "FSHR_DERIBIT_OrderProcessor.h"
#include "FSHR_DERIBIT_CSVParser.h"
#include "FSHR_DERIBIT_JSONBuilder.h"
#include "FSHR_DERIBIT_Logger.h"
#include "FSHR_DERIBIT_Constants.h"

#include <fstream>
#include <iostream>
#include <stdexcept>

namespace fischer::deribit
{
    template<typename Traits>
    OrderProcessor<Traits>::OrderProcessor()
        : m_ProcessedOrderCount{0}
        , m_TotalProcessingTime{0}
        , m_ParseTime{0}
        , m_BuildTime{0}
        , m_WriteTime{0}
        , m_MessageIdCounter{Traits::InitialMessageId}
        , m_Status{ProcessingStatus::Idle}
    {
        LOG_DEBUG("OrderProcessor initialized with message ID:", m_MessageIdCounter);
    }

    template<typename Traits>
    void OrderProcessor<Traits>::ProcessOrders(const std::string& inputFile,
                                               const std::string& outputFile)
    {
        LOG_INFO("Processing orders from", inputFile, "to", outputFile);
        m_Status = ProcessingStatus::Parsing;

        auto startTime = std::chrono::high_resolution_clock::now();

        try
        {
            // Parse CSV input
            auto parseStart = std::chrono::high_resolution_clock::now();
            std::vector<OrderType> orders = ParseOrderFile(inputFile);
            auto parseEnd = std::chrono::high_resolution_clock::now();

            LOG_INFO("Parsed", orders.size(), "orders");

            // Build JSON output
            m_Status = ProcessingStatus::Building;
            auto buildStart = std::chrono::high_resolution_clock::now();
            std::string jsonOutput = BuildJsonPayload(orders);
            auto buildEnd = std::chrono::high_resolution_clock::now();

            LOG_DEBUG("Built JSON payload with size:", jsonOutput.size());

            // Write output file
            m_Status = ProcessingStatus::Writing;
            auto writeStart = std::chrono::high_resolution_clock::now();
            WriteOutputFile(outputFile, jsonOutput);
            auto writeEnd = std::chrono::high_resolution_clock::now();

            // Calculate metrics
            m_ProcessedOrderCount = static_cast<SizeType>(orders.size());
            m_ParseTime = std::chrono::duration_cast<std::chrono::microseconds>(
                parseEnd - parseStart);
            m_BuildTime = std::chrono::duration_cast<std::chrono::microseconds>(
                buildEnd - buildStart);
            m_WriteTime = std::chrono::duration_cast<std::chrono::microseconds>(
                writeEnd - writeStart);
            m_TotalProcessingTime = std::chrono::duration_cast<std::chrono::microseconds>(
                writeEnd - startTime);

            m_Status = ProcessingStatus::Complete;

            LOG_INFO("Processing complete. Orders:", m_ProcessedOrderCount,
                    "Total time:", m_TotalProcessingTime.count(), "μs");
        }
        catch (const std::exception& e)
        {
            m_Status = ProcessingStatus::Failed;
            LOG_ERROR("Processing failed:", e.what());
            throw;
        }
    }

    template<typename Traits>
    std::vector<typename OrderProcessor<Traits>::OrderType>
    OrderProcessor<Traits>::ParseOrderFile(const std::string& filename)
    {
        CsvParser<Traits> parser;

        if (false == parser.LoadFile(filename))
        {
            LOG_ERROR("Failed to load file:", filename);
            throw std::runtime_error("Failed to load CSV file");
        }

        LOG_DEBUG("File loaded. Size:", parser.GetFileSize(), "bytes");
        return parser.ParseOrders();
    }

    template<typename Traits>
    std::string OrderProcessor<Traits>::BuildJsonPayload(const std::vector<OrderType>& orders)
    {
        JsonBuilder<Traits> builder;

        for (const auto& order : orders)
        {
            builder.BuildOrderMessage(order, m_MessageIdCounter++);
        }

        return builder.GetResult();
    }

    template<typename Traits>
    void OrderProcessor<Traits>::WriteOutputFile(const std::string& filename,
                                                 const std::string& content)
    {
        std::ofstream file(filename, std::ios::binary);

        if (false == file.is_open())
        {
            LOG_ERROR("Failed to open output file:", filename);
            throw std::runtime_error("Failed to open output file");
        }

        file.write(content.data(), static_cast<std::streamsize>(content.size()));

        if (false == file.good())
        {
            LOG_ERROR("Failed to write output file:", filename);
        }

        LOG_INFO("Output written successfully:", filename);
    }

    template class OrderProcessor<DeribitTraits>;
}