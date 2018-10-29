#include "input_widget.hpp"

#include <QDebug>

InputWidget::InputWidget(QWidget * parent){
    evaluateStartupFile();
    
    parseError = false;
    exceptionError = false;
}


void InputWidget::keyPressEvent(QKeyEvent * ev){
    
    if((ev->type()==QEvent::KeyPress) && (((((QKeyEvent*)ev)->key() == Qt::Key_Return)) || ((QKeyEvent*)ev)->key() == Qt::Key_Enter)){
        
        if(ev->modifiers() == Qt::ShiftModifier){
            evaluateText();
            emit textEvaluated();
        } else {
            // Still types when not evaluate
            QPlainTextEdit::keyPressEvent(ev);
        }
    } else {
        // Still types when not evaluate
        QPlainTextEdit::keyPressEvent(ev);
    }
}

void InputWidget::evaluateStartupFile(){
    
    std::ifstream startup_stream(STARTUP_FILE);
    
    if(!startup_stream){
        parseError = true;
        exp = Expression(Atom("Could not open startup file for reading."));
    }
    
    if(!interp.parseStream(startup_stream)){
        parseError = true;
        exp = Expression(Atom("Invalid Program. Could not parse start up file."));
    }
    else{
        try{
            exp = interp.evaluate();
            parseError = false;
            exceptionError = false;
        }
        catch(const SemanticError & ex){
            exceptionError = true;
            exp = Expression(Atom(ex.what()));
        }
    }
    
}

void InputWidget::evaluateText(){
    
    auto qstream = this->toPlainText();
    auto line = qstream.toStdString();
    
    std::istringstream expression(line);
    
    if(!interp.parseStream(expression)){
        parseError = true;
        exp = Expression(Atom("Invalid Program. Could not parse."));
    }
    else{
        try{
            exp = interp.evaluate();
            parseError = false;
            exceptionError = false;
        }
        catch(const SemanticError & ex){
            exceptionError = true;
            exp = Expression(Atom(ex.what()));
        }
    }
    
    std::cout << exp << std::endl;
    
}

Expression InputWidget::getResult(){
    return exp;
}

bool InputWidget::checkParseError(){
    return parseError;
}

bool InputWidget::checkExceptionError(){
    return exceptionError;
}
