#ifndef INPUT_WIDGET_H
#define INPUT_WIDGET_H

#include <QPlainTextEdit>
#include <sstream>
#include <iostream>
#include <fstream>
#include <thread>

#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"

class InputWidget: public QPlainTextEdit {
    Q_OBJECT
public:
    
    InputWidget(QWidget * parent = nullptr);
    
    void keyPressEvent(QKeyEvent *ev);
    
    // Helper methods to get expression and errors
    Expression getResult();
    bool checkParseError();
    bool checkExceptionError();
    
private:
    
    struct Message {
        Expression expression;
        bool isError;  // Whether there was an error or not
        std::string errorMsg;
    };
    
    Expression exp;
    Interpreter interp;
    
    // Threading stuff
    void interpret(MessageQueue<std::string> & inputQueue, MessageQueue<Message> & outputQueue, bool & runInterpreter, Interpreter * interp);
    
    bool runInterpreter;
    
    MessageQueue<std::string> inputQueue;
    MessageQueue<Message> outputQueue;
    
    // Keep track if there is a parse error or semantic error
    bool parseError;
    bool exceptionError;
    
    void evaluateText();
    void evaluateStartupFile();
    
signals:
    void textEvaluated();

};

#endif
