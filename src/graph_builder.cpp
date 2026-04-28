#include "graph_builder.h"
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

graph_builder::graph_builder(QWidget* parent)
    : QMainWindow(parent)
    // Инициализируем указатели на выбранные узлы как nullptr (ничего не выбрано)
    , selectedNode1(nullptr)
    , selectedNode2(nullptr)
    , isSelectingLink(false)
    , isDeletingNode(false)
    , isDeletingLink(false)
    , isSearchingTrip(false)
    , isSearchingEdges(false)
    , isFormatted(false)
{
    ui.setupUi(this);
    // Создаём новую графическую сцену, родитель - это окно
    QGraphicsScene* scene = new QGraphicsScene(this);
    scene->setSceneRect(-500, -500, 1000, 1000);
    ui.graphicsView->setScene(scene);

    updateButtonsState();

    // Подключение кнопок
    connect(ui.Node, &QToolButton::clicked, this, &graph_builder::onNodeClicked);
    connect(ui.Link, &QToolButton::clicked, this, &graph_builder::onLinkClicked);
    connect(ui.ClearAll, &QToolButton::clicked, this, &graph_builder::onClearAllClicked);
    connect(ui.DeleteNode, &QPushButton::clicked, this, &graph_builder::onDeleteNodeClicked);
    connect(ui.DeleteLink, &QPushButton::clicked, this, &graph_builder::onDeleteLinkClicked);
    connect(ui.MoveNode, &QPushButton::clicked, this, &graph_builder::onMoveNodeClicked);
    connect(ui.MinTrip, &QPushButton::clicked, this, &graph_builder::onMinTripClicked);
    connect(ui.MinNumEdge, &QPushButton::clicked, this, &graph_builder::onMinNumEdgeClicked);
    connect(ui.ClearFormat, &QPushButton::clicked, this, &graph_builder::onClearFormatClicked);

    // Устанавливаем фильтр событий на область просмотра GraphicsView для обработки использования мыши
    ui.graphicsView->viewport()->installEventFilter(this);
}

graph_builder::~graph_builder()
{
    clear_graph();
}

/////////////////////////////////
//Обработчики кнопок

// Функция кнопки создания узла
void graph_builder::onNodeClicked()
{
    // Выходим из режима создания связи (если были в нём)
    isSelectingLink = false;
    isDeletingNode = false;
    isDeletingLink = false;
    isSearchingTrip = false;
    isSearchingEdges = false;
    if (selectedNode1 != nullptr) {
        selectedNode1->setBrush(QBrush(Qt::white));
        selectedNode1 = nullptr;
    }
    if (selectedNode2 != nullptr) {
        selectedNode2->setBrush(QBrush(Qt::white));
        selectedNode2 = nullptr;
    }
    graph_builder::onClearFormatClicked();

    onMoveNodeClicked();

    // Получаем указатель на сцену из GraphicsView
    QGraphicsScene* scene = ui.graphicsView->scene();

    int newId = add_node();

    // Создание круга
    QGraphicsEllipseItem* circle = new QGraphicsEllipseItem(-20, -20, 40, 40);
    circle->setBrush(QBrush(Qt::white));
    circle->setPen(QPen(Qt::black, 2));
    circle->setPos(0, 0);
    circle->setFlag(QGraphicsItem::ItemIsMovable, true);
    circle->setData(0, newId);

    // Создание текста в круге
    QGraphicsTextItem* text = new QGraphicsTextItem(QString::number(newId), circle);
    text->setDefaultTextColor(Qt::black);
    // Настройка шрифта
    QFont font = text->font();
    font.setPointSize(12);
    text->setFont(font);

    // Центрирование текста
    // Получаем прямоугольник, который занимает текст
    QRectF textRect = text->boundingRect();
    // Смещаем текст так, чтобы его центр совпал с центром кружка
    text->setPos(-textRect.width() / 2, -textRect.height() / 2);

    // Добавляем узел в область
    scene->addItem(circle);

    updateButtonsState();
    statusBar()->showMessage(QString::fromLocal8Bit("Узел %1 создан").arg(newId));
}

// Функция кнопки включения режима перемещения
void graph_builder::onMoveNodeClicked() {
    // Входим в режим перемещения (то есть никакой режим)
    if (selectedNode1 != nullptr && isSearchingTrip != true) {
        selectedNode1->setBrush(QBrush(Qt::white));
        selectedNode1 = nullptr;
    }
    if (selectedNode2 != nullptr && isSearchingTrip != true) {
        selectedNode2->setBrush(QBrush(Qt::white));
        selectedNode2 = nullptr;
    }
    isSelectingLink = false;
    isDeletingNode = false;
    isDeletingLink = false;
    isSearchingTrip = false;
    isSearchingEdges = false;

    ui.MoveNode->setChecked(true);
    ui.Link->setChecked(false);
    ui.DeleteNode->setChecked(false);
}

// Функция кнопки включения режима создания связи
void graph_builder::onLinkClicked()
{
    isSelectingLink = true;
    isDeletingNode = false;
    isDeletingLink = false;
    isSearchingTrip = false;
    isSearchingEdges = false;
    if (selectedNode1 != nullptr) {
        selectedNode1->setBrush(QBrush(Qt::white));
        selectedNode1 = nullptr;
    }
    if (selectedNode2 != nullptr) {
        selectedNode2->setBrush(QBrush(Qt::white));
        selectedNode2 = nullptr;
    }
    graph_builder::onClearFormatClicked();

    statusBar()->showMessage(QString::fromLocal8Bit("Режим создания связи: выберите начальный узел"));
}

// Функция кнопки включения режима удаления узла
void graph_builder::onDeleteNodeClicked()
{
    // Включаем только режим удаления узла
    isDeletingNode = true;
    isSelectingLink = false;
    isDeletingLink = false;
    isSearchingTrip = false;
    isSearchingEdges = false;
    if (selectedNode1 != nullptr) {
        selectedNode1->setBrush(QBrush(Qt::white));
        selectedNode1 = nullptr;
    }
    if (selectedNode2 != nullptr) {
        selectedNode2->setBrush(QBrush(Qt::white));
        selectedNode2 = nullptr;
    }
    graph_builder::onClearFormatClicked();

    statusBar()->showMessage(QString::fromLocal8Bit("Режим удаления: кликните на узел"));
}

// Функция кнопки включения режима удаления связи
void graph_builder::onDeleteLinkClicked()
{
    isDeletingLink = true;
    isSelectingLink = false;
    isDeletingNode = false;
    isSearchingTrip = false;
    isSearchingEdges = false;
    if (selectedNode1 != nullptr) {
        selectedNode1->setBrush(QBrush(Qt::white));
        selectedNode1 = nullptr;
    }
    if (selectedNode2 != nullptr) {
        selectedNode2->setBrush(QBrush(Qt::white));
        selectedNode2 = nullptr;
    }
    graph_builder::onClearFormatClicked();

    statusBar()->showMessage(QString::fromLocal8Bit("Режим удаления связи: кликните на стрелку"));
}

// Функция кнопки удаления графа
void graph_builder::onClearAllClicked() {

    // Очищаем структуру
    clear_graph();

    // Очищаем графическую сцену
    QGraphicsScene* scene = ui.graphicsView->scene();
    scene->clear();

    // Очищаем списки линий и стрелок
    lines.clear();
    arrows.clear();
    linkPairs.clear();
    weightTexts.clear();

    // Сбрасываем переменные состояния
    selectedNode1 = nullptr;
    selectedNode2 = nullptr;
    isSelectingLink = false;
    isDeletingNode = false;
    isDeletingLink = false;
    isSearchingTrip = false;

    onMoveNodeClicked();

    updateButtonsState();
    statusBar()->showMessage(QString::fromLocal8Bit("Граф очищен"));
}

// Функция кнопки удаления форматирования
void graph_builder::onClearFormatClicked() {
    // Очищаем предыдущую подсветку (восстанавливаем цвета)
    QGraphicsScene* scene = ui.graphicsView->scene();
    QList<QGraphicsItem*> allItems = scene->items();
    for (QGraphicsItem* item : allItems) {
        if (item->type() == QGraphicsEllipseItem::Type) {
            QGraphicsEllipseItem* node = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);
            node->setBrush(QBrush(Qt::white));  // Возвращаем белый цвет
        }
        if (item->type() == QGraphicsLineItem::Type) {
            QGraphicsLineItem* line = qgraphicsitem_cast<QGraphicsLineItem*>(item);
            line->setPen(QPen(Qt::black, 2));  // Возвращаем черный цвет
        }
        if (item->type() == QGraphicsPolygonItem::Type) {
            QGraphicsPolygonItem* arrow = qgraphicsitem_cast<QGraphicsPolygonItem*>(item);
            arrow->setBrush(QBrush(Qt::black));  // Возвращаем черный цвет стрелки
            arrow->setPen(QPen(Qt::black, 2));
        }
    }

    // Сброс форматирования текста весов
    for (int j = 0; j < weightTexts.size(); j++) {
        weightTexts[j]->setDefaultTextColor(Qt::black);  // Черный цвет текста
        weightTexts[j]->setFont(QFont("Arial", 10, QFont::Bold));  // Обычный шрифт
    }
    graph_builder::isFormatted = false;
    updateButtonsState();

    statusBar()->showMessage(QString::fromLocal8Bit("Форматирование удалено"));
}

// Функция кнопки поиска кратчайшего пути
void graph_builder::onMinTripClicked() {
    isSelectingLink = false;
    isDeletingNode = false;
    isDeletingLink = false;
    isSearchingTrip = true;
    isSearchingEdges = false;
    graph_builder::onClearFormatClicked();

    statusBar()->showMessage(QString::fromLocal8Bit("Режим поиска кратчайшего пути: выберите начальный узел"));
}

// Функция кнопки поиска наименьшего количества ребер
void graph_builder::onMinNumEdgeClicked() {
    graph_builder::onMinTripClicked();
    isSearchingEdges = true;
}

/////////////////////////////////
//Функции действия

// Обработчик событий
bool graph_builder::eventFilter(QObject* obj, QEvent* event) {
    if (obj == ui.graphicsView->viewport()) {
        // Обновляем стрелки при движении мыши
        if (event->type() == QEvent::MouseMove) {
            updateAllLinks();
        }

        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

            // Правая кнопка - отмена текущего режима
            if ((mouseEvent->button() == Qt::RightButton) && (isSelectingLink || isDeletingNode || isDeletingLink || isSearchingTrip))
            {
                isSelectingLink = false;
                isDeletingNode = false;
                isDeletingLink = false;
                isSearchingTrip = false;
                isSearchingEdges = false;

                // Проверяем, что указатели не nullptr, прежде чем менять цвет
                if (selectedNode1 != nullptr) {
                    selectedNode1->setBrush(QBrush(Qt::white));
                    selectedNode1 = nullptr;
                }
                if (selectedNode2 != nullptr) {
                    selectedNode2->setBrush(QBrush(Qt::white));
                    selectedNode2 = nullptr;
                }
                graph_builder::onClearFormatClicked();

                // Переключаемся в режим перемещения
                onMoveNodeClicked();

                statusBar()->showMessage(QString::fromLocal8Bit("Операция отменена"));
                return true;
            }

            // Левая кнопка - действия в зависимости от режима
            if (mouseEvent->button() == Qt::LeftButton)
            {
                if (isSelectingLink) {
                    chooseNodes(mouseEvent, [this](QGraphicsEllipseItem* from, QGraphicsEllipseItem* to) {
                        createLink(from, to);
                        });
                    return true;
                }

                if (isDeletingNode) {
                    deleteNodeMode(mouseEvent);
                    return true;
                }

                if (isDeletingLink) {
                    deleteLinkMode(mouseEvent);
                    return true;
                }

                if (isSearchingTrip) {
                    chooseNodes(mouseEvent, [this](QGraphicsEllipseItem* from, QGraphicsEllipseItem* to) {
                        searchTrip(from, to);
                        });
                    return true;
                }
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

// Функция создания связи
void graph_builder::createLink(QGraphicsEllipseItem* from, QGraphicsEllipseItem* to) {
    // Извлекаем ID узлов из кружков
    int fromId = from->data(0).toInt();
    int toId = to->data(0).toInt();

    int res = add_link(fromId, toId);
    switch (res) {
    case -2: {
        statusBar()->showMessage(QString::fromLocal8Bit("Ошибка: связь уже существует"));
        QMessageBox::critical(this,
            QString::fromLocal8Bit("Ошибка!"),
            QString::fromLocal8Bit("Связь уже существует."));
        selectedNode1->setBrush(QBrush(Qt::white));
        selectedNode2->setBrush(QBrush(Qt::white));
        return;
    }
    case -3: {
        statusBar()->showMessage(QString::fromLocal8Bit("Ошибка: создание циклической связи не разрешено"));
        QMessageBox::critical(this,
            QString::fromLocal8Bit("Ошибка!"),
            QString::fromLocal8Bit("Создание циклической связи не разрешено."));
        selectedNode1->setBrush(QBrush(Qt::white));
        selectedNode2->setBrush(QBrush(Qt::white));
        return;
    }
    }
    remove_link(fromId, toId);
    // Запрашиваем вес у пользователя
    bool ok;
    int weight = QInputDialog::getInt(this,
        QString::fromLocal8Bit("Вес связи"),
        QString::fromLocal8Bit("Введите вес связи:"),
        1,  // значение по умолчанию
        1,  // минимум
        1000,  // максимум
        1,  // шаг
        &ok);

    if (!ok) {
        statusBar()->showMessage(QString::fromLocal8Bit("Создание связи отменено"));
        return;
    }

    // Добавляем связь в структуру
    add_link(fromId, toId, weight);

    // Рисуем связь
    drawLink(from, to, weight);

    // Сохраняем пару узлов для последующего обновления
    LinkPair pair;
    pair.from = from;
    pair.to = to;
    pair.weight = weight;
    linkPairs.append(pair);

    updateButtonsState();
    selectedNode1->setBrush(QBrush(Qt::white));
    selectedNode2->setBrush(QBrush(Qt::white));
    statusBar()->showMessage(QString::fromLocal8Bit("Связь %1 -> %2 создана").arg(fromId).arg(toId));
}

// Вспомогательная функция: вычисляет точку на краю круга
QPointF graph_builder::getEdgePoint(QPointF center, QPointF otherCenter, qreal radius) {
    // Вектор от центра к другому центру
    QPointF direction = otherCenter - center;
    // Вычисляем угол направления
    qreal angle = atan2(direction.y(), direction.x());
    // Точка на краю круга
    QPointF edgePoint(center.x() + radius * cos(angle),
        center.y() + radius * sin(angle));
    return edgePoint;
}

QPointF graph_builder::getPerpendicularOffset(QPointF p1, QPointF p2, qreal distance) {
    // Направление линии
    QPointF direction = p2 - p1;
    qreal length = sqrt(direction.x() * direction.x() + direction.y() * direction.y());

    if (length == 0) return p1;

    // Единичный вектор направления
    QPointF unitDir = direction / length;

    // Перпендикулярный вектор (поворот на 90 градусов)
    QPointF perpendicular(-unitDir.y(), unitDir.x());

    // Середина линии
    QPointF midPoint = (p1 + p2) / 2;

    // Смещаем на заданное расстояние по перпендикуляру
    return midPoint + perpendicular * distance;
}

// Вспомогательная функция отрисовки связи
void graph_builder::drawLink(QGraphicsEllipseItem* from, QGraphicsEllipseItem* to, int weight) {
    QGraphicsScene* scene = ui.graphicsView->scene();

    qreal radius = 20;

    QPointF fromCenter = from->pos();
    QPointF toCenter = to->pos();

    QPointF fromEdge = getEdgePoint(fromCenter, toCenter, radius);
    QPointF toEdge = getEdgePoint(toCenter, fromCenter, radius);

    // Рисуем линию
    QGraphicsLineItem* line = new QGraphicsLineItem(QLineF(fromEdge, toEdge));
    line->setPen(QPen(Qt::black, 2));
    scene->addItem(line);
    lines.append(line);

    // Рисуем стрелку
    QPointF direction = toEdge - fromEdge;
    qreal length = sqrt(direction.x() * direction.x() + direction.y() * direction.y());

    if (length > 0) {
        QPointF unitVector = direction / length;
        qreal arrowSize = 10;
        QPointF arrowP1 = toEdge - unitVector * arrowSize;
        QPointF perpendicular(-unitVector.y(), unitVector.x());
        QPointF arrowLeft = arrowP1 - perpendicular * (arrowSize / 2);
        QPointF arrowRight = arrowP1 + perpendicular * (arrowSize / 2);

        QPolygonF arrowHead;
        arrowHead << toEdge << arrowLeft << arrowRight;

        QGraphicsPolygonItem* arrowItem = scene->addPolygon(arrowHead, QPen(Qt::black, 2), QBrush(Qt::black));
        arrows.append(arrowItem);
    }

    // Рисуем вес
    QPointF textPos = getPerpendicularOffset(fromEdge, toEdge, 10);

    QGraphicsTextItem* weightText = new QGraphicsTextItem(QString::number(weight));
    weightText->setDefaultTextColor(Qt::black);
    weightText->setFont(QFont("Arial", 10, QFont::Bold));

    // Сразу центрируем
    QRectF textRect = weightText->boundingRect();
    weightText->setPos(textPos.x() - textRect.width() / 2,
        textPos.y() - textRect.height() / 2);

    // Сохраняем также саму позицию для текста (без центрирования)
    weightText->setData(0, textPos.x());  // Сохраняем базовую позицию X
    weightText->setData(1, textPos.y());  // Сохраняем базовую позицию Y

    weightTexts.append(weightText);
    scene->addItem(weightText);
}

// Функция выбора связываемых узлов
void graph_builder::chooseNodes(QMouseEvent* mouseEvent, std::function<void(QGraphicsEllipseItem* from, QGraphicsEllipseItem* to)>fun) {
    QPointF scenePos = ui.graphicsView->mapToScene(mouseEvent->pos());
    QGraphicsScene* scene = ui.graphicsView->scene();

    // Ищем узел под курсором
    QGraphicsItem* item = nullptr;
    QList<QGraphicsItem*> items = scene->items(QRectF(scenePos.x() - 20, scenePos.y() - 20, 40, 40));

    for (QGraphicsItem* i : items) {
        if (i->type() == QGraphicsEllipseItem::Type) {
            item = i;
            break;
        }
    }

    // Если нашли кружок
    if (item && item->type() == QGraphicsEllipseItem::Type) {
        QGraphicsEllipseItem* clickedNode = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);

        // Выбор узлов
        if (selectedNode1 == nullptr) {
            selectedNode1 = clickedNode;
            selectedNode1->setBrush(QBrush(Qt::green));
            statusBar()->showMessage(QString::fromLocal8Bit("Выберите конечный узел"));
        }
        else if (selectedNode2 == nullptr && clickedNode != selectedNode1) {
            selectedNode2 = clickedNode;
            selectedNode2->setBrush(QBrush(Qt::green));
            fun(selectedNode1, selectedNode2);

            selectedNode1 = nullptr;
            selectedNode2 = nullptr;
        }
    }
}

// Функция удаления узла
void graph_builder::deleteNodeMode(QMouseEvent* mouseEvent) {
    QPointF scenePos = ui.graphicsView->mapToScene(mouseEvent->pos());
    QGraphicsScene* scene = ui.graphicsView->scene();

    // Ищем узел под курсором
    QGraphicsItem* item = nullptr;
    QList<QGraphicsItem*> items = scene->items(QRectF(scenePos.x() - 20, scenePos.y() - 20, 40, 40));

    for (QGraphicsItem* i : items) {
        if (i->type() == QGraphicsEllipseItem::Type) {
            item = i;
            break;
        }
    }

    if (item && item->type() == QGraphicsEllipseItem::Type) {
        QGraphicsEllipseItem* nodeToDelete = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);
        int nodeId = nodeToDelete->data(0).toInt();

        // Удаляем из структуры
        remove_node(nodeId);

        // Удаляем все линии и стрелки, связанные с узлом
        for (int i = linkPairs.size() - 1; i >= 0; i--) {
            if (linkPairs[i].from == nodeToDelete || linkPairs[i].to == nodeToDelete) {
                if (i < lines.size()) {
                    scene->removeItem(lines[i]);
                    delete lines[i];
                    lines.removeAt(i);
                }
                if (i < arrows.size()) {
                    scene->removeItem(arrows[i]);
                    delete arrows[i];
                    arrows.removeAt(i);
                }
                // Удаляем текст веса
                if (i < weightTexts.size()) {
                    scene->removeItem(weightTexts[i]);
                    delete weightTexts[i];
                    weightTexts.removeAt(i);
                }
                linkPairs.removeAt(i);
            }
        }

        // Удаляем узел
        scene->removeItem(nodeToDelete);
        delete nodeToDelete;

        updateButtonsState();
        statusBar()->showMessage(QString::fromLocal8Bit("Узел %1 удалён").arg(nodeId));
    }
    else {
        statusBar()->showMessage(QString::fromLocal8Bit("Кликните на узел для удаления"));
    }
}

// Функция удаления связи
void graph_builder::deleteLinkMode(QMouseEvent* mouseEvent) {
    QPointF scenePos = ui.graphicsView->mapToScene(mouseEvent->pos());
    QGraphicsScene* scene = ui.graphicsView->scene();

    // Ищем линию (стрелку) под курсором
    QGraphicsItem* item = nullptr;
    QList<QGraphicsItem*> items = scene->items(QRectF(scenePos.x() - 10, scenePos.y() - 10, 20, 20));

    for (QGraphicsItem* i : items) {
        if (i->type() == QGraphicsLineItem::Type || i->type() == QGraphicsPolygonItem::Type) {
            item = i;
            break;
        }
    }

    if (item) {
        // Находим индекс удаляемой связи
        int linkIndex = -1;

        // Проверяем, линия это или стрелка
        if (item->type() == QGraphicsLineItem::Type) {
            for (int i = 0; i < lines.size(); i++) {
                if (lines[i] == item) {
                    linkIndex = i;
                    break;
                }
            }
        }
        else if (item->type() == QGraphicsPolygonItem::Type) {
            for (int i = 0; i < arrows.size(); i++) {
                if (arrows[i] == item) {
                    linkIndex = i;
                    break;
                }
            }
        }

        if (linkIndex != -1 && linkIndex < linkPairs.size()) {
            // Получаем ID узлов из структуры
            int fromId = linkPairs[linkIndex].from->data(0).toInt();
            int toId = linkPairs[linkIndex].to->data(0).toInt();

            // Удаляем из структуры данных
            remove_link(fromId, toId);

            // Удаляем линию
            if (linkIndex < lines.size()) {
                scene->removeItem(lines[linkIndex]);
                delete lines[linkIndex];
                lines.removeAt(linkIndex);
            }

            // Удаляем стрелку
            if (linkIndex < arrows.size()) {
                scene->removeItem(arrows[linkIndex]);
                delete arrows[linkIndex];
                arrows.removeAt(linkIndex);
            }

            // Удаляем пару узлов
            linkPairs.removeAt(linkIndex);

            // Удаляем текст веса
            if (linkIndex < weightTexts.size()) {
                scene->removeItem(weightTexts[linkIndex]);
                delete weightTexts[linkIndex];
                weightTexts.removeAt(linkIndex);
            }

            updateButtonsState();
            statusBar()->showMessage(QString::fromLocal8Bit("Связь %1 -> %2 удалена").arg(fromId).arg(toId));
        }
    }
    else {
        statusBar()->showMessage(QString::fromLocal8Bit("Кликните на стрелку для удаления связи"));
    }
}

// Функция обновления связей
void graph_builder::updateAllLinks() {
    for (int i = 0; i < linkPairs.size(); i++) {
        QGraphicsEllipseItem* from = linkPairs[i].from;
        QGraphicsEllipseItem* to = linkPairs[i].to;

        if (from && to) {
            qreal radius = 20;
            QPointF fromEdge = getEdgePoint(from->pos(), to->pos(), radius);
            QPointF toEdge = getEdgePoint(to->pos(), from->pos(), radius);

            // Обновляем линию
            if (i < lines.size()) {
                lines[i]->setLine(QLineF(fromEdge, toEdge));
            }

            // Обновляем стрелку
            if (i < arrows.size()) {
                QPointF direction = toEdge - fromEdge;
                qreal length = sqrt(direction.x() * direction.x() + direction.y() * direction.y());

                if (length > 0) {
                    QPointF unitVector = direction / length;
                    qreal arrowSize = 10;
                    QPointF arrowP1 = toEdge - unitVector * arrowSize;
                    QPointF perpendicular(-unitVector.y(), unitVector.x());
                    QPointF arrowLeft = arrowP1 - perpendicular * (arrowSize / 2);
                    QPointF arrowRight = arrowP1 + perpendicular * (arrowSize / 2);

                    QPolygonF arrowHead;
                    arrowHead << toEdge << arrowLeft << arrowRight;
                    arrows[i]->setPolygon(arrowHead);
                }
            }

            // Обновляем позицию текста веса (без пересчета boundingRect)
            if (i < weightTexts.size()) {
                QPointF textPos = getPerpendicularOffset(fromEdge, toEdge, 10);
                QRectF textRect = weightTexts[i]->boundingRect();
                weightTexts[i]->setPos(textPos.x() - textRect.width() / 2,
                    textPos.y() - textRect.height() / 2);
            }
        }
    }
}

// Функция поиска кратчайшего пути
void graph_builder::searchTrip(QGraphicsEllipseItem* from, QGraphicsEllipseItem* to) {
    // Извлекаем ID узлов из кружков
    int fromId = from->data(0).toInt();
    int toId = to->data(0).toInt();

    // Получаем путь
    std::vector<int> path = getPathTrip(fromId, toId, !isSearchingEdges);
    if (path.empty()) {
        statusBar()->showMessage(QString::fromLocal8Bit("Путь не найден"));
        QMessageBox::critical(this,
            QString::fromLocal8Bit("Ошибка!"),
            QString::fromLocal8Bit("Путь не найден. Возможно, у узлов нет связи друг с другом."));
        selectedNode1->setBrush(QBrush(Qt::white));
        selectedNode2->setBrush(QBrush(Qt::white));
        return;
    }

    // Получаем сцену
    QGraphicsScene* scene = ui.graphicsView->scene();
    QList<QGraphicsItem*> allItems = scene->items();
    graph_builder::onClearFormatClicked();

    int sumWeight = 0;

    // Подсвечиваем новый путь (path - это vector<int> с ID узлов)
    for (int i = 0; i < path.size(); i++) {
        // Подсветка узлов
        for (QGraphicsItem* item : allItems) {
            if (item->type() == QGraphicsEllipseItem::Type) {
                QGraphicsEllipseItem* node = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);
                if (node->data(0).toInt() == path[i]) {
                    // Начальный и конечный узлы - зеленым
                    if (i == 0 || i == path.size() - 1) {
                        node->setBrush(QBrush(Qt::green));
                    }
                    // Промежуточные узлы - желтым (опционально)
                    else {
                        node->setBrush(QBrush(Qt::red));
                    }
                    break;
                }
            }
        }

        // Подсветка связи между текущим и следующим узлом
        if (i < path.size() - 1) {
            int fromId = path[i];
            int toId = path[i + 1];

            // Ищем линию и стрелку между этими узлами
            for (int j = 0; j < linkPairs.size(); j++) {
                int lineFromId = linkPairs[j].from->data(0).toInt();
                int lineToId = linkPairs[j].to->data(0).toInt();

                if ((lineFromId == fromId && lineToId == toId) ||
                    (lineFromId == toId && lineToId == fromId)) {
                    // Подсветка линии
                    if (j < lines.size()) {
                        lines[j]->setPen(QPen(Qt::red, 3));  // Красная жирная линия
                    }
                    // Подсветка стрелки
                    if (j < arrows.size()) {
                        arrows[j]->setBrush(QBrush(Qt::red));  // Красная стрелка
                        arrows[j]->setPen(QPen(Qt::red, 2));
                    }
                    // Подсветка текста веса
                    if (j < weightTexts.size()) {
                        weightTexts[j]->setDefaultTextColor(Qt::red);  // Красный текст
                        weightTexts[j]->setFont(QFont("Arial", 10, QFont::Bold));  // Жирный шрифт
                    }
                    if (isSearchingEdges)
                        sumWeight++;
                    else
                        sumWeight += linkPairs[j].weight;
                    break;
                }
            }
        }
    }
    graph_builder::isFormatted = true;
    isSearchingTrip = false;
    ui.MoveNode->setChecked(true);
    updateButtonsState();

    if (isSearchingEdges) {
        statusBar()->showMessage(QString::fromLocal8Bit("Наименьшее число рёбер найдено"));
        QMessageBox::information(this,
            QString::fromLocal8Bit("Наименьшее число рёбер найдено!"),
            QString::fromLocal8Bit("Количество рёбер составляет: %1").arg(sumWeight));
    }
    else {
        statusBar()->showMessage(QString::fromLocal8Bit("Кратчайший путь найден"));
        QMessageBox::information(this,
            QString::fromLocal8Bit("Кратчайший путь найден!"),
            QString::fromLocal8Bit("Длина пути составляет: %1").arg(sumWeight));
    }
}

// Функция обновления состояния кнопок
void graph_builder::updateButtonsState() {
    bool hasNodes = (nodes.size() > 0);
    bool hasTwoNodes = (nodes.size() > 1);
    bool hasLinks = (linkPairs.size() > 0);

    ui.DeleteNode->setEnabled(hasNodes);
    ui.Link->setEnabled(hasTwoNodes);
    ui.DeleteLink->setEnabled(hasLinks);
    ui.ClearAll->setEnabled(hasNodes);
    ui.MoveNode->setEnabled(hasNodes);
    ui.MinTrip->setEnabled(hasLinks);
    ui.MinNumEdge->setEnabled(hasLinks);
    ui.ClearFormat->setEnabled(isFormatted);
}