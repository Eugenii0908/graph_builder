#include "graph_builder.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    graph_builder w;
    w.show();
    return a.exec();
}
