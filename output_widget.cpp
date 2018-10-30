#include "output_widget.hpp"

#include <QDebug>

OutputWidget::OutputWidget(QWidget * parent){
    if(parent!=nullptr){
        // Use variable, useless
    }
    
    scene = new QGraphicsScene;
    view = new QGraphicsView(scene);
    // view->setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern));
    view->setFixedSize(500, 200);
    
    myPen = new QPen();
    myPen->setBrush(QBrush(Qt::black, Qt::SolidPattern));
    
    auto layout = new QGridLayout;
    layout->addWidget(view, 0, 0);
    setLayout(layout);
    
    outputList = false;
}

void OutputWidget::updateOutput(Expression result){
    
    if (!outputList){
        scene->clear();
    }
    
    if (result.isHeadPoint()){
        // Create Point
        double size = result.get_property(Expression(Atom("\"size\""))).head().asNumber();
        double xLoc = result.tail()[-1].head().asNumber();// - (size / 2);
        double yLoc = result.tail()[0].head().asNumber();// - (size / 2);
        
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
        bool error = false;
        
        double thickness = result.get_property(Expression(Atom("\"thickness\""))).head().asNumber();
        double xLoc1;
        double yLoc1;
        double xLoc2;
        double yLoc2;
        
        Expression point1;
        Expression point2;
        
        if (result.isHeadList()){
            point1 = result.tail()[-1];
            point2 = result.tail()[0];
        }
        
        if (point1.isHeadList() && point2.isHeadList()) {
            xLoc1 = point1.tail()[-1].head().asNumber();
            yLoc1 = point1.tail()[0].head().asNumber();
            xLoc2 = point2.tail()[-1].head().asNumber();
            yLoc2 = point2.tail()[0].head().asNumber();
        } else {
            error = true;
        }
        
        if (thickness >= 0 && !error){
            myPen->setWidth(thickness);
            scene->addLine(xLoc1, yLoc1, xLoc2, yLoc2, *myPen);
        } else {
            std::stringstream resultString;
            
            resultString << "Error in make-line: Thickness can't be negative";
            
            QString qResultString = QString::fromStdString(resultString.str());
            scene->addText(qResultString);
        }
        
    } else if (result.isHeadText()){
        // Create text
        std::stringstream resultString;
        
        Expression point = result.get_property(Expression(Atom("\"position\"")));
        
        double xLoc = point.tail()[-1].head().asNumber();
        double yLoc = point.tail()[0].head().asNumber();
        
        // Full string with quotations
        std::string fullString = result.head().asSymbol();
        
        // Remove quotations from string
        resultString << fullString.substr(1, fullString.length()-2);
        
        QString qResultString = QString::fromStdString(resultString.str());
        QGraphicsTextItem *text = scene->addText(qResultString);
        text->setPos(xLoc, yLoc);
        
        
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
    } else if (result.isHeadList()) {
        outputList = true;
        for(auto e = result.tailConstBegin(); e != result.tailConstEnd(); ++e){
            updateOutput(*e);
        }
        outputList = false;
    }
}

void OutputWidget::updateOutputError(Expression result){
    scene->clear();
    
    std::stringstream resultString;
    
    resultString << result.head();
    
    QString qResultString = QString::fromStdString(resultString.str());
    scene->addText(qResultString);
    
}
