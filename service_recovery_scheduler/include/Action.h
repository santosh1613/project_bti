#pragma once

#include "Common.h"

class Service;

class IRecoveryAction
{
public:

    explicit IRecoveryAction(RecoveryActionType type);

    virtual ~IRecoveryAction() = default;

    RecoveryActionType type() const noexcept;

    virtual void execute(Service& service) const = 0;

private:

    RecoveryActionType m_type;
};

///////////////////////////////////////////////////////////////////////////////

class RestartAction final : public IRecoveryAction
{
public:

    RestartAction();

    void execute(Service& service) const override;
};

///////////////////////////////////////////////////////////////////////////////

class StopAction final : public IRecoveryAction
{
public:

    StopAction();

    void execute(Service& service) const override;
};

///////////////////////////////////////////////////////////////////////////////

class DisableAction final : public IRecoveryAction
{
public:

    DisableAction();

    void execute(Service& service) const override;
};