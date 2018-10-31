#include <QTest>

#include <QSignalSpy>

#include "notebook_app.hpp"

class NotebookTest : public QObject {
    Q_OBJECT
    
    private slots:
    
    void initTestCase();
    
    // TODO: implement additional tests here
    
    void testConstructor();
    
    void testInputEvaluate();
    
    void testEvaluateText();
    
private:
    NotebookApp notebook;
    
    InputWidget * input;
    
    OutputWidget * output;
    
};

void NotebookTest::initTestCase(){
    input = notebook.findChild<InputWidget *>("input");
    
    output = notebook.findChild<OutputWidget *>("output");
}

void NotebookTest::testConstructor(){
    QVERIFY2(input, "Could not find widget with name: 'input'");
    
    QVERIFY2(output, "Could not find widget with name: 'output'");
}

void NotebookTest::testInputEvaluate(){
    QSignalSpy spy(input, SIGNAL(textEvaluated()));
    
    // Test when button other than return is pressed
    QTest::keyClick(input, Qt::Key_Shift);
    QCOMPARE(spy.count(), 0);
    
    // Test when enter is pressed without shift modifier
    QTest::keyClick(input, Qt::Key_Return);
    QCOMPARE(spy.count(), 0);
    
    // Test when enter is pressed with shift modifier
    QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);
    QCOMPARE(spy.count(), 1);
}

void NotebookTest::testEvaluateText(){
    // Input text into the input and evaluate it
    input->insertPlainText("(cos pi)");
    QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);
    
    QCOMPARE(input->getResult(), Expression(Atom(-1)));
    QVERIFY2(!input->checkParseError(), "Parse error when there shouldn't be");
    QVERIFY2(!input->checkExceptionError(), "Exception error when there shouldn't be");
    
    input->clear();
    
    // Input text with parse error
    input->insertPlainText("(cos pi");
    QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);
    
    QCOMPARE(input->getResult(), Expression(Atom("Error: Invalid Expression. Could not parse.")));
    QVERIFY2(input->checkParseError(), "No parse error when there should be");
    QVERIFY2(!input->checkExceptionError(), "Exception error when there shouldn't be");
    
    input->clear();
    
    // Input text with exception error
    input->insertPlainText("(first 1)");
    QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);
    
    auto real = input->getResult();
    
    QCOMPARE(input->getResult(), Expression(Atom("Error: argument to first is not a list")));
    QVERIFY2(input->checkParseError(), "Not parse error when there should be");
    QVERIFY2(input->checkExceptionError(), "No exception error when there should be");
    
    input->clear();
}


QTEST_MAIN(NotebookTest)
#include "notebook_test.moc"
