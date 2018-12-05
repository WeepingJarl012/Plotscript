#include "notebook_app.hpp"
#include <QLayout>

NotebookApp::NotebookApp(QWidget * parent){
    if(parent!=nullptr){
        // Use variable, useless
    }
    
    input = new InputWidget();
    output = new OutputWidget();
    input->setObjectName("input");
    output->setObjectName("output");
    
    start = new QPushButton("Start Kernel", parent);
    stop = new QPushButton("Stop Kernel", parent);
    reset = new QPushButton("Reset Kernel", parent);
    interrupt = new QPushButton("Interrupt", parent);
    start->setObjectName("start");
    stop->setObjectName("stop");
    reset->setObjectName("reset");
    interrupt->setObjectName("interrupt");
    
    // QObject::connect(input, &InputWidget::textEvaluated, output, &OutputWidget::updateOutput);
    QObject::connect(input, SIGNAL(textEvaluated()), this, SLOT(changeOutput()));
    QObject::connect(this, &NotebookApp::outputChanged, output, &OutputWidget::updateOutput);
    QObject::connect(this, &NotebookApp::outputChangedError, output, &OutputWidget::updateOutputError);
    
    auto layout = new QGridLayout();
    layout->addWidget(input, 1, 0);
    layout->addWidget(output, 2, 0);
    auto buttons = new QHBoxLayout();
    buttons->addWidget(start);
    buttons->addWidget(stop);
    buttons->addWidget(reset);
    buttons->addWidget(interrupt);
    layout->addLayout(buttons, 0, 0);
    
    setLayout(layout);
}

void NotebookApp::changeOutput(){
    if (input->checkParseError() || input->checkExceptionError()){
        outputChangedError(input->getResult());
    } else {
        outputChanged(input->getResult());
    }
}
