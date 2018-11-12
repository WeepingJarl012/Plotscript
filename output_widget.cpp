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
    const double center = 0;
    
    // Add the plot rectangle
    double botRightX = center + (N / 2);
    double botRightY = center + (N / 2);
    double topLeftX = center - (N / 2) - 1;
    double topLeftY = center - (N / 2) - 1;
    QRect dataBox = QRect(QPoint(botRightX, botRightY), QPoint(topLeftX, topLeftY));
    myPen->setWidth(0);
    scene->addRect(dataBox, *myPen);
    
    // Add graph title
    Expression titleLoc;
    titleLoc.setHead(Atom("list"));
    titleLoc.append(center);
    titleLoc.append(Atom(topLeftY - A + 1));
    title.add_property(Expression(Atom("\"position\"")), titleLoc);
    title.add_property(Expression(Atom("\"text-scale\"")), textScale);
    outputText(title);
    
    // Add graph x-axis label
    Expression absLoc;
    absLoc.setHead(Atom("list"));
    absLoc.append(center);
    absLoc.append(Atom(botRightY + A));
    absLabel.add_property(Expression(Atom("\"position\"")), absLoc);
    absLabel.add_property(Expression(Atom("\"text-scale\"")), textScale);
    outputText(absLabel);
    
    // Add graph y-axis label
    Expression ordLoc;
    ordLoc.setHead(Atom("list"));
    ordLoc.append(Atom(topLeftX - B + 1));
    ordLoc.append(center);
    ordLabel.add_property(Expression(Atom("\"position\"")), ordLoc);
    ordLabel.add_property(Expression(Atom("\"text-scale\"")), textScale);
    // Rotate label by 90 degrees in radians
    ordLabel.add_property(Expression(Atom("\"text-rotation\"")), Expression(Atom(std::atan2(0, -1)/-2)));
    outputText(ordLabel);
    
    // Get min and max for x and y values
    double maxXVal = -__DBL_MAX__;
    double maxYVal = -__DBL_MAX__;
    double minXVal = __DBL_MAX__;
    double minYVal = __DBL_MAX__;
    for (Expression::ConstIteratorType i = result.tailConstBegin(); i != result.tailConstEnd(); i++){
        Expression point = *i;
        
        if (point.tail()[-1].head().asNumber() > maxXVal){
            maxXVal = point.tail()[-1].head().asNumber();
        }
        if (point.tail()[-1].head().asNumber() < minXVal){
            minXVal = point.tail()[-1].head().asNumber();
        }
        
        if (point.tail()[0].head().asNumber() > maxYVal){
            maxYVal = point.tail()[0].head().asNumber();
        }
        if (point.tail()[0].head().asNumber() < minYVal){
            minYVal = point.tail()[0].head().asNumber();
        }
    }
    
    // Add graph ou label
    Expression ouLabel;
    std::stringstream resultOU;
    resultOU << "\"" << std::setprecision(2) << maxYVal << "\"";
    ouLabel.setHead(Atom(resultOU.str()));
    Expression ouLoc;
    ouLoc.setHead(Atom("list"));
    ouLoc.append(Atom(topLeftX - D));
    ouLoc.append(topLeftY);
    ouLabel.add_property(Expression(Atom("\"position\"")), ouLoc);
    ouLabel.add_property(Expression(Atom("\"text-scale\"")), textScale);
    outputText(ouLabel);
    
    // Add graph ol label
    Expression olLabel;
    std::stringstream resultOL;
    resultOL << "\"" << std::setprecision(2) << minYVal << "\"";
    olLabel.setHead(Atom(resultOL.str()));
    Expression olLoc;
    olLoc.setHead(Atom("list"));
    olLoc.append(Atom(topLeftX - D));
    olLoc.append(botRightY);
    olLabel.add_property(Expression(Atom("\"position\"")), olLoc);
    olLabel.add_property(Expression(Atom("\"text-scale\"")), textScale);
    outputText(olLabel);
    
    // Add graph au label
    Expression auLabel;
    std::stringstream resultAU;
    resultAU << "\"" << std::setprecision(2) << maxXVal << "\"";
    auLabel.setHead(Atom(resultAU.str()));
    Expression auLoc;
    auLoc.setHead(Atom("list"));
    auLoc.append(botRightX);
    auLoc.append(Atom(botRightY + C));
    auLabel.add_property(Expression(Atom("\"position\"")), auLoc);
    auLabel.add_property(Expression(Atom("\"text-scale\"")), textScale);
    outputText(auLabel);
    
    // Add graph al label
    Expression alLabel;
    std::stringstream resultAL;
    resultAL << "\"" << std::setprecision(2) << minXVal << "\"";
    alLabel.setHead(Atom(resultAL.str()));
    Expression alLoc;
    alLoc.setHead(Atom("list"));
    alLoc.append(topLeftX);
    alLoc.append(Atom(botRightY + C));
    alLabel.add_property(Expression(Atom("\"position\"")), alLoc);
    alLabel.add_property(Expression(Atom("\"text-scale\"")), textScale);
    outputText(alLabel);
    
    // Add grid
    // double xSpacing = (maxXVal - minXVal) / N;
    // double ySpacing = (maxYVal - minYVal) / N;
    
    // Corners of the plot
    double botLX = topLeftX;
    double botLY = botRightY;
    double botRX = botRightX;
    double topLY = topLeftY;
    
    // Add x line for origin grid
    if (0 > minXVal && 0 < maxXVal){
        double zeroX = botLX + ((0 - minXVal) * N / (maxXVal - minXVal)) + 1;
        
        Expression line;
        line.add_property(Expression(Atom("\"object-name\"")), Expression(Atom("\"line\"")));
        line.add_property(Expression(Atom("\"thickness\"")), Expression(Atom(0)));
        line.setHead(Atom("list"));
        
        Expression point1;
        Expression point2;
        point1.add_property(Expression(Atom("\"object-name\"")), Expression(Atom("\"point\"")));
        point2.add_property(Expression(Atom("\"object-name\"")), Expression(Atom("\"point\"")));
        point1.setHead(Atom("list"));
        point1.append(Atom(zeroX));
        point1.append(Atom(botLY));
        
        point2.setHead(Atom("list"));
        point2.append(Atom(zeroX));
        point2.append(Atom(topLY + 1));
        
        line.append(point1);
        line.append(point2);
        
        outputLine(line);
    }
    
    // Add y line for origin grid
    if (0 > minYVal && 0 < maxYVal){
        double zeroY = botLY - ((0 - minYVal) * N / (maxYVal - minYVal));
        
        Expression line;
        line.add_property(Expression(Atom("\"object-name\"")), Expression(Atom("\"line\"")));
        line.add_property(Expression(Atom("\"thickness\"")), Expression(Atom(0)));
        line.setHead(Atom("list"));
        
        Expression point1;
        Expression point2;
        point1.add_property(Expression(Atom("\"object-name\"")), Expression(Atom("\"point\"")));
        point2.add_property(Expression(Atom("\"object-name\"")), Expression(Atom("\"point\"")));
        point1.setHead(Atom("list"));
        point1.append(Atom(botLX + 1));
        point1.append(Atom(zeroY));
        
        point2.setHead(Atom("list"));
        point2.append(Atom(botRX));
        point2.append(Atom(zeroY));
        
        line.append(point1);
        line.append(point2);
        
        outputLine(line);
    }
    
    // Add points
    for (Expression::ConstIteratorType i = result.tailConstBegin(); i != result.tailConstEnd(); i++){
        Expression point = *i;
        Expression newPoint;
        newPoint.add_property(Expression(Atom("\"size\"")), Expression(Atom(P)));
        newPoint.add_property(Expression(Atom("\"object-name\"")), Expression(Atom("\"point\"")));
        
        newPoint.setHead(point.head());
        
        // Find x/y coord relative to minimum x/y
        double relativeX = ((0 - minXVal) * N / (point.tail()[-1].head().asNumber()));
        double relativeY = 0 - ((0 - minYVal) * N / (point.tail()[0].head().asNumber()));
        
        if (relativeX > botRX){
            // Put smallest x on x axis
            relativeX = botLX;
        }
        if (relativeY < topLY){
            // Put smalled y on y axis
            relativeY = botLY;
        }
        
        newPoint.append(1 + relativeX);
        newPoint.append(relativeY);
        
        outputPoint(newPoint);
        
    }
    
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
    // double newxLoc = xLoc - (text->boundingRect().width()/2);
    // double newyLoc = yLoc - (text->boundingRect().height()/2);
    
    // Fix center when rotated
    if (textRot != 0){
        // newxLoc = xLoc - (std::cos(textRot * std::atan2(0,-1) / 180) * text->boundingRect().width() / 2);
        // newyLoc = yLoc - (std::sin(textRot * std::atan2(0,-1) / 180) * text->boundingRect().height() / 2);
        // text->setPos(QPointF(xLoc, yLoc) + text->boundingRect().center());
        text->setPos(xLoc - text->boundingRect().center().rx(), (yLoc + text->boundingRect().center().ry()));
    } else {
        text->setPos(QPointF(xLoc, yLoc) - text->boundingRect().center());
    }
    
    // text->setPos(newxLoc, newyLoc);
    
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


