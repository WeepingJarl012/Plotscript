#ifndef NOTEBOOKAPP_H
#define NOTEBOOKAPP_H

#include <QWidget>
#include "input_widget.hpp"
#include "output_widget.hpp"

class InputWidget;
class OutputWidget;

class NotebookApp: public QWidget {
    Q_OBJECT
private:
    
    InputWidget * input;
    OutputWidget * output;
    
    
    private slots:
    
    
public:
    
    NotebookApp(QWidget * parent = nullptr);
    
};

#endif

