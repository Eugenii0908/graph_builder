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

void clear_graph() {
    nodes.clear();
}

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

    print_bellman_ford_start(start_node, start_index, n, edges.size());

    // Основная фаза: релаксация ребер (n-1 раз)
    for (int i = 1; i <= n - 1; i++) {
        bool changed = false;
        print_bellman_ford_iteration_header(i);

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
                    print_bellman_ford_relaxation(edge, result.distances[edge.to_index], new_distance);

                    result.distances[edge.to_index] = new_distance;
                    result.previous[edge.to_index] = edge.from_index;
                    changed = true;
                }
            }
        }

        print_bellman_ford_iteration_result(i, result.distances, changed);

        if (!changed) {
            print_bellman_ford_early_stop();
            break;
        }
    }

    print_bellman_ford_final_distances(result.distances, result.previous);
    print_bellman_ford_separator();

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
        print_bellman_ford_no_path(start_node, end_node);
        return path;
    }

    // Восстанавливаем путь
    for (int at = end_index; at != -1; at = result.previous[at]) {
        path.push_back(nodes[at].id);
    }

    std::reverse(path.begin(), path.end());
    return path;
}

void print_shortest_path_bf(int start_node, int end_node, bool checkWeight) {
    print_bellman_ford_path_header(start_node, end_node);

    std::vector<int> path = getPathTrip(start_node, end_node, checkWeight);

    if (path.empty()) {
        print_bellman_ford_no_path(start_node, end_node);
        print_bellman_ford_separator();
        return;
    }

    // Находим расстояние
    int end_index = -1;
    for (size_t i = 0; i < nodes.size(); i++) {
        if (nodes[i].id == end_node) {
            end_index = i;
            break;
        }
    }

    ShortTrip result = searchShortTrip(start_node, checkWeight);

    print_bellman_ford_path(path, result.distances[end_index]);

    print_bellman_ford_separator();
}



// ============ ФУНКЦИИ ДЛЯ ВЫВОДА ПРОМЕЖУТОЧНЫХ ДАННЫХ БЕЛЛМАНА-ФОРДА ============

void print_bellman_ford_start(int start_node, int start_index, int total_nodes, int total_edges) {
    std::cout << "\n========== BELLMAN-FORD ALGORITHM ==========\n";
    std::cout << "Start node: " << start_node << " (index: " << start_index << ")\n";
    std::cout << "Total nodes: " << total_nodes << "\n";
    std::cout << "Total edges: " << total_edges << "\n";
    std::cout << "=============================================\n\n";
}

void print_bellman_ford_iteration_header(int iteration) {
    std::cout << "Iteration " << iteration << ":\n";
}

void print_bellman_ford_iteration_result(int iteration, const std::vector<int>& distances, bool changed) {
    std::cout << "  Result after iteration " << iteration << ":\n";
    std::cout << "  Distances: ";
    for (size_t i = 0; i < nodes.size(); i++) {
        if (distances[i] == INT_MAX) {
            std::cout << nodes[i].id << ":INF ";
        }
        else {
            std::cout << nodes[i].id << ":" << distances[i] << " ";
        }
    }
    std::cout << "\n";
    std::cout << "  Changes made: " << (changed ? "YES" : "NO") << "\n";
}

void print_bellman_ford_relaxation(const Edge& edge, int old_distance, int new_distance) {
    std::cout << "  Relaxing edge: " << nodes[edge.from_index].id
        << " -> " << nodes[edge.to_index].id
        << " (weight=" << edge.weight << ")\n";

    if (old_distance == INT_MAX) {
        std::cout << "    Distance to " << nodes[edge.to_index].id
            << " improved from INF to " << new_distance << "\n";
    }
    else {
        std::cout << "    Distance to " << nodes[edge.to_index].id
            << " improved from " << old_distance
            << " to " << new_distance << "\n";
    }
}

void print_bellman_ford_early_stop() {
    std::cout << "  No changes in this iteration - early stop!\n";
}

void print_bellman_ford_negative_cycle_check_start() {
    std::cout << "\nChecking for negative cycles...\n";
}

void print_bellman_ford_negative_cycle_detected() {
    std::cout << "\n  ⚠️  WARNING: Negative cycle detected! ⚠️\n";
    std::cout << "  The graph contains a negative cycle.\n";
    std::cout << "  Shortest path may not be well-defined.\n\n";
}

void print_bellman_ford_no_negative_cycle() {
    std::cout << "No negative cycles detected.\n";
}

void print_bellman_ford_final_distances(const std::vector<int>& distances, const std::vector<int>& previous) {
    std::cout << "\n";
    for (size_t i = 0; i < nodes.size(); i++) {
        std::cout << "Node " << nodes[i].id << ": ";
        if (distances[i] == INT_MAX) {
            std::cout << "INF";
        }
        else {
            std::cout << distances[i];
        }
        std::cout << " (previous: ";
        if (previous[i] == -1) {
            std::cout << "none";
        }
        else {
            std::cout << nodes[previous[i]].id;
        }
        std::cout << ")\n";
    }
}

void print_bellman_ford_separator() {
    std::cout << "=============================================\n\n";
}

void print_bellman_ford_path_header(int start_node, int end_node) {
    std::cout << "\n========== SHORTEST PATH (BELLMAN-FORD) ==========\n";
    std::cout << "From: " << start_node << " To: " << end_node << "\n";
}

void print_bellman_ford_path(const std::vector<int>& path, int total_distance) {
    std::cout << "\nPath: ";
    for (size_t i = 0; i < path.size(); i++) {
        std::cout << path[i];
        if (i < path.size() - 1) std::cout << " -> ";
    }
    std::cout << "\nTotal distance: " << total_distance << "\n";
}

void print_bellman_ford_no_path(int start_node, int end_node) {
    std::cout << "No path found from " << start_node << " to " << end_node << "!\n";
}

void print_bellman_ford_error_node_not_exist(int node) {
    std::cout << "Error: Node " << node << " does not exist!\n";
}

void print_bellman_ford_error_negative_cycle() {
    std::cout << "Cannot find path due to negative cycle!\n";
}

// ============ ФУНКЦИИ ДЛЯ ВЫВОДА ОБЩЕЙ ИНФОРМАЦИИ О ГРАФЕ ============

void print_graph_header() {
    std::cout << "\n=== DIRECTED GRAPH ===\n";
}

void print_node_list() {
    std::cout << "Nodes: ";
    for (size_t i = 0; i < nodes.size(); i++) {
        std::cout << nodes[i].id << " ";
    }
    std::cout << "\n";
}

void print_edge_list() {
    std::cout << "\nEdges:\n";
    bool has_edges = false;
    for (size_t i = 0; i < nodes.size(); i++) {
        for (size_t j = 0; j < nodes[i].links.size(); j++) {
            std::cout << "  " << nodes[i].id << " -> " << nodes[nodes[i].links[j].index].id
                << " (weight: " << nodes[i].links[j].weight << ")\n";
            has_edges = true;
        }
    }

    if (!has_edges) {
        std::cout << "  No edges\n";
    }
}

void print_adjacency_matrix() {
    if (nodes.size() <= 15 && nodes.size() > 0) {
        std::cout << "\nAdjacency matrix:\n   ";
        for (size_t i = 0; i < nodes.size(); i++) {
            std::cout << nodes[i].id << " ";
        }
        std::cout << "\n";

        for (size_t i = 0; i < nodes.size(); i++) {
            std::cout << nodes[i].id << "  ";
            for (size_t j = 0; j < nodes.size(); j++) {
                bool hasEdge = false;
                for (size_t k = 0; k < nodes[i].links.size(); k++) {
                    if (nodes[i].links[k].index == (int)j) {
                        hasEdge = true;
                        break;
                    }
                }
                std::cout << (hasEdge ? "1 " : "0 ");
            }
            std::cout << "\n";
        }
    }
}

void print_graph_footer() {
    std::cout << "=====================\n";
}

void print_edge_details(int from_id, int to_id, int weight) {
    std::cout << "  " << from_id << " -> " << to_id << " (weight: " << weight << ")\n";
}

void print_node_count(int count) {
    std::cout << "Number of nodes: " << count << "\n";
}

void print_edge_count(int count) {
    std::cout << "Number of edges: " << count << "\n";
}

// ============ ОБНОВЛЕННАЯ ФУНКЦИЯ PRINT_GRAPH ============

void print_graph() {
    print_graph_header();
    print_node_count(nodes.size());
    print_edge_count(get_edge_count());
    std::cout << "\n";

    if (nodes.empty()) {
        std::cout << "Graph is empty\n";
        print_graph_footer();
        return;
    }

    print_node_list();
    print_edge_list();
    print_adjacency_matrix();
    print_graph_footer();
}

// ============ ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ДЛЯ БЕЛЛМАНА-ФОРДА ============


int get_edge_count() {
    int count = 0;
    for (size_t i = 0; i < nodes.size(); i++) {
        count += nodes[i].links.size();
    }
    return count;
}