#include "Scheduler.h"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

///////////////////////////////////////////////////////////////////////////////
//
// AlertManager
//
///////////////////////////////////////////////////////////////////////////////

void AlertManager::notify(const Service& service,
    const PolicyResult& result) const
{
    std::cout
        << "\n=========================================\n"
        << "ALERT\n"
        << "-----------------------------------------\n"
        << "Service  : " << service.name() << '\n'
        << "Category : ";

    switch (service.category())
    {
    case ServiceCategory::Critical:
        std::cout << "Critical";
        break;

    case ServiceCategory::Major:
        std::cout << "Major";
        break;

    case ServiceCategory::Minor:
        std::cout << "Minor";
        break;
    }

    std::cout
        << "\nReason   : "
        << result.reason
        << "\nAction   : Notify Manager"
        << "\n=========================================\n";
}

///////////////////////////////////////////////////////////////////////////////
//
// ServiceScheduler
//
///////////////////////////////////////////////////////////////////////////////

ServiceScheduler::ServiceScheduler()
    :
    m_criticalPolicy(
        {
            &m_restartAction,
            &m_restartAction,
            &m_restartAction,
            &m_stopAction,
            &m_disableAction
        }),
    m_majorPolicy(
        {
            &m_restartAction,
            &m_restartAction,
            &m_stopAction
        }),
    m_minorPolicy(
        {
            &m_restartAction,
            &m_disableAction
        })
{}

///////////////////////////////////////////////////////////////////////////////

IRecoveryPolicy&
ServiceScheduler::getRecoveryPolicy(
    ServiceCategory category)
{
    switch (category)
    {
    case ServiceCategory::Critical:
        return m_criticalPolicy;

    case ServiceCategory::Major:
        return m_majorPolicy;

    case ServiceCategory::Minor:
        return m_minorPolicy;
    }

    throw std::logic_error(
        "Unsupported service category.");
}

///////////////////////////////////////////////////////////////////////////////

void ServiceScheduler::registerService(
    std::string serviceName,
    ServiceCategory category)
{
    auto service =
        std::make_unique<Service>(
            std::move(serviceName),
            category,
            getRecoveryPolicy(category));

    m_registry.registerService(
        std::move(service));
}

///////////////////////////////////////////////////////////////////////////////

void ServiceScheduler::notifyFailure(
    const FailureEvent& event)
{
    auto* service =
        m_registry.findService(event.serviceName);

    if (!service)
    {
        std::cout
            << "[ERROR] Unknown Service : "
            << event.serviceName
            << '\n';

        return;
    }

    //-------------------------------------------------------------
    // Step 1
    // Apply Global Policy
    //-------------------------------------------------------------

    const auto result =
        m_globalPolicy.evaluate(event);

    if (result.decision ==
        PolicyDecision::Reject)
    {
        m_alertManager.notify(
            *service,
            result);

        return;
    }

    //-------------------------------------------------------------
    // Step 2
    // Update Service Statistics
    //-------------------------------------------------------------

    service->setState(
        ServiceState::Recovering);

    service->incrementFailureCount();

    //-------------------------------------------------------------
    // Step 3
    // Determine Recovery Action
    //-------------------------------------------------------------

    const auto& action =
        service->policy().nextAction(
            service->recoveryLevel());

    //-------------------------------------------------------------
    // Step 4
    // Execute Recovery Action
    //-------------------------------------------------------------

    action.execute(*service);

    //-------------------------------------------------------------
    // Step 5
    // Update Runtime State
    //-------------------------------------------------------------

    service->setLastAction(
        action.type());

    service->incrementRecoveryLevel();

    service->setState(
        ServiceState::Running);
}

///////////////////////////////////////////////////////////////////////////////

const Service*
ServiceScheduler::getServiceStatus(
    const std::string& serviceName) const
{
    return m_registry.findService(
        serviceName);
}