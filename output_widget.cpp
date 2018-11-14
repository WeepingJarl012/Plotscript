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
    myPen->setWidth(0);
    outputBoundingPlot(botRightX, botRightY, topLeftX, topLeftY);
    
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
    ouLoc.append(Atom(topLeftX - D + 1));
    ouLoc.append(topLeftY + 1);
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
    olLoc.append(Atom(topLeftX - D + 1));
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
    alLoc.append(topLeftX + 1);
    alLoc.append(Atom(botRightY + C));
    alLabel.add_property(Expression(Atom("\"position\"")), alLoc);
    alLabel.add_property(Expression(Atom("\"text-scale\"")), textScale);
    outputText(alLabel);
    
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
    
    double plotZeroY;
    
    // Add y line for origin grid
    if (0 > minYVal && 0 < maxYVal){
        double zeroY = botLY - ((0 - minYVal) * N / (maxYVal - minYVal));
        plotZeroY = zeroY;
        
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
        double xFromLeft = (minXVal - point.tail()[-1].head().asNumber());
        double yFromBot = (minYVal - point.tail()[0].head().asNumber());
        
        double relativeX;
        double relativeY;
        
        if (xFromLeft == 0) {
            relativeX = botLX;
        } else {
            relativeX = botLX - (xFromLeft * N / abs(minXVal - maxXVal));
        }
        
        if (yFromBot == 0){
            relativeY = botLY;
        } else {
            relativeY = botLY + (yFromBot * N / abs(minYVal - maxYVal));
        }
        
        newPoint.append(1 + relativeX);
        newPoint.append(relativeY);
        
        // Create line to point
        Expression pointLine;
        pointLine.add_property(Expression(Atom("\"object-name\"")), Expression(Atom("\"line\"")));
        pointLine.add_property(Expression(Atom("\"thickness\"")), Expression(Atom(0)));
        pointLine.setHead(Atom("list"));
        
        Expression point1l;
        Expression point2l;
        point1l.add_property(Expression(Atom("\"object-name\"")), Expression(Atom("\"point\"")));
        point1l.setHead(Atom("list"));
        point1l.append(Atom(1 + relativeX));
        point1l.append(Atom(plotZeroY));
        
        pointLine.append(point1l);
        pointLine.append(newPoint);
        
        outputLine(pointLine);
        
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
    text->setFont(myTextFont);
    text->setPos(xLoc - (text->boundingRect().width()/2), yLoc - (text->boundingRect().height()/2));
    text->setTransformOriginPoint(text->boundingRect().center());
    text->setRotation(textRot);
    
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
        // Qt::NoPen() instead of QPen()
        scene->addEllipse(xLoc, yLoc, size, size, Qt::NoPen, QBrush(Qt::black, Qt::SolidPattern));
    } else {
        std::stringstream resultString;
        
        resultString << "Error in make-point: Size can't be negative";
        
        QString qResultString = QString::fromStdString(resultString.str());
        scene->addText(qResultString);
    }
    
}

void OutputWidget::outputBoundingPlot(double botRightX, double botRightY, double topLeftX, double topLeftY){
    
    // Create left line
    Expression leftLine;
    leftLine.add_property(Expression(Atom("\"object-name\"")), Expression(Atom("\"line\"")));
    leftLine.add_property(Expression(Atom("\"thickness\"")), Expression(Atom(0)));
    leftLine.setHead(Atom("list"));
    
    Expression point1ll;
    Expression point2ll;
    point1ll.add_property(Expression(Atom("\"object-name\"")), Expression(Atom("\"point\"")));
    point2ll.add_property(Expression(Atom("\"object-name\"")), Expression(Atom("\"point\"")));
    point1ll.setHead(Atom("list"));
    point1ll.append(Atom(topLeftX + 1));
    point1ll.append(Atom(botRightY));
    
    point2ll.setHead(Atom("list"));
    point2ll.append(Atom(topLeftX + 1));
    point2ll.append(Atom(topLeftY + 1));
    
    leftLine.append(point1ll);
    leftLine.append(point2ll);
    
    outputLine(leftLine);
    
    // Create right line
    Expression rightLine;
    rightLine.add_property(Expression(Atom("\"object-name\"")), Expression(Atom("\"line\"")));
    rightLine.add_property(Expression(Atom("\"thickness\"")), Expression(Atom(0)));
    rightLine.setHead(Atom("list"));
    
    Expression point1rl;
    Expression point2rl;
    point1rl.add_property(Expression(Atom("\"object-name\"")), Expression(Atom("\"point\"")));
    point2rl.add_property(Expression(Atom("\"object-name\"")), Expression(Atom("\"point\"")));
    point1rl.setHead(Atom("list"));
    point1rl.append(Atom(botRightX));
    point1rl.append(Atom(botRightY));
    
    point2rl.setHead(Atom("list"));
    point2rl.append(Atom(botRightX));
    point2rl.append(Atom(topLeftY + 1));
    
    rightLine.append(point1rl);
    rightLine.append(point2rl);
    
    outputLine(rightLine);
    
    // Create top line
    Expression topLine;
    topLine.add_property(Expression(Atom("\"object-name\"")), Expression(Atom("\"line\"")));
    topLine.add_property(Expression(Atom("\"thickness\"")), Expression(Atom(0)));
    topLine.setHead(Atom("list"));
    
    Expression point1tl;
    Expression point2tl;
    point1tl.add_property(Expression(Atom("\"object-name\"")), Expression(Atom("\"point\"")));
    point2tl.add_property(Expression(Atom("\"object-name\"")), Expression(Atom("\"point\"")));
    point1tl.setHead(Atom("list"));
    point1tl.append(Atom(topLeftX + 1));
    point1tl.append(Atom(topLeftY + 1));
    
    point2tl.setHead(Atom("list"));
    point2tl.append(Atom(botRightX));
    point2tl.append(Atom(topLeftY + 1));
    
    topLine.append(point1tl);
    topLine.append(point2tl);
    
    outputLine(topLine);
    
    // Create bottom line
    Expression bottomLine;
    bottomLine.add_property(Expression(Atom("\"object-name\"")), Expression(Atom("\"line\"")));
    bottomLine.add_property(Expression(Atom("\"thickness\"")), Expression(Atom(0)));
    bottomLine.setHead(Atom("list"));
    
    Expression point1bl;
    Expression point2bl;
    point1bl.add_property(Expression(Atom("\"object-name\"")), Expression(Atom("\"point\"")));
    point2bl.add_property(Expression(Atom("\"object-name\"")), Expression(Atom("\"point\"")));
    point1bl.setHead(Atom("list"));
    point1bl.append(Atom(botRightX));
    point1bl.append(Atom(botRightY));
    
    point2bl.setHead(Atom("list"));
    point2bl.append(Atom(topLeftX + 1));
    point2bl.append(Atom(botRightY));
    
    bottomLine.append(point1bl);
    bottomLine.append(point2bl);
    
    outputLine(bottomLine);
}


