#ifndef SJFWIDGET_H
#define SJFWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>

class SJFWidget : public QWidget {
    Q_OBJECT

public:
    explicit SJFWidget(QWidget *parent = nullptr);

private slots:
    void calculateSJF();
    void loadFromFile();

private:
    QLineEdit *serviceTimeInput;
    QLineEdit *arrivalTimeInput;
    QTextEdit *outputBox;
    QPushButton *calculateButton;
    QPushButton *fileButton;
};

#endif // SJFWIDGET_H
