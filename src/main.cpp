#include <QIcon>
#include <QApplication>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/main.ico"));

    return app.exec();
}
