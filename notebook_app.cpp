#include "notebook_app.hpp"
#include <QLayout>

NotebookApp::NotebookApp(QWidget * parent){
    input = new InputWidget();
    output = new OutputWidget();
    input->setObjectName("input");
    output->setObjectName("output");
    
    auto layout = new QGridLayout();
    layout->addWidget(input, 0, 0);
    layout->addWidget(output, 1, 0);
    
    setLayout(layout);
}
