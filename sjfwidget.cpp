#include "sjfwidget.h"
#include <QStringList>
#include <QVector>
#include <QPair>
#include <algorithm>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

SJFWidget::SJFWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    serviceTimeInput = new QLineEdit(this);
    serviceTimeInput->setPlaceholderText("Enter service times separated by spaces");

    arrivalTimeInput = new QLineEdit(this);
    arrivalTimeInput->setPlaceholderText("Enter arrival times separated by spaces");

    calculateButton = new QPushButton("Calculate", this);
    fileButton = new QPushButton("File", this); // Add file button
    outputBox = new QTextEdit(this);
    outputBox->setReadOnly(true);

    mainLayout->addWidget(serviceTimeInput);
    mainLayout->addWidget(arrivalTimeInput); // Add arrival time input to layout
    mainLayout->addWidget(calculateButton);
    mainLayout->addWidget(fileButton); // Add file button to layout
    mainLayout->addWidget(outputBox);

    connect(calculateButton, &QPushButton::clicked, this, &SJFWidget::calculateSJF);
    connect(fileButton, &QPushButton::clicked, this, &SJFWidget::loadFromFile); // Connect file button
}

void SJFWidget::calculateSJF() {
    QString serviceInputText = serviceTimeInput->text();
    QString arrivalInputText = arrivalTimeInput->text();

    if (serviceInputText.isEmpty() || arrivalInputText.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Service times or arrival times cannot be empty.");
        return;
    }

    QStringList serviceTimeList = serviceInputText.split(" ");
    QStringList arrivalTimeList = arrivalInputText.split(" ");

    if (serviceTimeList.size() != arrivalTimeList.size()) {
        QMessageBox::warning(this, "Input Error", "The number of service times and arrival times must be equal.");
        return;
    }

    QVector<QPair<int, QString>> times;
    QVector<int> arrivalTimes;

    for (int i = 0; i < serviceTimeList.size(); ++i) {
        times.append(qMakePair(serviceTimeList[i].toInt(), QString("P%1").arg(i + 1)));
        arrivalTimes.append(arrivalTimeList[i].toInt());
    }

    QVector<bool> isCompleted(serviceTimeList.size(), false);
    QString output;
    int currentTime = 0;
    int totalWaitTime = 0;
    int completedProcesses = 0;

    while (completedProcesses < serviceTimeList.size()) {
        int shortestIndex = -1;
        int minServiceTime = INT_MAX;

        for (int i = 0; i < times.size(); ++i) {
            if (arrivalTimes[i] <= currentTime && !isCompleted[i] && times[i].first < minServiceTime) {
                minServiceTime = times[i].first;
                shortestIndex = i;
            }
        }

        if (shortestIndex == -1) {
            currentTime++;
            continue;
        }

        int startTime = currentTime;
        currentTime += times[shortestIndex].first;
        int endTime = currentTime;
        totalWaitTime += startTime - arrivalTimes[shortestIndex];
        output += QString("%1: %2 - %3\n").arg(times[shortestIndex].second).arg(startTime).arg(endTime);
        isCompleted[shortestIndex] = true;
        completedProcesses++;
    }

    double avgWaitTime = static_cast<double>(totalWaitTime) / times.size();
    output += QString("Average Waiting Time: %1\n").arg(avgWaitTime);

    outputBox->setText(output);
}

void SJFWidget::loadFromFile() {
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
