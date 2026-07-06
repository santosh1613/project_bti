#include "Service.h"

#include <stdexcept>
#include <utility>

///////////////////////////////////////////////////////////////////////////////
// Service
///////////////////////////////////////////////////////////////////////////////

Service::Service(std::string name, ServiceCategory category, IRecoveryPolicy& policy)
    : m_name(std::move(name)), m_category(category), m_policy(policy) {}

const std::string& Service::name() const noexcept {
    return m_name;
}

ServiceCategory Service::category() const noexcept {
    return m_category;
}

ServiceState Service::state() const noexcept {
    return m_state;
}

void Service::setState(ServiceState state) {
    m_state = state;
}

int Service::recoveryLevel() const noexcept {
    return m_recoveryLevel;
}

void Service::incrementRecoveryLevel() {
    ++m_recoveryLevel;
}

int Service::failureCount() const noexcept {
    return m_failureCount;
}

void Service::incrementFailureCount() {
    ++m_failureCount;
}

RecoveryActionType Service::lastAction() const noexcept {
    return m_lastAction;
}

void Service::setLastAction(RecoveryActionType action) {
    m_lastAction = action;
}

IRecoveryPolicy& Service::policy() noexcept {
    return m_policy;
}

///////////////////////////////////////////////////////////////////////////////
// ServiceRegistry
///////////////////////////////////////////////////////////////////////////////

void ServiceRegistry::registerService(
    std::unique_ptr<Service> service)
{
    if (service == nullptr)
    {
        throw std::invalid_argument(
            "Cannot register nullptr service.");
    }

    const auto serviceName = service->name();

    auto [iter, inserted] =
        m_services.emplace(serviceName, std::move(service));

    if (!inserted)
    {
        throw std::runtime_error(
            "Service already registered : " + serviceName);
    }
}

Service*
ServiceRegistry::findService(
    const std::string& serviceName)
{
    auto iter = m_services.find(serviceName);

    if (iter == m_services.end())
    {
        return nullptr;
    }

    return iter->second.get();
}

const Service*
ServiceRegistry::findService(
    const std::string& serviceName) const
{
    auto iter = m_services.find(serviceName);

    if (iter == m_services.end())
    {
        return nullptr;
    }

    return iter->second.get();
}