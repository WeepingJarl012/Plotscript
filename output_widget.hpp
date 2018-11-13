#ifndef OUTPUT_WIDGET_H
#define OUTPUT_WIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWidget>
#include <QLayout>
#include <QDebug>
#include <QGraphicsTextItem>
#include <iomanip>
#include "interpreter.hpp"

class QGraphicsScene;
class QGraphicsView;

class OutputWidget: public QWidget {
    Q_OBJECT
public:
    
    OutputWidget(QWidget * parent = nullptr);
    
private:
    
    QGraphicsScene * scene;
    QGraphicsView * view;
    QPen * myPen;
    bool outputList;
    
    void createPlot(Expression result);
    void outputResult(Expression result);
    void outputText(Expression result);
    void outputLine(Expression result);
    void outputPoint(Expression result);
    void outputBoundingPlot(double botRightX, double botRightY, double topLeftX, double topLeftY);
    
public slots:
    
    void updateOutput(Expression result);
    void updateOutputError(Expression result);
    
    
};

#endif
