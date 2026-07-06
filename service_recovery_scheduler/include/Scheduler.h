#pragma once

#include <string>

#include "Action.h"
#include "Policy.h"
#include "Service.h"

///////////////////////////////////////////////////////////////////////////////
//
// AlertManager
//
///////////////////////////////////////////////////////////////////////////////

class AlertManager
{
public:

    void notify(const Service& service,
        const PolicyResult& result) const;
};

///////////////////////////////////////////////////////////////////////////////
//
// ServiceScheduler
//
///////////////////////////////////////////////////////////////////////////////

class ServiceScheduler
{
public:

    ServiceScheduler();

    ~ServiceScheduler() = default;

    ServiceScheduler(const ServiceScheduler&) = delete;
    ServiceScheduler& operator=(const ServiceScheduler&) = delete;

    ServiceScheduler(ServiceScheduler&&) = default;
    ServiceScheduler& operator=(ServiceScheduler&&) = default;

public:

    void registerService(std::string serviceName,
        ServiceCategory category);

    void notifyFailure(const FailureEvent& event);

    const Service*
        getServiceStatus(const std::string& serviceName) const;

private:

    IRecoveryPolicy&
        getRecoveryPolicy(ServiceCategory category);

private:

    //-------------------------------------------------------------
    // Registry
    //-------------------------------------------------------------

    ServiceRegistry m_registry;

    //-------------------------------------------------------------
    // Alerting
    //-------------------------------------------------------------

    AlertManager m_alertManager;

    //-------------------------------------------------------------
    // Global Policy
    //-------------------------------------------------------------

    GlobalPolicy m_globalPolicy;

    //-------------------------------------------------------------
    // Recovery Actions
    //
    // IMPORTANT:
    // Actions MUST be constructed before RecoveryPolicy because
    // policies keep pointers to these action objects.
    //-------------------------------------------------------------

    RestartAction m_restartAction;

    StopAction m_stopAction;

    DisableAction m_disableAction;

    //-------------------------------------------------------------
    // Category Policies
    //-------------------------------------------------------------

    RecoveryPolicy m_criticalPolicy;

    RecoveryPolicy m_majorPolicy;

    RecoveryPolicy m_minorPolicy;
};