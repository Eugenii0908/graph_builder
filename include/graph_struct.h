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

// Алгоритм Беллмана-Форда
// Алгоритм поиска
ShortTrip searchShortTrip(int start_node, bool checkWeight);
// Восстановление маршрута
std::vector<int> getPathTrip(int start_node, int end_node, bool checkWeight);
void print_shortest_path_bf(int start_node, int end_node, bool checkWeight);

// ============ ФУНКЦИИ ДЛЯ ВЫВОДА ПРОМЕЖУТОЧНЫХ ДАННЫХ БЕЛЛМАНА-ФОРДА ============

void print_bellman_ford_start(int start_node, int start_index, int total_nodes, int total_edges);
void print_bellman_ford_iteration_header(int iteration);
void print_bellman_ford_iteration_result(int iteration, const std::vector<int>& distances, bool changed);
void print_bellman_ford_relaxation(const Edge& edge, int old_distance, int new_distance);
void print_bellman_ford_early_stop();
void print_bellman_ford_negative_cycle_check_start();
void print_bellman_ford_negative_cycle_detected();
void print_bellman_ford_no_negative_cycle();
void print_bellman_ford_final_distances(const std::vector<int>& distances, const std::vector<int>& previous);
void print_bellman_ford_separator();
void print_bellman_ford_path_header(int start_node, int end_node);
void print_bellman_ford_path(const std::vector<int>& path, int total_distance);
void print_bellman_ford_no_path(int start_node, int end_node);
void print_bellman_ford_error_node_not_exist(int node);
void print_bellman_ford_error_negative_cycle();

// ============ ФУНКЦИИ ДЛЯ ВЫВОДА ОБЩЕЙ ИНФОРМАЦИИ О ГРАФЕ ============

void print_graph_header();
void print_node_list();
void print_edge_list();
void print_adjacency_matrix();
void print_graph_footer();
void print_edge_details(int from_id, int to_id, int weight);
void print_node_count(int count);
void print_edge_count(int count);

std::vector<Edge> get_all_edges();
int get_edge_count();