#ifndef OUTPUT_WIDGET_H
#define OUTPUT_WIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWidget>
#include <QLayout>

class QGraphicsScene;
class QGraphicsView;

class OutputWidget: public QWidget {
    Q_OBJECT
public:
    
    OutputWidget(QWidget * parent = nullptr);
    
private:
    
    QGraphicsScene * scene;
    QGraphicsView * view;
    
    private slots:
    
    
};

#endif
