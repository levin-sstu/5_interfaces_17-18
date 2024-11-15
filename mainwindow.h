#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QHeaderView>
#include <QHash>
#include <QPair>
#include <QDate>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QMessageBox>
#include <QInputDialog>
#include <QColor>
#include <QTimer>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addStudent();
    void removeStudent();
    void renameStudent();
    void addSubject();
    void removeSubject();
    void renameSubject();
    void editCell(int row, int column);
    void sortColumn(int column);
    void toggleStudentActivity(int state);
    void saveToJson();
    void loadFromJson();

private:
    QTableWidget *table;
    QHash<QPair<QString, QString>, QPair<QDate, int>> grades;
    QHash<QString, QList<QPair<QDate, int>>> history;
    QList<QString> subjects;
    QList<QString> students;
    bool sortAscending = true;
    QTimer *clickTimer;
    int lastClickedRow = -1;
    int lastClickedColumn = -1;

    void setupTable();
    void setupButtons();
    void updateTable();
    void updateCell(int row, int column);
    void showHistory(int row, int column);
};

#endif // MAINWINDOW_H
