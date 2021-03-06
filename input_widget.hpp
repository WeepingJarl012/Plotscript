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
    Expression exp;
    Interpreter interp;
    
    // Keep track if there is a parse error or semantic error
    bool parseError;
    bool exceptionError;
    
    void evaluateText();
    void evaluateStartupFile();
    
signals:
    void textEvaluated();

};

#endif
