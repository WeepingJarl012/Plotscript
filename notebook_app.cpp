#include "notebook_app.hpp"
#include <QLayout>

NotebookApp::NotebookApp(QWidget * parent){
    input = new InputWidget();
    output = new OutputWidget();
    input->setObjectName("input");
    output->setObjectName("output");
    
    // QObject::connect(input, &InputWidget::textEvaluated, output, &OutputWidget::updateOutput);
    QObject::connect(input, SIGNAL(textEvaluated()), this, SLOT(changeOutput()));
    QObject::connect(this, &NotebookApp::outputChanged, output, &OutputWidget::updateOutput);
    QObject::connect(this, &NotebookApp::outputChangedError, output, &OutputWidget::updateOutputError);
    
    auto layout = new QGridLayout();
    layout->addWidget(input, 0, 0);
    layout->addWidget(output, 1, 0);
    
    setLayout(layout);
}

void NotebookApp::changeOutput(){
    if (input->checkParseError() || input->checkExceptionError()){
        outputChangedError(input->getResult());
    } else {
        outputChanged(input->getResult());
    }
}
