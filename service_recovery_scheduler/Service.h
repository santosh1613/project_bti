#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "Common.h"

class IRecoveryPolicy;

class Service
{
public:
    Service(std::string name, ServiceCategory category, IRecoveryPolicy& policy);
    const std::string& name() const noexcept;
    ServiceCategory category() const noexcept;
    ServiceState state() const noexcept;
    void setState(ServiceState state);
    int recoveryLevel() const noexcept;
    void incrementRecoveryLevel();
    int failureCount() const noexcept;
    void incrementFailureCount();
    RecoveryActionType lastAction() const noexcept;
    void setLastAction(RecoveryActionType action);
    IRecoveryPolicy&  policy() noexcept;

private:
    std::string m_name;
    ServiceCategory m_category;
    ServiceState m_state{ ServiceState::Running };
    int m_recoveryLevel{ 0 };
    int m_failureCount{ 0 };
    RecoveryActionType m_lastAction{ RecoveryActionType::None };
    IRecoveryPolicy& m_policy;
};

class ServiceRegistry
{
public:

    void registerService(std::unique_ptr<Service> service);
    Service* findService(const std::string& serviceName);
    const Service* findService(const std::string& serviceName) const;

private:
    std::unordered_map< std::string, std::unique_ptr<Service>> m_services;
};