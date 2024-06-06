#ifndef BANKERSWIDGET_H
#define BANKERSWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>

class BankersWidget : public QWidget {
    Q_OBJECT

public:
    BankersWidget(QWidget *parent = nullptr);

private slots:
    void calculateBankers();
    void loadFromFile();
    void loadRequestFromFile();

private:
    QVector<QLineEdit*> allocationInputs;
    QVector<QLineEdit*> maxInputs;
    QLineEdit *availableInput;
    QLineEdit *requestProcessInput;
    QLineEdit *requestResourceInput;
    QPushButton *calculateButton;
    QPushButton *fileButton;
    QPushButton *loadRequestButton;
    QTextEdit *outputBox;
    bool requestLoaded = false;
};

#endif // BANKERSWIDGET_H
