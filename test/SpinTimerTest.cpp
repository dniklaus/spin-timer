#include <gtest/gtest.h>
#include <limits.h>

#include <SpinTimer.h>
#include <UptimeInfo.h>

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}
