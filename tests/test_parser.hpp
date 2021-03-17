#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "../src/token.hpp"
#include "../src/lexer.hpp"

using namespace std;

class TestParser : public CppUnit::TestCase {
private:
    CPPUNIT_TEST_SUITE(TestParser);

    CPPUNIT_TEST(test_primitive_assign);
    CPPUNIT_TEST(test_struct_declaration);
    CPPUNIT_TEST(test_interface_declaration);

    CPPUNIT_TEST_SUITE_END();

public:
    void test_primitive_assign();
    void test_struct_declaration();
    void test_interface_declaration();
};
