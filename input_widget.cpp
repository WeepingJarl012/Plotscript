#include "input_widget.hpp"

#include <QDebug>

InputWidget::InputWidget(QWidget * parent){
    
}


void InputWidget::keyPressEvent(QKeyEvent * ev){
    
    if((ev->type()==QEvent::KeyPress) && (((((QKeyEvent*)ev)->key() == Qt::Key_Return)) || ((QKeyEvent*)ev)->key() == Qt::Key_Enter)){
        
        if(ev->modifiers() == Qt::ShiftModifier){
            qDebug() << "Enter Key pressed with shift";
            evaluateText();
            emit textChanged();
        } else {
            // Still types when not evaluate
            QPlainTextEdit::keyPressEvent(ev);
        }
    } else {
        // Still types when not evaluate
        QPlainTextEdit::keyPressEvent(ev);
    }
}

void InputWidget::evaluateText(){
    Interpreter interp;
    
    auto stream = this->toPlainText();
    
    qDebug() << stream;
    
    /**
    std::ifstream startup_stream(STARTUP_FILE);
    
    if(!startup_stream){
        // error("Could not open startup file for reading.");
    }
    
    if(!interp.parseStream(startup_stream)){
        // error("Invalid Program. Could not parse start up file.");
    }
    else{
        try{
            exp = interp.evaluate();
        }
        catch(const SemanticError & ex){
            std::cerr << ex.what() << std::endl;
        }
    }
    
    if(!interp.parseStream(stream)){
        // error("Invalid Program. Could not parse.");
        // return EXIT_FAILURE;
    }
    else{
        try{
            exp = interp.evaluate();
            std::cout << exp << std::endl;
        }
        catch(const SemanticError & ex){
            std::cerr << ex.what() << std::endl;
            // return EXIT_FAILURE;
        }
    }
    **/
    
}
