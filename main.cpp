#include <QApplication>
#include <QTabWidget>
#include "sjfwidget.h"
#include "srtfwidget.h"
#include "rrwidget.h"
#include "bankerswidget.h"
//#include "ticketwidget.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QTabWidget tabWidget;

    // 添加各个算法的页面
    tabWidget.addTab(new SJFWidget(), "SJF Algorithm");
    tabWidget.addTab(new SRTFWidget(), "SRTF Algorithm");
    tabWidget.addTab(new RRWidget(), "RR Algorithm");
    tabWidget.addTab(new BankersWidget(), "Banker's Algorithm");
    //tabWidget.addTab(new TicketWidget(), "Ticket Sales");

    tabWidget.setWindowTitle("Algorithm Integration");
    tabWidget.resize(600, 400);
    tabWidget.show();
    
    return a.exec();
}
