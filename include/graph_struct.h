#pragma once

#include <vector>

// Структура для хранения связей
struct Link {
    int index;  // Индекс узла назначения
    int weight;  // Вес связи
};

// Структура для хранения ребра (для алгоритма Беллмана-Форда)
struct Edge {
    int from_index;
    int to_index;
    int weight;
};

// Результат работы алгоритма Беллмана-Форда
struct ShortTrip {
    std::vector<int> distances;  // расстояния от start до всех узлов
    std::vector<int> previous;  // предыдущие узлы на кратчайшем пути
};

// Класс графа
class Graph {
private:
    struct Node {
        int id;
        std::vector<Link> links;
    };

    std::vector<Edge> get_all_edges();
    ShortTrip searchShortTrip(int start_node, bool checkWeight);

public:
    std::vector<Node> nodes;
    // Добавление узла
    int add_node();

    // Добавление связи
    int add_link(int from, int to, int weight);
    int add_link(int from, int to);

    // Удаление
    bool remove_node(int node);
    bool remove_link(int from, int to);
    void clear_graph();

    // Поиск путей
    std::vector<int> getPathTrip(int start_node, int end_node, bool checkWeight);


    // Вспомогательные функции
    int get_node_count() {
        return nodes.size();
    }

    int get_link_count() const {
        int count = 0;
        for (const auto& node : nodes) {
            count += node.links.size();
        }
        return count;
    }
};