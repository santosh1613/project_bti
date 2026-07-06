#include "Action.h"

#include <iostream>

#include "Service.h"

///////////////////////////////////////////////////////////////////////////////
// IRecoveryAction
///////////////////////////////////////////////////////////////////////////////

IRecoveryAction::IRecoveryAction(RecoveryActionType type) : m_type(type) {}

RecoveryActionType IRecoveryAction::type() const noexcept
{
    return m_type;
}

///////////////////////////////////////////////////////////////////////////////
// RestartAction
///////////////////////////////////////////////////////////////////////////////

RestartAction::RestartAction() : IRecoveryAction(RecoveryActionType::Restart) {}

void RestartAction::execute(Service& service) const
{
    std::cout
        << "[ACTION] Restarting Service : "
        << service.name()
        << '\n';
}

///////////////////////////////////////////////////////////////////////////////
// StopAction
///////////////////////////////////////////////////////////////////////////////

StopAction::StopAction() : IRecoveryAction(RecoveryActionType::Stop) {}

void StopAction::execute(Service& service) const
{
    std::cout
        << "[ACTION] Stopping Service : "
        << service.name()
        << '\n';
}

///////////////////////////////////////////////////////////////////////////////
// DisableAction
///////////////////////////////////////////////////////////////////////////////

DisableAction::DisableAction()
    : IRecoveryAction(RecoveryActionType::Disable)
{}

void DisableAction::execute(Service& service) const
{
    std::cout
        << "[ACTION] Disabling Service : "
        << service.name()
        << '\n';
}