#include <gtest/gtest.h>

#include <memory>

#include "Action.h"
#include "Policy.h"
#include "Service.h"

namespace
{
    RecoveryPolicy makePolicy(const IRecoveryAction& action)
    {
        return RecoveryPolicy({ &action });
    }
}

TEST(ServiceTest, InitialState)
{
    RestartAction restart;
    RecoveryPolicy policy = makePolicy(restart);

    Service service("Navigation", ServiceCategory::Critical, policy);

    EXPECT_EQ(service.name(), "Navigation");
    EXPECT_EQ(service.category(), ServiceCategory::Critical);
    EXPECT_EQ(service.state(), ServiceState::Running);
    EXPECT_EQ(service.recoveryLevel(), 0);
    EXPECT_EQ(service.failureCount(), 0);
    EXPECT_EQ(service.lastAction(), RecoveryActionType::None);
}

TEST(ServiceTest, MutatorsUpdateState)
{
    RestartAction restart;
    RecoveryPolicy policy = makePolicy(restart);

    Service service("Logger", ServiceCategory::Minor, policy);

    service.setState(ServiceState::Recovering);
    service.incrementRecoveryLevel();
    service.incrementFailureCount();
    service.incrementFailureCount();
    service.setLastAction(RecoveryActionType::Restart);

    EXPECT_EQ(service.state(), ServiceState::Recovering);
    EXPECT_EQ(service.recoveryLevel(), 1);
    EXPECT_EQ(service.failureCount(), 2);
    EXPECT_EQ(service.lastAction(), RecoveryActionType::Restart);
}

TEST(ServiceRegistryTest, RegisterAndFind)
{
    RestartAction restart;
    RecoveryPolicy policy = makePolicy(restart);

    ServiceRegistry registry;
    registry.registerService(
        std::make_unique<Service>("Radio", ServiceCategory::Major, policy));

    ASSERT_NE(registry.findService("Radio"), nullptr);
    EXPECT_EQ(registry.findService("Radio")->name(), "Radio");
    EXPECT_EQ(registry.findService("missing"), nullptr);
}

TEST(ServiceRegistryTest, RejectsNullService)
{
    ServiceRegistry registry;
    EXPECT_THROW(registry.registerService(nullptr), std::invalid_argument);
}

TEST(ServiceRegistryTest, RejectsDuplicateService)
{
    RestartAction restart;
    RecoveryPolicy policy = makePolicy(restart);

    ServiceRegistry registry;
    registry.registerService(
        std::make_unique<Service>("Radio", ServiceCategory::Major, policy));

    EXPECT_THROW(
        registry.registerService(
            std::make_unique<Service>("Radio", ServiceCategory::Major, policy)),
        std::runtime_error);
}
