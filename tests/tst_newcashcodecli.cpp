#include <QtTest>

// add necessary includes here

class TestCashCodeCLI : public QObject
{
    Q_OBJECT

public:
    TestCashCodeCLI();
    ~TestCashCodeCLI();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();

};

TestCashCodeCLI::TestCashCodeCLI()
{

}

TestCashCodeCLI::~TestCashCodeCLI()
{

}

void TestCashCodeCLI::initTestCase()
{

}

void TestCashCodeCLI::cleanupTestCase()
{

}

void TestCashCodeCLI::test_case1()
{

}

QTEST_APPLESS_MAIN(TestCashCodeCLI)

#include "tst_newcashcodecli.moc"

