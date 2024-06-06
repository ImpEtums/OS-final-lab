#ifndef RRWIDGET_H
#define RRWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

class RRWidget : public QWidget
{
    Q_OBJECT

public:
    RRWidget(QWidget *parent = nullptr);

private slots:
    void calculate();
    void loadFromFile();

private:
    QLineEdit *timeSliceInput;
    QLineEdit *serviceTimeInput;
    QLineEdit *arrivalTimeInput;
    QPushButton *calculateButton;
    QPushButton *fileButton;
    QTextEdit *outputDisplay;
};

#endif // RRWIDGET_H
