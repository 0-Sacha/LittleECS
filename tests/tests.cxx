
#include "StreamFormat/Tester/TestSuite/AllTestSuite.h"

int main() {
    StreamFormat::Tester::TestSuitesManager::Verbose = false;
    return static_cast<int>(StreamFormat::Tester::TestSuitesManager::ExecAllTestSuites());
}
