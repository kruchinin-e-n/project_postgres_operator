#include "mainwindow.h"

#include <QApplication>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <iostream>
#include <pqxx/pqxx>
#include <memory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QWidget window;
    window.setWindowTitle("Postgre connector");
    window.setMinimumWidth(1350);
    window.setMinimumHeight(850);
    window.setStyleSheet("QWidget { background-color : rgb(235,235,235); color : black; font-size:40px}");

    std::string dbname;

    QLabel dbname_label{"Database:", &window};
    dbname_label.setGeometry(0, 50, 300, 50 );
    dbname_label.setAlignment(Qt::AlignRight);
    dbname_label.setStyleSheet("QLabel { background-color : rgb(235,235,235); color : black; font-size:40px}");

    QLineEdit dbname_field(&window);
    dbname_field.setGeometry(350, 50, 400, 50 );
    dbname_field.setStyleSheet("QLineEdit { background-color : black; color : white; font-size:40px; border-radius: 15px; }");

    std::string user;

    QLabel user_label{"User:", &window};
    user_label.setGeometry(0, 150, 300, 50 );
    user_label.setAlignment(Qt::AlignRight);
    user_label.setStyleSheet("QLabel { background-color : rgb(235,235,235); color : black; font-size:40px}");

    QLineEdit user_field(&window);
    user_field.setGeometry(350, 150, 400, 50 );
    user_field.setStyleSheet("QLineEdit { background-color : black; color : white; font-size:40px; border-radius: 15px; }");

    std::string password;

    QLabel password_label{"Password:", &window};
    password_label.setGeometry(0, 250, 300, 50 );
    password_label.setAlignment(Qt::AlignRight);
    password_label.setStyleSheet("QLabel { background-color : rgb(235,235,235); color : black; font-size:40px}");

    QLineEdit password_field(&window);
    password_field.setGeometry(350, 250, 400, 50 );
    password_field.setStyleSheet("QLineEdit { background-color : black; color : black; font-size:40px; border-radius: 15px; }");

    std::string hostaddr;

    QLabel hostaddr_label{"Host address:", &window};
    hostaddr_label.setGeometry(0, 350, 300, 50 );
    hostaddr_label.setAlignment(Qt::AlignRight);
    hostaddr_label.setStyleSheet("QLabel { background-color : rgb(235,235,235); color : black; font-size:40px}");

    QLineEdit hostaddr_field(&window);
    hostaddr_field.setGeometry(350, 350, 400, 50 );
    hostaddr_field.setStyleSheet("QLineEdit { background-color : black; color : white; font-size:40px; border-radius: 15px; }");

    std::string port;

    QLabel port_label{"Port:", &window};
    port_label.setGeometry(0, 450, 300, 50 );
    port_label.setAlignment(Qt::AlignRight);
    port_label.setStyleSheet("QLabel { background-color : rgb(235,235,235); color : black; font-size:40px}");

    QLineEdit port_field(&window);
    port_field.setGeometry(350, 450, 400, 50 );
    port_field.setStyleSheet("QLineEdit { background-color : black; color : white; font-size:40px; border-radius: 15px; }");

    QPushButton connect_button("Connect", &window);
    connect_button.setGeometry(350, 550, 400, 50 );
    connect_button.setStyleSheet("QPushButton { background-color : green; color : black; font-size:40px; border-radius: 15px; }");

    QLabel sucess_label{"", &window};
    sucess_label.setGeometry(800, 50, 500, 650 );
    sucess_label.setAlignment(Qt::AlignLeft);
    sucess_label.setStyleSheet("QLabel { background-color : white; color : black; font-size:40px; border: 2px solid gray; border-radius: 15px; }");

    QLabel open_notification{"", &window};
    open_notification.setGeometry(350, 750, 950, 50 );
    open_notification.setAlignment(Qt::AlignLeft);
    open_notification.setStyleSheet("QLabel { background-color : rgb(235,235,235); color : black; font-size:40px}");

    std::unique_ptr<pqxx::connection> C;

    QObject::connect
    (
        &connect_button,
        &QPushButton::clicked,
        [&] ()
        {
            if (C && C->is_open())
            {
                QString con_name = C->dbname();
                C.reset();
                connect_button.setStyleSheet("QPushButton { background-color : green; color : black; font-size:40px; border-radius: 15px; }");
                open_notification.setText("Закрыто подключение к " + con_name);
                connect_button.setText("Connect");

                return;
            }

            std::string conn_str =
                "dbname=" + dbname_field.text().toStdString() +
                " user=" + user_field.text().toStdString() +
                " password=" + password_field.text().toStdString() +
                " hostaddr=" + hostaddr_field.text().toStdString() +
                " port=" + port_field.text().toStdString();

            C = std::make_unique<pqxx::connection>(conn_str);

            if (C->is_open())
            {
                QString first = "Открыто подключение к ";
                QString second = C->dbname();
                open_notification.setText(first + second);
                connect_button.setStyleSheet("QPushButton { background-color : red; color : black; font-size:40px; border-radius: 15px; }");
                connect_button.setText("Disconnect");
            }
            else
            {
                open_notification.setText("Не могу открыть базу данных");
                open_notification.setStyleSheet("QPushButton { background-color : green; color : white; font-size:50px; border-radius: 15px; }");
            }
        }
    );

    QPushButton print_button("Print", &window);
    print_button.setGeometry(350, 650, 400, 50 );
    print_button.setStyleSheet("QPushButton { background-color : grey; color : black; font-size:40px; border-radius: 15px; }");

    QObject::connect
    (
        &print_button,
        &QPushButton::clicked,
        [&] ()
        {
            if (C && C->is_open()) {
                pqxx::nontransaction N(*C);
                pqxx::result R( N.exec( "SELECT * FROM elements" ));
                QString results;

                for (auto row: R)
                {
                    for (auto field: row)
                    {
                        results += field.c_str();
                        results += "\t";
                    }
                    results += "\n";
                }

                sucess_label.setText(results);
            }
            else {
                sucess_label.setText("Connection\nnot\nestablished");
            }
        }
    );

    window.show();
    return a.exec();
}
