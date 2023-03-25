
#include "ProjectCore/Tester/TestSuite/AllTestSuite.h"

int main()
{
	ProjectCore::Tester::TestSuitesManager::Verbose = false;
	ProjectCore::Tester::TestSuitesManager::ExecAllTestSuites();
}
