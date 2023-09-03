#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QObject>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(core_res);

    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
