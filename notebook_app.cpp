#include "notebook_app.hpp"
#include <QLayout>

NotebookApp::NotebookApp(QWidget * parent){
    input = new InputWidget();
    output = new OutputWidget();
    
    auto layout = new QGridLayout();
    layout->addWidget(input, 0, 0);
    layout->addWidget(output, 0, 1);
    
    setLayout(layout);
}
