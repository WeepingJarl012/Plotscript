#include "input_widget.hpp"

#include <QDebug>

InputWidget::InputWidget(QWidget * parent){
    
}

void InputWidget::keyPressEvent(QKeyEvent * ev){
    
    if((ev->type()==QEvent::KeyPress) && (((((QKeyEvent*)ev)->key() == Qt::Key_Return)) || ((QKeyEvent*)ev)->key() == Qt::Key_Enter)){
        
        if(ev->modifiers() == Qt::ShiftModifier){
            qDebug() << "Enter Key pressed with shift";
            emit textChanged();
        }
        
    }
    
}
