#include "output_widget.hpp"

#include <QDebug>

OutputWidget::OutputWidget(QWidget * parent){
    scene = new QGraphicsScene;
    view = new QGraphicsView(scene);
    // view->setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern));
    view->setFixedSize(500, 200);
    
    auto layout = new QGridLayout;
    layout->addWidget(view, 0, 0);
    setLayout(layout);
}

void OutputWidget::updateOutput(Expression result){
    scene->clear();
    
    if (result.isHeadComplex() || result.isHeadNone() || result.isHeadNumber() || result.isHeadString() || result.isHeadSymbol()) {
        std::stringstream resultString;
        
        if (!result.isHeadComplex()){
            resultString << "(" << result.head() << ")";
        } else {
            resultString << result.head();
        }
        
        QString qResultString = QString::fromStdString(resultString.str());
        scene->addText(qResultString);
    } else if (result.isHeadLambda()) {
        // Do nothing
    }
    
}

void OutputWidget::updateOutputError(Expression result){
    scene->clear();
    
    std::stringstream resultString;
    
    resultString << result.head();
    
    QString qResultString = QString::fromStdString(resultString.str());
    scene->addText(qResultString);
    
}
