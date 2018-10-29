#include "output_widget.hpp"

OutputWidget::OutputWidget(QWidget * parent){
    scene = new QGraphicsScene;
    view = new QGraphicsView(scene);
    // view->setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern));
    view->setFixedSize(500, 200);
    
    auto layout = new QGridLayout;
    layout->addWidget(view, 0, 0);
    setLayout(layout);
}
