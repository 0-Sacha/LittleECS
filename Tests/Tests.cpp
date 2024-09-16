
#include "StreamFormat/Tester/TestSuite/AllTestSuite.h"

int main()
{
    StreamFormat::Tester::TestSuitesManager::Verbose = false;
    return StreamFormat::Tester::TestSuitesManager::ExecAllTestSuites();
}
