#pragma once

#include <string>
#include<string_view>

enum class ServiceCategory
{
    Critical,
    Major,
    Minor
};

enum class ServiceState
{
    Running,
    Recovering,
    Disabled
};

enum class RecoveryActionType
{
    None,
    Restart,
    Stop,
    Disable
};

enum class FailureSignal
{
    Unknown,
    SigTerm,
    SigAbrt,
    SigSegv,
    SigIll
};

enum class PolicyDecision
{
    Allow,
    Reject
};

struct FailureEvent
{
    std::string serviceName;
    FailureSignal signal{ FailureSignal::Unknown };
};

struct PolicyResult
{
    PolicyDecision decision{ PolicyDecision::Allow };
    std::string reason;
};


inline std::string_view toString(ServiceCategory category)
{
    switch (category)
    {
    case ServiceCategory::Critical: return "Critical";
    case ServiceCategory::Major:    return "Major";
    case ServiceCategory::Minor:    return "Minor";
    }

    return "Unknown";
}

inline std::string_view toString(ServiceState state)
{
    switch (state)
    {
    case ServiceState::Running:     return "Running";
    case ServiceState::Recovering:  return "Recovering";
    case ServiceState::Disabled:    return "Disabled";
    }

    return "Unknown";
}

inline std::string_view toString(RecoveryActionType action)
{
    switch (action)
    {
    case RecoveryActionType::None:     return "None";
    case RecoveryActionType::Restart:  return "Restart";
    case RecoveryActionType::Stop:     return "Stop";
    case RecoveryActionType::Disable:  return "Disable";
    }

    return "Unknown";
}

inline std::string_view toString(FailureSignal signal)
{
    switch (signal)
    {
    case FailureSignal::Unknown: return "Unknown";
    case FailureSignal::SigTerm: return "SIGTERM";
    case FailureSignal::SigAbrt: return "SIGABRT";
    case FailureSignal::SigSegv: return "SIGSEGV";
    case FailureSignal::SigIll:  return "SIGILL";
    }

    return "Unknown";
}