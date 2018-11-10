#include "output_widget.hpp"

#include <QDebug>

OutputWidget::OutputWidget(QWidget * parent){
    if(parent!=nullptr){
        // Use variable, useless
    }
    
    scene = new QGraphicsScene;
    view = new QGraphicsView(scene);
    // view->setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern));
    // Changes output widget size, comment out to fit
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
        
        outputPoint(result);
        
    } else if (result.isHeadLine()){
        
        outputLine(result);
        
    } else if (result.isHeadText()){
        
        outputText(result);
        
    } else if (result.isHeadComplex() || result.isHeadNone() || result.isHeadNumber() || result.isHeadString() || result.isHeadSymbol()) {
        
        // Check if it's a discrete plot
        if (result.head().asSymbol() == "discrete-plot"){
            createPlot(result);
        } else {
            outputResult(result);
        }
        
    } else if (result.isHeadLambda()) {
        // Do nothing
    } else if (result.isHeadList()) {
        outputList = true;
        for(auto e = result.tailConstBegin(); e != result.tailConstEnd(); ++e){
            updateOutput(*e);
        }
        outputList = false;
    }
    
    view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void OutputWidget::updateOutputError(Expression result){
    scene->clear();
    
    std::stringstream resultString;
    
    resultString << result.head();
    
    QString qResultString = QString::fromStdString(resultString.str());
    scene->addText(qResultString);
    
}

void OutputWidget::createPlot(Expression result){
    
    Expression title = result.get_property(Expression(Atom("\"title\"")));
    Expression absLabel = result.get_property(Expression(Atom("\"abscissa-label\"")));
    Expression ordLabel = result.get_property(Expression(Atom("\"ordinate-label\"")));
    Expression textScale = result.get_property(Expression(Atom("\"text-scale\"")));
    
    // Constant layout parameters
    const double N = 20;
    const double A = 3;
    const double B = 3;
    const double C = 2;
    const double D = 2;
    const double P = 0.5;
    
    // Add the plot rectangle
    double botRightX = P + (N / 2);
    double botRightY = P + (N / 2);
    double topLeftX = P - (N / 2);
    double topLeftY = P - (N / 2);
    QRect dataBox = QRect(QPoint(botRightX, botRightY), QPoint(topLeftX, topLeftY));
    myPen->setWidth(0);
    scene->addRect(dataBox, *myPen);
    
    // Add graph title
    Expression titleLoc;
    titleLoc.setHead(Atom("list"));
    titleLoc.append(P);
    titleLoc.append(Atom(topLeftY - A));
    title.add_property(Expression(Atom("\"position\"")), titleLoc);
    title.add_property(Expression(Atom("\"text-scale\"")), textScale);
    outputText(title);
    
    // Add graph x-axis label
    Expression absLoc;
    absLoc.setHead(Atom("list"));
    absLoc.append(P);
    absLoc.append(Atom(botRightY + A));
    absLabel.add_property(Expression(Atom("\"position\"")), absLoc);
    absLabel.add_property(Expression(Atom("\"text-scale\"")), textScale);
    outputText(absLabel);
    
    // Add graph y-axis label
    Expression ordLoc;
    ordLoc.setHead(Atom("list"));
    ordLoc.append(Atom(topLeftX - B));
    ordLoc.append(P);
    ordLabel.add_property(Expression(Atom("\"position\"")), ordLoc);
    ordLabel.add_property(Expression(Atom("\"text-scale\"")), textScale);
    // Rotate label by 90 degrees in radians
    ordLabel.add_property(Expression(Atom("\"text-rotation\"")), Expression(Atom(std::atan2(0, -1)/-2)));
    outputText(ordLabel);
    
}

void OutputWidget::outputResult(Expression result){
    
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
    
}

void OutputWidget::outputText(Expression result){
    
    // Create text
    std::stringstream resultString;
    
    Expression point = result.get_property(Expression(Atom("\"position\"")));
    
    double xLoc = point.tail()[-1].head().asNumber();
    double yLoc = point.tail()[0].head().asNumber();
    int textSize = 1;
    double textRot = 0;
    
    Expression textScale = result.get_property(Expression(Atom("\"text-scale\"")));
    Expression textRotation = result.get_property(Expression(Atom("\"text-rotation\"")));
    
    if (textScale.isHeadNumber() && textScale.head().asNumber() > 0) {
        textSize = textScale.head().asNumber();
    }
    
    if (textRotation.isHeadNumber()) {
        textRot = textRotation.head().asNumber();
        textRot *= (180/std::atan2(0,-1));
    }
    
    // Setup font
    QFont myTextFont("Courier", textSize);
    
    // Full string with quotations
    std::string fullString = result.head().asSymbol();
    
    // Remove quotations from string
    resultString << fullString.substr(1, fullString.length()-2);
    
    QString qResultString = QString::fromStdString(resultString.str());
    QGraphicsTextItem *text = scene->addText(qResultString);
    // Set rotation in radians
    text->setRotation(textRot);
    text->setFont(myTextFont);
    double newxLoc = xLoc - (text->boundingRect().width()/2);
    double newyLoc = yLoc - (text->boundingRect().height()/2);
    
    // Fix center when rotated
    if (textRot != 0){
        newxLoc = xLoc + (std::cos(textRot) * text->boundingRect().width() / 2);
        newyLoc = yLoc - (std::sin(textRot) * text->boundingRect().width() / 2);
    }
    
    text->setPos(newxLoc, newyLoc);
    
}

void OutputWidget::outputLine(Expression result){
    
    // Create line
    bool error = false;
    
    int thickness = result.get_property(Expression(Atom("\"thickness\""))).head().asNumber();
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
}

void OutputWidget::outputPoint(Expression result){
    
    // Create Point
    double size = result.get_property(Expression(Atom("\"size\""))).head().asNumber();
    double xLoc = result.tail()[-1].head().asNumber() - (size / 2);
    double yLoc = result.tail()[0].head().asNumber() - (size / 2);
    
    if (size >= 0){
        scene->addEllipse(xLoc, yLoc, size, size, QPen(), QBrush(Qt::black, Qt::SolidPattern));
    } else {
        std::stringstream resultString;
        
        resultString << "Error in make-point: Size can't be negative";
        
        QString qResultString = QString::fromStdString(resultString.str());
        scene->addText(qResultString);
    }
    
}
