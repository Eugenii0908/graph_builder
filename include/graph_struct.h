#pragma once

#include <vector>
#include <iostream>

// Структура для хранения узлов
struct Link {
    int index;
    int weight;
};

// Структура для хранения информации об узле
struct Node {
    int id;
    std::vector<Link> links; // индексы узлов, куда ведут ребра
};

// Глобальные переменные
extern std::vector<Node> nodes; // список всех узлов

// Добавление узла
// Возвращает индекс узла
int add_node();

// Добавление связи от from к to
// Возвращает true если успешно
int add_link(int from, int to, int weight);
int add_link(int from, int to);

// Удаление узла и всех связанных с ним ребер
bool remove_node(int node);

// Удаление связи от from к to
bool remove_link(int from, int to);

// Очистка графа
void clear_graph();

// Структура для хранения ребра (для алгоритма Беллмана-Форда)
struct Edge {
    int from_index;
    int to_index;
    int weight;
};

// Результат работы алгоритма Беллмана-Форда
struct ShortTrip {
    std::vector<int> distances; // расстояния от start до всех узлов
    std::vector<int> previous; // предыдущие узлы на кратчайшем пути
};

// Поиск кратчайшего пути
ShortTrip searchShortTrip(int start_node, bool checkWeight);

// Восстановление маршрута
std::vector<int> getPathTrip(int start_node, int end_node, bool checkWeight);

// Получение всех ребер графа
std::vector<Edge> get_all_edges();