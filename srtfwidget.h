#ifndef SRTFWIDGET_H
#define SRTFWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

class SRTFWidget : public QWidget {
    Q_OBJECT

public:
    explicit SRTFWidget(QWidget *parent = nullptr);

private slots:
    void calculateSRTF();
    void loadFromFile();

private:
    QLineEdit *arrivalTimeInput;
    QLineEdit *serviceTimeInput;
    QTextEdit *outputBox;
    QPushButton *calculateButton;
    QPushButton *fileButton;
};

#endif // SRTFWIDGET_H
