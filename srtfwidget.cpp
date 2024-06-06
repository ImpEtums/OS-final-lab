#include "srtfwidget.h"
#include <QStringList>
#include <QVector>
#include <QPair>
#include <algorithm>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

SRTFWidget::SRTFWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    arrivalTimeInput = new QLineEdit(this);
    arrivalTimeInput->setPlaceholderText("Enter arrival times separated by spaces");

    serviceTimeInput = new QLineEdit(this);
    serviceTimeInput->setPlaceholderText("Enter service times separated by spaces");

    calculateButton = new QPushButton("Calculate", this);
    fileButton = new QPushButton("File", this);
    outputBox = new QTextEdit(this);
    outputBox->setReadOnly(true);

    mainLayout->addWidget(arrivalTimeInput);
    mainLayout->addWidget(serviceTimeInput);
    mainLayout->addWidget(calculateButton);
    mainLayout->addWidget(fileButton);
    mainLayout->addWidget(outputBox);

    connect(calculateButton, &QPushButton::clicked, this, &SRTFWidget::calculateSRTF);
    connect(fileButton, &QPushButton::clicked, this, &SRTFWidget::loadFromFile);
}

struct Process {
    int arrivalTime;
    int serviceTime;
    int remainingTime;
    QString name;
};

void SRTFWidget::calculateSRTF() {
    QString arrivalText = arrivalTimeInput->text();
    QString serviceText = serviceTimeInput->text();

    if (arrivalText.isEmpty() || serviceText.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Arrival times or service times cannot be empty.");
        return;
    }

    QStringList arrivalList = arrivalText.split(" ");
    QStringList serviceList = serviceText.split(" ");

    if (arrivalList.size() != serviceList.size()) {
        QMessageBox::warning(this, "Input Error", "The number of arrival times and service times must be equal.");
        return;
    }

    QVector<Process> processes;

    for (int i = 0; i < arrivalList.size(); ++i) {
        Process p;
        p.arrivalTime = arrivalList[i].toInt();
        p.serviceTime = serviceList[i].toInt();
        p.remainingTime = p.serviceTime;
        p.name = QString("P%1").arg(i + 1);
        processes.append(p);
    }

    QString output;
    int currentTime = 0;
    int completedProcesses = 0;
    int totalWaitTime = 0;

    while (completedProcesses < processes.size()) {
        int minRemainingTime = INT_MAX;
        int shortestProcessIndex = -1;

        for (int i = 0; i < processes.size(); ++i) {
            if (processes[i].arrivalTime <= currentTime && processes[i].remainingTime > 0 &&
                processes[i].remainingTime < minRemainingTime) {
                minRemainingTime = processes[i].remainingTime;
                shortestProcessIndex = i;
            }
        }

        if (shortestProcessIndex == -1) {
            currentTime++;
            continue;
        }

        processes[shortestProcessIndex].remainingTime--;
        output += QString("Time %1: %2\n").arg(currentTime).arg(processes[shortestProcessIndex].name);
        currentTime++;

        if (processes[shortestProcessIndex].remainingTime == 0) {
            completedProcesses++;
            int waitTime = currentTime - processes[shortestProcessIndex].arrivalTime - processes[shortestProcessIndex].serviceTime;
            totalWaitTime += waitTime;
        }
    }

    double avgWaitTime = static_cast<double>(totalWaitTime) / processes.size();
    output += QString("Average Waiting Time: %1\n").arg(avgWaitTime);

    outputBox->setText(output);
}

void SRTFWidget::loadFromFile() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt)"));
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error", "Cannot open file.");
        return;
    }

    QTextStream in(&file);
    QString serviceTimesLine = in.readLine();
    QString arrivalTimesLine = in.readLine();

    if (serviceTimesLine.isEmpty() || arrivalTimesLine.isEmpty()) {
        QMessageBox::warning(this, "File Error", "File contents are incomplete or empty.");
        return;
    }

    serviceTimeInput->setText(serviceTimesLine);
    arrivalTimeInput->setText(arrivalTimesLine);

    outputBox->setText("File loaded successfully.");
    file.close();
}
