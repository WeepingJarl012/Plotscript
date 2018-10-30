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
    
    if (result.isHeadPoint()){
        // Create Point
        double size = result.get_property(Expression(Atom("\"size\""))).head().asNumber();
        double xLoc = result.tail()[0].head().asNumber();
        double yLoc = result.tail()[1].head().asNumber();
        
        if (size >= 0){
            scene->addEllipse(xLoc, yLoc, size, size, QPen(), QBrush(Qt::black, Qt::SolidPattern));
        } else {
            std::stringstream resultString;
            
            resultString << "Error in make-point: Size can't be negative";
            
            QString qResultString = QString::fromStdString(resultString.str());
            scene->addText(qResultString);
        }
        
    } else if (result.isHeadLine()){
        // Create line
    } else if (result.isHeadText()){
        // Create text
    } else if (result.isHeadComplex() || result.isHeadNone() || result.isHeadNumber() || result.isHeadString() || result.isHeadSymbol()) {
        std::stringstream resultString;
        
        if (!result.isHeadComplex() && !result.isHeadNone()){
            resultString << "(" << result.head() << ")";
        } else if (result.isHeadComplex()){
            resultString << result.head();
        } else if (result.isHeadNone()){
            resultString << "NONE";
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
