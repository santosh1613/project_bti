#include <gtest/gtest.h>

#include <stdexcept>

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

TEST(SchedulerTest, MajorServiceEscalatesThroughRecoveryLevels)
{
    ServiceScheduler scheduler;
    scheduler.registerService("Radio", ServiceCategory::Major);

    // Major policy: restart, restart, stop.
    scheduler.notifyFailure({ "Radio", FailureSignal::SigTerm });
    const Service* service = scheduler.getServiceStatus("Radio");
    ASSERT_NE(service, nullptr);
    EXPECT_EQ(service->lastAction(), RecoveryActionType::Restart);

    scheduler.notifyFailure({ "Radio", FailureSignal::SigTerm });
    EXPECT_EQ(service->lastAction(), RecoveryActionType::Restart);

    scheduler.notifyFailure({ "Radio", FailureSignal::SigTerm });
    EXPECT_EQ(service->lastAction(), RecoveryActionType::Stop);

    EXPECT_EQ(service->failureCount(), 3);
    EXPECT_EQ(service->recoveryLevel(), 3);
}

TEST(SchedulerTest, MinorServiceEscalatesThroughRecoveryLevels)
{
    ServiceScheduler scheduler;
    scheduler.registerService("Logger", ServiceCategory::Minor);

    // Minor policy: restart, disable.
    scheduler.notifyFailure({ "Logger", FailureSignal::SigTerm });
    const Service* service = scheduler.getServiceStatus("Logger");
    ASSERT_NE(service, nullptr);
    EXPECT_EQ(service->lastAction(), RecoveryActionType::Restart);

    scheduler.notifyFailure({ "Logger", FailureSignal::SigTerm });
    EXPECT_EQ(service->lastAction(), RecoveryActionType::Disable);

    EXPECT_EQ(service->failureCount(), 2);
    EXPECT_EQ(service->recoveryLevel(), 2);
}

TEST(SchedulerTest, RejectedSignalAlertsMajorService)
{
    ServiceScheduler scheduler;
    scheduler.registerService("Radio", ServiceCategory::Major);

    EXPECT_NO_THROW(
        scheduler.notifyFailure({ "Radio", FailureSignal::SigIll }));

    const Service* service = scheduler.getServiceStatus("Radio");
    ASSERT_NE(service, nullptr);
    EXPECT_EQ(service->failureCount(), 0);
    EXPECT_EQ(service->lastAction(), RecoveryActionType::None);
}

TEST(SchedulerTest, RejectedSignalAlertsMinorService)
{
    ServiceScheduler scheduler;
    scheduler.registerService("Logger", ServiceCategory::Minor);

    EXPECT_NO_THROW(
        scheduler.notifyFailure({ "Logger", FailureSignal::SigSegv }));

    const Service* service = scheduler.getServiceStatus("Logger");
    ASSERT_NE(service, nullptr);
    EXPECT_EQ(service->failureCount(), 0);
    EXPECT_EQ(service->lastAction(), RecoveryActionType::None);
}

TEST(SchedulerTest, UnsupportedCategoryThrows)
{
    ServiceScheduler scheduler;

    EXPECT_THROW(
        scheduler.registerService(
            "Invalid", static_cast<ServiceCategory>(99)),
        std::logic_error);
}
