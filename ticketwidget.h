#ifndef TICKETWIDGET_H
#define TICKETWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QThread>

class TicketWidget : public QWidget {
    Q_OBJECT

public:
    explicit TicketWidget(QWidget *parent = nullptr);

private slots:
    void startSales();

private:
    QTextEdit *outputBox;
    QPushButton *startButton;
};

class TicketSales : public QThread {
    Q_OBJECT

public:
    explicit TicketSales(QTextEdit *outputBox, QObject *parent = nullptr);
    void run() override;

signals:
    void updateOutput(const QString &text);

private:
    int tickets;
    QTextEdit *outputBox;
};

#endif // TICKETWIDGET_H
