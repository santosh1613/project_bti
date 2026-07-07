#include <gtest/gtest.h>

#include "Common.h"
#include "Scheduler.h"

TEST(SchedulerTest, UnknownServiceIsIgnored)
{
    ServiceScheduler scheduler;

    // Should not throw for an unregistered service.
    EXPECT_NO_THROW(
        scheduler.notifyFailure({ "UnknownService", FailureSignal::SigTerm }));

    EXPECT_EQ(scheduler.getServiceStatus("UnknownService"), nullptr);
}

TEST(SchedulerTest, RecoverableFailureUpdatesService)
{
    ServiceScheduler scheduler;
    scheduler.registerService("Navigation", ServiceCategory::Critical);

    scheduler.notifyFailure({ "Navigation", FailureSignal::SigTerm });

    const Service* service = scheduler.getServiceStatus("Navigation");
    ASSERT_NE(service, nullptr);
    EXPECT_EQ(service->failureCount(), 1);
    EXPECT_EQ(service->recoveryLevel(), 1);
    EXPECT_EQ(service->lastAction(), RecoveryActionType::Restart);
}

TEST(SchedulerTest, EscalatesThroughRecoveryLevels)
{
    ServiceScheduler scheduler;
    scheduler.registerService("Navigation", ServiceCategory::Critical);

    for (int i = 0; i < 5; ++i)
    {
        scheduler.notifyFailure({ "Navigation", FailureSignal::SigTerm });
    }

    const Service* service = scheduler.getServiceStatus("Navigation");
    ASSERT_NE(service, nullptr);
    EXPECT_EQ(service->failureCount(), 5);
    // Critical policy: restart, restart, restart, stop, disable.
    EXPECT_EQ(service->lastAction(), RecoveryActionType::Disable);
}

TEST(SchedulerTest, RejectedSignalDoesNotChangeStats)
{
    ServiceScheduler scheduler;
    scheduler.registerService("Navigation", ServiceCategory::Critical);

    scheduler.notifyFailure({ "Navigation", FailureSignal::SigSegv });

    const Service* service = scheduler.getServiceStatus("Navigation");
    ASSERT_NE(service, nullptr);
    EXPECT_EQ(service->failureCount(), 0);
    EXPECT_EQ(service->recoveryLevel(), 0);
    EXPECT_EQ(service->lastAction(), RecoveryActionType::None);
}
