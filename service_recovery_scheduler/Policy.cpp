#include "Policy.h"

#include <stdexcept>
#include <utility>

#include "Action.h"

///////////////////////////////////////////////////////////////////////////////
// GlobalPolicy
///////////////////////////////////////////////////////////////////////////////

PolicyResult GlobalPolicy::evaluate(const FailureEvent& event) const
{
    switch (event.signal)
    {
    case FailureSignal::SigSegv:
        return
        {
            PolicyDecision::Reject,
            "Recovery is not allowed for SIGSEGV."
        };

    case FailureSignal::SigIll:
        return
        {
            PolicyDecision::Reject,
            "Recovery is not allowed for SIGILL."
        };

    default:
        return
        {
            PolicyDecision::Allow,
            "Recovery allowed."
        };
    }
}

///////////////////////////////////////////////////////////////////////////////
// RecoveryPolicy
///////////////////////////////////////////////////////////////////////////////

RecoveryPolicy::RecoveryPolicy( std::vector<const IRecoveryAction*> actions) : m_actions(std::move(actions))
{
    if (m_actions.empty())
    {
        throw std::invalid_argument( "Recovery action sequence cannot be empty.");
    }
}

const IRecoveryAction& RecoveryPolicy::nextAction(int recoveryLevel) const
{
    //
    // Clamp the recovery level to the last configured action.
    // Once the maximum recovery level is reached, the last
    // recovery action is returned for all subsequent failures.
    //

    if (recoveryLevel < 0)
    {
        recoveryLevel = 0;
    }

    std::size_t index = static_cast<std::size_t>(recoveryLevel);

    if (index >= m_actions.size())
    {
        index = m_actions.size() - 1;
    }

    return *m_actions[index];
}