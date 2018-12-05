#ifndef NOTEBOOKAPP_H
#define NOTEBOOKAPP_H

#include <QWidget>
#include <QPushButton>
#include "input_widget.hpp"
#include "output_widget.hpp"

class InputWidget;
class OutputWidget;

class NotebookApp: public QWidget {
    Q_OBJECT
private:
    
    InputWidget * input;
    OutputWidget * output;
    QPushButton * start;
    QPushButton * stop;
    QPushButton * reset;
    QPushButton * interrupt;
    
    
private slots:
    void changeOutput();
    
signals:
    void outputChanged(Expression result);
    void outputChangedError(Expression result);
    
public:
    
    NotebookApp(QWidget * parent = nullptr);
    
};

#endif

