
#include "ProjectCore/Tester/TestSuite/AllTestSuite.h"

int main()
{
    ProjectCore::Tester::TestSuitesManager::Verbose = false;
    return ProjectCore::Tester::TestSuitesManager::ExecAllTestSuites();
}
