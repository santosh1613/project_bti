#include <iostream>
#include "Scheduler.h"


void printServiceStatus(const ServiceScheduler& scheduler, const std::string& serviceName) {
    const Service* service = scheduler.getServiceStatus(serviceName);
    if (!service)
    {
        std::cout
            << "Service not found : "
            << serviceName
            << '\n';
        return;
    }

    std::cout
        << "\n=========================================\n"
        << "Service Name   : " << service->name() << '\n'
        << "Category       : " << toString(service->category()) << '\n'
        << "State          : " << toString(service->state()) << '\n'
        << "Recovery Level : " << service->recoveryLevel() << '\n'
        << "Failure Count  : " << service->failureCount() << '\n'
        << "Last Action    : " << toString(service->lastAction()) << '\n'
        << "=========================================\n";
}

void simulateFailures(ServiceScheduler& scheduler, const std::string& serviceName, FailureSignal signal, int count) {
    for (int i = 0; i < count; ++i)
    {
        scheduler.notifyFailure(
            {
                serviceName,
                signal
            });
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    ServiceScheduler scheduler;

    //------------------------------------------------------------
    // Register Services
    //------------------------------------------------------------

    scheduler.registerService(
        "Database",
        ServiceCategory::Critical);

    scheduler.registerService(
        "Cache",
        ServiceCategory::Major);

    scheduler.registerService(
        "Logger",
        ServiceCategory::Minor);

    //------------------------------------------------------------
    // Critical Service
    //------------------------------------------------------------

    std::cout
        << "\n========== DATABASE ==========\n";

    simulateFailures(
        scheduler,
        "Database",
        FailureSignal::SigTerm,
        5);

    printServiceStatus(
        scheduler,
        "Database");

    //------------------------------------------------------------
    // Major Service
    //------------------------------------------------------------

    std::cout
        << "\n========== CACHE ==========\n";

    simulateFailures(
        scheduler,
        "Cache",
        FailureSignal::SigAbrt,
        3);

    printServiceStatus(
        scheduler,
        "Cache");

    //------------------------------------------------------------
    // Minor Service
    //------------------------------------------------------------

    std::cout
        << "\n========== LOGGER ==========\n";

    simulateFailures(
        scheduler,
        "Logger",
        FailureSignal::SigTerm,
        2);

    printServiceStatus(
        scheduler,
        "Logger");

    //------------------------------------------------------------
    // Global Policy Test
    //------------------------------------------------------------

    std::cout
        << "\n========== GLOBAL POLICY ==========\n";

    scheduler.notifyFailure(
        {
            "Database",
            FailureSignal::SigSegv
        });

    scheduler.notifyFailure(
        {
            "Cache",
            FailureSignal::SigIll
        });

    //------------------------------------------------------------
    // Unknown Service
    //------------------------------------------------------------

    std::cout
        << "\n========== UNKNOWN SERVICE ==========\n";

    scheduler.notifyFailure(
        {
            "UnknownService",
            FailureSignal::SigTerm
        });

    return 0;
}