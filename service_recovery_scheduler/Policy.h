#pragma once

#include <vector>

#include "Common.h"

class Service;
class IRecoveryAction;

class IGlobalPolicy
{
public:
    virtual ~IGlobalPolicy() = default;
    virtual PolicyResult evaluate(const FailureEvent& event) const = 0;
};

class GlobalPolicy final : public IGlobalPolicy
{
public:
    PolicyResult
        evaluate(const FailureEvent& event) const override;
};

class IRecoveryPolicy
{
public:
    virtual ~IRecoveryPolicy() = default;
    virtual const IRecoveryAction& nextAction(int recoveryLevel) const = 0;
};

class RecoveryPolicy final : public IRecoveryPolicy
{
public:
    explicit RecoveryPolicy( std::vector<const IRecoveryAction*> actions);
    const IRecoveryAction&  nextAction(int recoveryLevel) const override;
private:
    std::vector<const IRecoveryAction*> m_actions;
};