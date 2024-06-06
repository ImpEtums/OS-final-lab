#include "rrwidget.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <algorithm>

RRWidget::RRWidget(QWidget *parent)
    : QWidget(parent),
      timeSliceInput(new QLineEdit(this)),
      serviceTimeInput(new QLineEdit(this)),
      arrivalTimeInput(new QLineEdit(this)),
      calculateButton(new QPushButton("Calculate", this)),
      fileButton(new QPushButton("File", this)),
      outputDisplay(new QTextEdit(this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    timeSliceInput->setPlaceholderText("Time Slice Length");
    serviceTimeInput->setPlaceholderText("Service Time");
    arrivalTimeInput->setPlaceholderText("Arrival Time");

    layout->addWidget(timeSliceInput);
    layout->addWidget(serviceTimeInput);
    layout->addWidget(arrivalTimeInput);
    layout->addWidget(calculateButton);
    layout->addWidget(fileButton);
    layout->addWidget(outputDisplay);

    setLayout(layout);

    connect(calculateButton, &QPushButton::clicked, this, &RRWidget::calculate);
    connect(fileButton, &QPushButton::clicked, this, &RRWidget::loadFromFile);
}

void RRWidget::calculate()
{
    int timeSlice = timeSliceInput->text().toInt();
    QStringList serviceTimesStr = serviceTimeInput->text().split(' ');
    QStringList arrivalTimesStr = arrivalTimeInput->text().split(' ');

    if (serviceTimesStr.size() != arrivalTimesStr.size()) {
        QMessageBox::warning(this, "Input Error", "Service times and arrival times must have the same number of elements.");
        return;
    }

    int n = serviceTimesStr.size();
    QVector<int> serviceTimes(n);
    QVector<int> arrivalTimes(n);

    for (int i = 0; i < n; ++i) {
        serviceTimes[i] = serviceTimesStr[i].toInt();
        arrivalTimes[i] = arrivalTimesStr[i].toInt();
    }

    QVector<int> remainingTimes = serviceTimes;
    QVector<int> completionTimes(n, 0);
    QVector<int> waitingTimes(n, 0);
    QVector<int> turnAroundTimes(n, 0);

    int time = 0;
    QVector<int> queue;

    auto compare = [&](int a, int b) {
        if (remainingTimes[a] == remainingTimes[b])
            return arrivalTimes[a] < arrivalTimes[b];
        return remainingTimes[a] < remainingTimes[b];
    };

    QVector<QString> schedule;

    while (true) {
        // Add all processes that have arrived to the queue
        for (int i = 0; i < n; ++i) {
            if (arrivalTimes[i] <= time && remainingTimes[i] > 0 && !queue.contains(i)) {
                queue.push_back(i);
            }
        }

        if (queue.empty()) {
            // If no processes are in the queue and no remaining processes exist, break
            if (std::all_of(remainingTimes.begin(), remainingTimes.end(), [](int t) { return t == 0; }))
                break;
            time++;
            continue;
        }

        // Sort the queue based on remaining time and arrival time
        std::sort(queue.begin(), queue.end(), compare);

        int currentProcess = queue.front();
        queue.pop_front();

        int startTime = time;
        if (remainingTimes[currentProcess] > timeSlice) {
            time += timeSlice;
            remainingTimes[currentProcess] -= timeSlice;
        } else {
            time += remainingTimes[currentProcess];
            remainingTimes[currentProcess] = 0;
        }

        schedule.append(QString("Time %1 - Time %2 : P%3")
                        .arg(startTime)
                        .arg(time)
                        .arg(currentProcess + 1));

        if (remainingTimes[currentProcess] == 0) {
            completionTimes[currentProcess] = time;
            waitingTimes[currentProcess] = time - serviceTimes[currentProcess] - arrivalTimes[currentProcess];
        }
    }

    for (int i = 0; i < n; ++i) {
        turnAroundTimes[i] = serviceTimes[i] + waitingTimes[i];
    }

    QString output;
    for (const QString &entry : schedule) {
        output += entry + "\n";
    }

    double totalWaitingTime = std::accumulate(waitingTimes.begin(), waitingTimes.end(), 0.0);
    double averageWaitingTime = totalWaitingTime / n;
    output += QString("\nAverage Waiting Time: %1").arg(averageWaitingTime);

    outputDisplay->setText(output);
}

void RRWidget::loadFromFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "Text Files (*.txt)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "File Error", "Unable to open the selected file.");
        return;
    }

    QTextStream in(&file);
    if (!in.atEnd()) {
        timeSliceInput->setText(in.readLine().trimmed());
    }
    if (!in.atEnd()) {
        serviceTimeInput->setText(in.readLine().trimmed());
    }
    if (!in.atEnd()) {
        arrivalTimeInput->setText(in.readLine().trimmed());
    }
}
