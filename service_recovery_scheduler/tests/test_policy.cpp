#include <gtest/gtest.h>

#include "Action.h"
#include "Policy.h"

TEST(GlobalPolicyTest, AllowsRecoverableSignals)
{
    GlobalPolicy policy;

    const auto result =
        policy.evaluate({ "svc", FailureSignal::SigTerm });

    EXPECT_EQ(result.decision, PolicyDecision::Allow);
}

TEST(GlobalPolicyTest, RejectsSigSegv)
{
    GlobalPolicy policy;

    const auto result =
        policy.evaluate({ "svc", FailureSignal::SigSegv });

    EXPECT_EQ(result.decision, PolicyDecision::Reject);
}

TEST(GlobalPolicyTest, RejectsSigIll)
{
    GlobalPolicy policy;

    const auto result =
        policy.evaluate({ "svc", FailureSignal::SigIll });

    EXPECT_EQ(result.decision, PolicyDecision::Reject);
}

TEST(RecoveryPolicyTest, EmptySequenceThrows)
{
    EXPECT_THROW(RecoveryPolicy({}), std::invalid_argument);
}

TEST(RecoveryPolicyTest, ReturnsActionForEachLevel)
{
    RestartAction restart;
    StopAction stop;
    DisableAction disable;

    RecoveryPolicy policy({ &restart, &stop, &disable });

    EXPECT_EQ(policy.nextAction(0).type(), RecoveryActionType::Restart);
    EXPECT_EQ(policy.nextAction(1).type(), RecoveryActionType::Stop);
    EXPECT_EQ(policy.nextAction(2).type(), RecoveryActionType::Disable);
}

TEST(RecoveryPolicyTest, ClampsLevelToLastAction)
{
    RestartAction restart;
    StopAction stop;

    RecoveryPolicy policy({ &restart, &stop });

    EXPECT_EQ(policy.nextAction(99).type(), RecoveryActionType::Stop);
}

TEST(RecoveryPolicyTest, ClampsNegativeLevelToFirstAction)
{
    RestartAction restart;
    StopAction stop;

    RecoveryPolicy policy({ &restart, &stop });

    EXPECT_EQ(policy.nextAction(-5).type(), RecoveryActionType::Restart);
}
