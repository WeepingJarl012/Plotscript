#ifndef OUTPUT_WIDGET_H
#define OUTPUT_WIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWidget>
#include <QLayout>
#include <QDebug>

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
    
public slots:
    
    void updateOutput(Expression result);
    
    
};

#endif
