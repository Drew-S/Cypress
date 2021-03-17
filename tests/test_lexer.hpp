#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "../src/token.hpp"

using namespace std;

class TestLexer : public CppUnit::TestCase {
private:
    CPPUNIT_TEST_SUITE(TestLexer);

    CPPUNIT_TEST(test_comments);
    CPPUNIT_TEST(test_keywords);
    CPPUNIT_TEST(test_primitive_types);
    CPPUNIT_TEST(test_identities);
    CPPUNIT_TEST(test_ignore);
    CPPUNIT_TEST(test_blocks);
    CPPUNIT_TEST(test_assign);
    CPPUNIT_TEST(test_arithmetic);
    CPPUNIT_TEST(test_logical);
    CPPUNIT_TEST(test_bitwise);
    CPPUNIT_TEST(test_arithmetic_assign);
    CPPUNIT_TEST(test_bitwise_assign);
    CPPUNIT_TEST(test_parenthesis);
    CPPUNIT_TEST(test_arrays);
    CPPUNIT_TEST(test_specials);

    CPPUNIT_TEST_SUITE_END();

public:
    void test_comments();
    void test_keywords();
    void test_primitive_types();
    void test_identities();
    void test_ignore();
    void test_blocks();
    void test_assign();
    void test_arithmetic();
    void test_logical();
    void test_bitwise();
    void test_arithmetic_assign();
    void test_bitwise_assign();
    void test_parenthesis();
    void test_arrays();
    void test_specials();
};
