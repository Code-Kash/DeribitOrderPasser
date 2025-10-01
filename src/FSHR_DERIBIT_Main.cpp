#include "FSHR_DERIBIT_OrderProcessor.h"
#include "FSHR_DERIBIT_Logger.h"
#include "FSHR_DERIBIT_Constants.h"

#include <iomanip>
#include <stdexcept>

using namespace fischer::deribit;

void PrintPerformanceMetrics(const OrderProcessor<DeribitTraits>& processor)
{
    LOG_INFO("Performance Metrics:");
    LOG_INFO("  Orders processed:", processor.GetProcessedOrderCount());
    LOG_INFO("  Total time:", processor.GetTotalProcessingTime().count(), "μs");
    LOG_INFO("  Parse time:", processor.GetParseTime().count(), "μs");
    LOG_INFO("  Build time:", processor.GetBuildTime().count(), "μs");
    LOG_INFO("  Write time:", processor.GetWriteTime().count(), "μs");

    double throughput = 0.0;
    if (0 < processor.GetTotalProcessingTime().count())
    {
        throughput = static_cast<double>(processor.GetProcessedOrderCount()) *
                    MicrosecondsToSeconds /
                    static_cast<double>(processor.GetTotalProcessingTime().count());
    }

    LOG_INFO("  Throughput:", static_cast<int>(throughput), "orders/sec");
}

int main(int argc, char* argv[])
{
    try
    {
        // Initialize logger
        std::string logFile = std::string(DefaultLogFile);
        Logger<DeribitTraits>::GetInstance().Initialize(logFile, LogLevel::Info, true, true);

        LOG_INFO("Fischer Framework - Deribit Order Processor");
        LOG_INFO("============================================");

        std::string inputFile(DefaultInputFile);
        std::string outputFile(DefaultOutputFile);

        if (1 < argc)
        {
            inputFile = argv[1];
        }

        if (2 < argc)
        {
            outputFile = argv[2];
        }

        LOG_INFO("Input:", inputFile);
        LOG_INFO("Output:", outputFile);

        OrderProcessor<DeribitTraits> processor;
        processor.ProcessOrders(inputFile, outputFile);

        PrintPerformanceMetrics(processor);

        LOG_INFO("Processing complete!");

        // Shutdown logger
        Logger<DeribitTraits>::GetInstance().Shutdown();
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Error:", e.what());
        return 1;
    }

    return 0;
}