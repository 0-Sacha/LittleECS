
#include "stream/tester/test_suite/all_test_suite.hxx"

int main() {
    stream::tester::TestSuitesManager::verbose = false;
    return static_cast<int>(stream::tester::TestSuitesManager::exec_all_test_suites());
}
