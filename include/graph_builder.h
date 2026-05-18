#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_graph_builder.h"
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QBrush>
#include <QPen>
#include <QMouseEvent>
#include <QtMath>
#include "graph_struct.h"
#include <QInputDialog>
#include <QMessageBox>

enum Mode { SelectLink, DeleteNode, DeleteLink, SearchTrip, SearchEdge, Move };

class graph_builder : public QMainWindow {
    Q_OBJECT

public:
    graph_builder(QWidget* parent = nullptr);
    ~graph_builder();

private slots:
    // Кнопки
    void onNodeClicked();
    void onLinkClicked();
    void onClearAllClicked();
    void onDeleteNodeClicked();
    void onDeleteLinkClicked();
    void onMoveNodeClicked();
    void onMinTripClicked();
    void onMinNumEdgeClicked();
    void onClearFormatClicked();


private:
    Ui::graph_builderClass ui;
    Graph graph;
    // Обработка мыши
    bool eventFilter(QObject* obj, QEvent* event) override;
    // Выбранные узлы для создания связи
    QGraphicsEllipseItem* selectedNode1;
    QGraphicsEllipseItem* selectedNode2;
    // Режимы
    Mode mode;
  /*  bool isSelectingLink;
    bool isDeletingNode;
    bool isDeletingLink;
    bool isSearchingTrip;
    bool isSearchingEdges;*/
    // Функции операций над графами
    void chooseNodes(QMouseEvent* mouseEvent, std::function<void(QGraphicsEllipseItem* from, QGraphicsEllipseItem* to)>fun);
    void deleteNodeMode(QMouseEvent* mouseEvent);
    void deleteLinkMode(QMouseEvent* mouseEvent);

    // Структура для хранения связанных пар
    struct LinkPair {
        QGraphicsEllipseItem* from;
        QGraphicsEllipseItem* to;
        int weight;
    };
    // Списки пар, линий и стрелок
    QList<LinkPair> linkPairs;
    QList<QGraphicsLineItem*> lines;
    QList<QGraphicsPolygonItem*> arrows;
    QList<QGraphicsTextItem*> weightTexts;

    // Вспомогательные функции отрисовки
    void createLink(QGraphicsEllipseItem* from, QGraphicsEllipseItem* to);
    void drawLink(QGraphicsEllipseItem* from, QGraphicsEllipseItem* to, int weight);
    QPointF getEdgePoint(QPointF center, QPointF otherCenter, qreal radius);
    QPointF getPerpendicularOffset(QPointF p1, QPointF p2, qreal distance);
    void searchTrip(QGraphicsEllipseItem* from, QGraphicsEllipseItem* to);

    // Функция обновления связей
    void updateAllLinks();
    // Функция обновления состояния кнопок
    void updateButtonsState();
    // Флаг наличия форматирования
    bool isFormatted;
};