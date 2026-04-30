#include "graph_struct.h"
#include <algorithm>

// Определение глобальных переменных
std::vector<Node> nodes;

// Добавление узла
// Возвращает индекс узла
int add_node() {
    Node newNode;
    // Ищем первый свободный ID
    int newId = 1;
    bool idFound = false;

    while (!idFound) {
        bool exists = false;
        for (int i = 0; i < nodes.size(); i++) {
            if (nodes[i].id == newId) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            idFound = true;
        }
        else {
            newId++;
        }
    }

    newNode.id = newId;
    nodes.push_back(newNode);
    return newId;
}

// Добавление связи от from к to
// Возвращает true если успешно
int add_link(int from, int to, int weight) {
    int fromIndex = -1, toIndex = -1;

    // Находим индексы узлов
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i].id == from) fromIndex = i;
        if (nodes[i].id == to) toIndex = i;
    }

    // Если нет таких узлов
    if (fromIndex == -1) {
        return -1;
    }
    if (toIndex == -1) {
        return -1;
    }

    // Проверяем, не существует ли уже такая связь
    for (int i = 0; i < nodes[fromIndex].links.size(); i++) {
        if (nodes[fromIndex].links[i].index == toIndex) {
            return -2;
        }
    }
    for (int i = 0; i < nodes[toIndex].links.size(); i++) {
        if (nodes[toIndex].links[i].index == fromIndex) {
            return -3;
        }
    }

    Link pushLink;
    pushLink.index = toIndex;
    pushLink.weight = weight;
    nodes[fromIndex].links.push_back(pushLink);
    return 0;
}

// Добавление связи от from к to
// Возвращает true если успешно
int add_link(int from, int to) {
    int fromIndex = -1, toIndex = -1;

    // Находим индексы узлов
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i].id == from) fromIndex = i;
        if (nodes[i].id == to) toIndex = i;
    }

    // Если нет таких узлов
    if (fromIndex == -1) {
        return -1;
    }
    if (toIndex == -1) {
        return -1;
    }

    // Проверяем, не существует ли уже такая связь
    for (int i = 0; i < nodes[fromIndex].links.size(); i++) {
        if (nodes[fromIndex].links[i].index == toIndex) {
            return -2;
        }
    }
    for (int i = 0; i < nodes[toIndex].links.size(); i++) {
        if (nodes[toIndex].links[i].index == fromIndex) {
            return -3;
        }
    }

    Link pushLink;
    pushLink.index = toIndex;
    pushLink.weight = 1;
    nodes[fromIndex].links.push_back(pushLink);
    return 0;
}

// Удаление узла и всех связанных с ним ребер
bool remove_node(int node) {
    int nodeIndex = -1;

    // Находим индекс узла
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i].id == node) {
            nodeIndex = i;
            break;
        }
    }

    // Если нет узла
    if (nodeIndex == -1) {
        return false;
    }

    // Удаляем все ребра, ведущие к этому узлу
    for (int i = 0; i < nodes.size(); i++) {
        auto& link = nodes[i].links;
        // Проходим по всем связям и удаляем те, что ведут к nodeIndex
        for (int j = 0; j < link.size(); j++) {
            if (link[j].index == nodeIndex) {
                link.erase(link.begin() + j);
                j--; // Уменьшаем счетчик, так как элементы сдвинулись
            }
        }
    }

    // Удаляем узел
    nodes.erase(nodes.begin() + nodeIndex);

    // Обновляем индексы в ребрах (так как после удаления индексы сдвинулись)
    for (int i = 0; i < nodes.size(); i++) {
        for (int j = 0; j < nodes[i].links.size(); j++) {
            if (nodes[i].links[j].index > nodeIndex) {
                nodes[i].links[j].index--;
            }
        }
    }

    return true;
}

// Алгоритм поиска
bool remove_link(int from, int to) {
    int fromIndex = -1, toIndex = -1;

    // Находим индексы узлов
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i].id == from) fromIndex = i;
        if (nodes[i].id == to) toIndex = i;
    }

    if (fromIndex == -1 || toIndex == -1) {
        return false;
    }

    // Ищем и удаляем ребро (ручной поиск)
    auto& link = nodes[fromIndex].links;
    for (int i = 0; i < link.size(); i++) {
        if (link[i].index == toIndex) {
            link.erase(link.begin() + i);  // Удаляем по индексу
            return true;
        }
    }

    return false;
}

// Очистка графа
void clear_graph() {
    nodes.clear();
}

// Поиск кратчайшего пути
ShortTrip searchShortTrip(int start_node, bool checkWeight) {
    ShortTrip result;

    // Находим индекс стартового узла
    int start_index = -1;
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i].id == start_node) {
            start_index = i;
            break;
        }
    }

    int n = nodes.size();
    result.distances.assign(n, INT_MAX);
    result.previous.assign(n, -1);
    result.distances[start_index] = 0;

    // Получаем все ребра
    std::vector<Edge> edges = get_all_edges();

    // Основная фаза: релаксация ребер (n-1 раз)
    for (int i = 1; i <= n - 1; i++) {
        bool changed = false;

        // Проходим по всем ребрам
        for (int j = 0; j < edges.size(); j++) {
            const Edge& edge = edges[j];

            if (result.distances[edge.from_index] != INT_MAX) {
                int new_distance;
                if (checkWeight)
                    new_distance = result.distances[edge.from_index] + edge.weight;
                else
                    new_distance = result.distances[edge.from_index] + 1;

                if (new_distance < result.distances[edge.to_index]) {
                    result.distances[edge.to_index] = new_distance;
                    result.previous[edge.to_index] = edge.from_index;
                    changed = true;
                }
            }
        }
        if (!changed) {
            break;
        }
    }
    return result;
}

// Получение всех ребер графа
std::vector<Edge> get_all_edges() {
    std::vector<Edge> all_edges;

    for (int i = 0; i < nodes.size(); i++) {
        for (int j = 0; j < nodes[i].links.size(); j++) {
            Edge edge;
            edge.from_index = i;
            edge.to_index = nodes[i].links[j].index;
            edge.weight = nodes[i].links[j].weight;
            all_edges.push_back(edge);
        }
    }

    return all_edges;
}

// Восстановление маршрута
std::vector<int> getPathTrip(int start_node, int end_node, bool checkWeight) {
    std::vector<int> path;

    ShortTrip result = searchShortTrip(start_node, checkWeight);

    // Находим индексы узлов
    int start_index = -1, end_index = -1;
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i].id == start_node) start_index = i;
        if (nodes[i].id == end_node) end_index = i;
    }

    // Проверяем, достижим ли конечный узел
    if (result.distances[end_index] == INT_MAX) {
        return path;
    }

    // Восстанавливаем путь
    for (int at = end_index; at != -1; at = result.previous[at]) {
        path.push_back(nodes[at].id);
    }

    std::reverse(path.begin(), path.end());
    return path;
}