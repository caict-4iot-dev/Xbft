#include <gmock/gmock.h>
#include <gtest/gtest.h>

//////////////////////////////////////////////////////////////////////////
// testMainEnvironment
//////////////////////////////////////////////////////////////////////////
class testMainEnvironment : public testing::Environment {
public:
    virtual void SetUp();
    virtual void TearDown();
};

void testMainEnvironment::SetUp() {
    std::cout << "testHandleEnvironment SetUP" << std::endl;
}

void testMainEnvironment::TearDown() {
    std::cout << "testHandleEnvironment TearDown" << std::endl;
}

GTEST_API_ int main(int argc, char **argv) {
    testing::AddGlobalTestEnvironment(new testMainEnvironment);
    testing::GTEST_FLAG(output) = "xml:gtest_result.xml";
    testing::InitGoogleTest(&argc, argv);
    testing::InitGoogleMock(&argc, argv);
    int ret = RUN_ALL_TESTS();
    printf("RUN_ALL_TESTS() return:%d\n", ret);
    return 0;
}