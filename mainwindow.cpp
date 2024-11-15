#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupTable();
    setupButtons();
    updateTable();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupTable() {
    table = new QTableWidget(this);
    table->setColumnCount(2); // Первый столбец для студентов, второй и далее для предметов
    table->setRowCount(5);    // Примерное количество студентов
    table->setHorizontalHeaderLabels({"Студент", "Предмет 1"});
    table->setVerticalHeaderLabels({"", "", "", "", ""});
    setCentralWidget(table);

    // Создаем таймер для обработки click и double click
    clickTimer = new QTimer(this);
    clickTimer->setSingleShot(true); // Таймер срабатывает один раз

    connect(table, &QTableWidget::cellClicked, this, [this](int row, int column) {
        clickTimer->start(200); // Устанавливаем задержку (в миллисекундах)
        // Сохраняем данные о клике
        lastClickedRow = row;
        lastClickedColumn = column;
    });

    connect(clickTimer, &QTimer::timeout, this, [this]() {
        // Обработка одиночного клика
        editCell(lastClickedRow, lastClickedColumn);
    });

    connect(table, &QTableWidget::cellDoubleClicked, this, [this](int row, int column) {
        clickTimer->stop(); // Останавливаем таймер
        showHistory(row, column); // Обработка двойного клика
    });

    // Соединяем сигнал клика по заголовку столбца с методом сортировки
    connect(table->horizontalHeader(), &QHeaderView::sectionClicked, this, &MainWindow::sortColumn);
}

void MainWindow::setupButtons() {
    QVBoxLayout *layout = new QVBoxLayout;

    QPushButton *addStudentBtn = new QPushButton("Добавить студента");
    connect(addStudentBtn, &QPushButton::clicked, this, &MainWindow::addStudent);
    layout->addWidget(addStudentBtn);

    QPushButton *removeStudentBtn = new QPushButton("Удалить студента");
    connect(removeStudentBtn, &QPushButton::clicked, this, &MainWindow::removeStudent);
    layout->addWidget(removeStudentBtn);

    QPushButton *renameStudentBtn = new QPushButton("Переименовать студента");
    connect(renameStudentBtn, &QPushButton::clicked, this, &MainWindow::renameStudent);
    layout->addWidget(renameStudentBtn);

    QPushButton *addSubjectBtn = new QPushButton("Добавить предмет");
    connect(addSubjectBtn, &QPushButton::clicked, this, &MainWindow::addSubject);
    layout->addWidget(addSubjectBtn);

    QPushButton *removeSubjectBtn = new QPushButton("Удалить предмет");
    connect(removeSubjectBtn, &QPushButton::clicked, this, &MainWindow::removeSubject);
    layout->addWidget(removeSubjectBtn);

    QPushButton *renameSubjectBtn = new QPushButton("Переименовать предмет");
    connect(renameSubjectBtn, &QPushButton::clicked, this, &MainWindow::renameSubject);
    layout->addWidget(renameSubjectBtn);

    QPushButton *saveBtn = new QPushButton("Сохранить в JSON");
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::saveToJson);
    layout->addWidget(saveBtn);

    QPushButton *loadBtn = new QPushButton("Загрузить из JSON");
    connect(loadBtn, &QPushButton::clicked, this, &MainWindow::loadFromJson);
    layout->addWidget(loadBtn);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setMenuWidget(widget);
}

void MainWindow::updateTable() {
    table->clearContents();
    table->setColumnCount(subjects.size() + 1);
    table->setHorizontalHeaderLabels(QStringList({"Студент"}) + subjects);
    table->setRowCount(students.size());

    for (int row = 0; row < students.size(); ++row) {
        QWidget *widget = new QWidget;
        QHBoxLayout *layout = new QHBoxLayout(widget);
        QCheckBox *checkBox = new QCheckBox;
        checkBox->setChecked(true);
        connect(checkBox, &QCheckBox::stateChanged, this, [this, row](int state) { toggleStudentActivity(state); });
        layout->addWidget(checkBox);
        QLineEdit *lineEdit = new QLineEdit(students[row]);
        layout->addWidget(lineEdit);
        layout->setContentsMargins(0, 0, 0, 0);
        widget->setLayout(layout);
        table->setCellWidget(row, 0, widget);

        for (int col = 1; col < subjects.size() + 1; ++col) {
            updateCell(row, col);
        }
    }
}

void MainWindow::updateCell(int row, int column) {
    QString student = students[row];
    QString subject = subjects[column - 1];
    QPair<QDate, int> grade = grades.value({student, subject}, {QDate(), 0});
    QTableWidgetItem *item = new QTableWidgetItem(QString::number(grade.second));
    switch (grade.second) {
    case 5: item->setBackground(Qt::green); break;
    case 4: item->setBackground(Qt::yellow); break;
    case 3: item->setBackground(Qt::blue); break;
    case 2: item->setBackground(Qt::red); break;
    default: item->setBackground(Qt::white); break;
    }
    table->setItem(row, column, item);
}

void MainWindow::addStudent() {
    bool ok;
    QString name = QInputDialog::getText(this, "Добавить студента", "Введите имя студента:", QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        students.append(name);
        table->setRowCount(students.size());
        updateTable();
    }
}

void MainWindow::removeStudent() {
    if (students.isEmpty()) return;

    bool ok;
    QStringList studentNames;
    for (const QString &student : students) {
        studentNames.append(student);
    }

    QString selectedStudent = QInputDialog::getItem(this, "Выберите студента", "Студент:", studentNames, 0, false, &ok);
    if (ok && !selectedStudent.isEmpty()) {
        int index = students.indexOf(selectedStudent);
        students.removeAt(index);
        table->setRowCount(students.size());
        updateTable();
    }
}

void MainWindow::renameStudent() {
    if (students.isEmpty()) return;

    bool ok;
    QStringList studentNames;
    for (const QString &student : students) {
        studentNames.append(student);
    }

    QString selectedStudent = QInputDialog::getItem(this, "Выберите студента", "Студент:", studentNames, 0, false, &ok);
    if (ok && !selectedStudent.isEmpty()) {
        QString newName = QInputDialog::getText(this, "Переименовать студента", "Введите новое имя студента:", QLineEdit::Normal, selectedStudent, &ok);
        if (ok && !newName.isEmpty()) {
            int index = students.indexOf(selectedStudent);
            students[index] = newName;
            updateTable();
        }
    }
}

void MainWindow::addSubject() {
    bool ok;
    QString name = QInputDialog::getText(this, "Добавить предмет", "Введите название предмета:", QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        subjects.append(name);
        table->setColumnCount(subjects.size() + 1);
        table->setHorizontalHeaderLabels(QStringList({"Студент"}) + subjects);
        updateTable();
    }
}

void MainWindow::removeSubject() {
    if (subjects.isEmpty()) return;

    bool ok;
    QStringList subjectNames;
    for (const QString &subject : subjects) {
        subjectNames.append(subject);
    }

    QString selectedSubject = QInputDialog::getItem(this, "Выберите предмет", "Предмет:", subjectNames, 0, false, &ok);
    if (ok && !selectedSubject.isEmpty()) {
        int index = subjects.indexOf(selectedSubject);
        subjects.removeAt(index);
        table->setColumnCount(subjects.size() + 1);
        table->setHorizontalHeaderLabels(QStringList({"Студент"}) + subjects);
        updateTable();
    }
}

void MainWindow::renameSubject() {
    if (subjects.isEmpty()) return;

    bool ok;
    QStringList subjectNames;
    for (const QString &subject : subjects) {
        subjectNames.append(subject);
    }

    QString selectedSubject = QInputDialog::getItem(this, "Выберите предмет", "Предмет:", subjectNames, 0, false, &ok);
    if (ok && !selectedSubject.isEmpty()) {
        QString newName = QInputDialog::getText(this, "Переименовать предмет", "Введите новое название предмета:", QLineEdit::Normal, selectedSubject, &ok);
        if (ok && !newName.isEmpty()) {
            int index = subjects.indexOf(selectedSubject);
            subjects[index] = newName;
            table->setHorizontalHeaderLabels(QStringList({"Студент"}) + subjects);
            updateTable();
        }
    }
}

void MainWindow::editCell(int row, int column) {
    if (column == 0) return; // Не редактируем первый столбец

    QString student = students[row];
    QString subject = subjects[column - 1];
    QTableWidgetItem *item = table->item(row, column);
    bool ok;
    int grade = 0; // Значение по умолчанию

    if (item && !item->text().isEmpty()) {
        // Используем текущую оценку из ячейки, если она есть
        grade = item->text().toInt();
    }

    // Открываем диалоговое окно для ввода/изменения оценки
    int newGrade = QInputDialog::getInt(
        this,
        "Редактирование оценки",
        QString("Введите оценку для %1 по предмету %2:")
            .arg(student)
            .arg(subject),
        grade,  // Текущее значение оценки
        2,      // Минимальная оценка
        5,      // Максимальная оценка
        1,      // Шаг
        &ok
        );

    if (ok) {
        // Сохраняем оценку
        grades[{student, subject}] = {QDate::currentDate(), newGrade};
        history[student].append({QDate::currentDate(), newGrade});
        updateCell(row, column); // Обновляем содержимое ячейки
    }
}

void MainWindow::sortColumn(int column) {
    if (column == 0) return; // Не сортируем первый столбец

    std::sort(students.begin(), students.end(), [this, column](const QString &a, const QString &b) {
        int gradeA = grades.value({a, subjects[column - 1]}, {QDate(), 0}).second;
        int gradeB = grades.value({b, subjects[column - 1]}, {QDate(), 0}).second;
        return sortAscending ? gradeA < gradeB : gradeA > gradeB;
    });
    sortAscending = !sortAscending;
    updateTable();
}

void MainWindow::toggleStudentActivity(int state) {
    QCheckBox *checkBox = qobject_cast<QCheckBox*>(sender());
    if (checkBox) {
        QWidget *widget = checkBox->parentWidget();
        int row = table->indexAt(widget->pos()).row();
        for (int col = 1; col < subjects.size() + 1; ++col) {
            QTableWidgetItem *item = table->item(row, col);
            if (item) {
                item->setBackground(state == Qt::Checked ? Qt::white : Qt::gray);
                item->setFlags(state == Qt::Checked ? item->flags() | Qt::ItemIsEnabled : item->flags() & ~Qt::ItemIsEnabled);
            }
        }
    }
}

void MainWindow::showHistory(int row, int column) {
    if (column == 0) return; // Не показываем историю для первого столбца

    QString student = students[row];
    QString subject = subjects[column - 1];
    QList<QPair<QDate, int>> studentHistory = history.value(student);
    QStringList historyList;
    for (const auto &entry : studentHistory) {
        historyList.append(entry.first.toString("dd.MM.yyyy") + ": " + QString::number(entry.second));
    }
    QMessageBox::information(this, "История оценок", historyList.join("\n"));
}

void MainWindow::saveToJson() {
    QJsonObject json;
    QJsonArray studentsArray;
    for (const QString &student : students) {
        QJsonObject studentObj;
        studentObj["name"] = student;
        QJsonArray gradesArray;
        for (const QString &subject : subjects) {
            QJsonObject gradeObj;
            gradeObj["subject"] = subject;
            gradeObj["date"] = grades[{student, subject}].first.toString("dd.MM.yyyy");
            gradeObj["grade"] = grades[{student, subject}].second;
            gradesArray.append(gradeObj);
        }
        studentObj["grades"] = gradesArray;
        studentsArray.append(studentObj);
    }
    json["students"] = studentsArray;

    QJsonDocument doc(json);
    QFile file("grades.json");
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл для записи.");
        return;
    }
    file.write(doc.toJson());
    file.close();
}

void MainWindow::loadFromJson() {
    QFile file("grades.json");
    if (!file.exists()) {
        QMessageBox::information(this, "Информация", "Файл не существует. Создан новый файл.");
        file.open(QIODevice::WriteOnly);
        file.close();
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл для чтения.");
        return;
    }

    QByteArray data = file.readAll();
    file.close();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject json = doc.object();
    QJsonArray studentsArray = json["students"].toArray();
    students.clear();
    subjects.clear();
    grades.clear();
    history.clear();
    for (const QJsonValue &studentValue : studentsArray) {
        QJsonObject studentObj = studentValue.toObject();
        QString student = studentObj["name"].toString();
        students.append(student);
        QJsonArray gradesArray = studentObj["grades"].toArray();
        for (const QJsonValue &gradeValue : gradesArray) {
            QJsonObject gradeObj = gradeValue.toObject();
            QString subject = gradeObj["subject"].toString();
            if (!subjects.contains(subject)) {
                subjects.append(subject);
            }
            QDate date = QDate::fromString(gradeObj["date"].toString(), "dd.MM.yyyy");
            int grade = gradeObj["grade"].toInt();
            grades[{student, subject}] = {date, grade};
            history[student].append({date, grade});
        }
    }
    updateTable();
}
