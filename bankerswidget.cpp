#include "bankerswidget.h"
#include <QStringList>
#include <QVector>
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

BankersWidget::BankersWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout *allocationLayout = new QHBoxLayout();
    QHBoxLayout *maxLayout = new QHBoxLayout();

    for (int i = 0; i < 5; ++i) {
        QLineEdit *allocLineEdit = new QLineEdit(this);
        allocLineEdit->setPlaceholderText(QString("Alloc %1").arg(i + 1));
        allocationInputs.append(allocLineEdit);
        allocationLayout->addWidget(allocLineEdit);

        QLineEdit *maxLineEdit = new QLineEdit(this);
        maxLineEdit->setPlaceholderText(QString("Max %1").arg(i + 1));
        maxInputs.append(maxLineEdit);
        maxLayout->addWidget(maxLineEdit);
    }

    mainLayout->addLayout(allocationLayout);
    mainLayout->addLayout(maxLayout);

    availableInput = new QLineEdit(this);
    availableInput->setPlaceholderText("Available resources separated by spaces");
    mainLayout->addWidget(availableInput);

    requestProcessInput = new QLineEdit(this);
    requestProcessInput->setPlaceholderText("Requesting Process");

    requestResourceInput = new QLineEdit(this);
    requestResourceInput->setPlaceholderText("Requested Resources");

    calculateButton = new QPushButton("Calculate", this);
    fileButton = new QPushButton("Load Allocation/Max", this);
    loadRequestButton = new QPushButton("Load Request", this);
    outputBox = new QTextEdit(this);
    outputBox->setReadOnly(true);

    mainLayout->addWidget(requestProcessInput);
    mainLayout->addWidget(requestResourceInput);
    mainLayout->addWidget(calculateButton);
    mainLayout->addWidget(fileButton);
    mainLayout->addWidget(loadRequestButton);
    mainLayout->addWidget(outputBox);

    connect(calculateButton, &QPushButton::clicked, this, &BankersWidget::calculateBankers);
    connect(fileButton, &QPushButton::clicked, this, &BankersWidget::loadFromFile);
    connect(loadRequestButton, &QPushButton::clicked, this, &BankersWidget::loadRequestFromFile);
}

struct ProcessData {
    QVector<int> allocation;
    QVector<int> max;
    QVector<int> need;
};

void findSafeSequences(QVector<ProcessData> &processes, QVector<int> &available, QVector<int> &safeSequence, QVector<QVector<int>> &allSafeSequences, QVector<bool> &finish) {
    int n = processes.size();
    int m = available.size();

    for (int p = 0; p < n; ++p) {
        if (!finish[p]) {
            bool canProceed = true;
            for (int i = 0; i < m; ++i) {
                if (processes[p].need[i] > available[i]) {
                    canProceed = false;
                    break;
                }
            }

            if (canProceed) {
                for (int i = 0; i < m; ++i) {
                    available[i] += processes[p].allocation[i];
                }
                safeSequence.append(p);
                finish[p] = true;

                findSafeSequences(processes, available, safeSequence, allSafeSequences, finish);

                for (int i = 0; i < m; ++i) {
                    available[i] -= processes[p].allocation[i];
                }
                safeSequence.removeLast();
                finish[p] = false;
            }
        }
    }

    if (safeSequence.size() == n) {
        allSafeSequences.append(safeSequence);
    }
}

void BankersWidget::calculateBankers() {
    QVector<ProcessData> processes(5);
    QVector<int> available(3); // Only three resources

    // Parse input data
    for (int i = 0; i < 5; ++i) {
        ProcessData process;
        process.allocation.resize(3);
        process.max.resize(3);
        process.need.resize(3);

        QStringList allocList = allocationInputs[i]->text().split(" ");
        QStringList maxList = maxInputs[i]->text().split(" ");

        if (allocList.size() != 3 || maxList.size() != 3) {
            outputBox->setText("Error: Each allocation and max input should have exactly 3 values.");
            return;
        }

        for (int j = 0; j < 3; ++j) {
            process.allocation[j] = allocList[j].toInt();
            process.max[j] = maxList[j].toInt();
            process.need[j] = process.max[j] - process.allocation[j];
        }

        processes[i] = process;
    }

    QStringList availableList = availableInput->text().split(" ");
    if (availableList.size() != 3) {
        outputBox->setText("Error: Available input should have exactly 3 values.");
        return;
    }
    for (int i = 0; i < 3; ++i) {
        available[i] = availableList[i].toInt();
    }

    QVector<QVector<int>> allSafeSequences;
    QVector<int> safeSequence;
    QVector<bool> finish(5, false);

    findSafeSequences(processes, available, safeSequence, allSafeSequences, finish);

    if (!allSafeSequences.isEmpty()) {
        QString output = "Safe sequences are:\n";
        for (const auto &sequence : allSafeSequences) {
            for (int p : sequence) {
                output += QString("P%1 ").arg(p);
            }
            output += "\n";
        }
        outputBox->setText(output);
    } else {
        outputBox->setText("No safe sequence found.");
    }

    if (requestLoaded) {
        int requestProcess = requestProcessInput->text().toInt();
        QStringList requestResourceList = requestResourceInput->text().split(" ");
        QVector<int> requestResources(3);

        if (requestResourceList.size() != 3) {
            outputBox->setText("Error: Requested resources input should have exactly 3 values.");
            return;
        }

        for (int i = 0; i < 3; ++i) {
            requestResources[i] = requestResourceList[i].toInt();
        }

        // Check if request can be granted
        for (int i = 0; i < 3; ++i) {
            if (requestResources[i] > processes[requestProcess].need[i] || requestResources[i] > available[i]) {
                outputBox->setText("Error: Request cannot be granted.");
                return;
            }
        }

        // Temporarily allocate the requested resources
        for (int i = 0; i < 3; ++i) {
            available[i] -= requestResources[i];
            processes[requestProcess].allocation[i] += requestResources[i];
            processes[requestProcess].need[i] -= requestResources[i];
        }

        QVector<QVector<int>> allSafeSequences;
        QVector<int> safeSequence;
        QVector<bool> finish(5, false);

        findSafeSequences(processes, available, safeSequence, allSafeSequences, finish);

        // Rollback the allocation
        for (int i = 0; i < 3; ++i) {
            available[i] += requestResources[i];
            processes[requestProcess].allocation[i] -= requestResources[i];
            processes[requestProcess].need[i] += requestResources[i];
        }

        if (!allSafeSequences.isEmpty()) {
            QString output = "Safe sequences after request are:\n";
            for (const auto &sequence : allSafeSequences) {
                for (int p : sequence) {
                    output += QString("P%1 ").arg(p);
                }
                output += "\n";
            }
            outputBox->setText(output);
        } else {
            outputBox->setText("No safe sequence found after request.");
        }
    }
}
void BankersWidget::loadFromFile() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Allocation/Max File"), "", tr("Text Files (*.txt)"));
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        outputBox->setText("Error: Cannot open file.");
        return;
    }

    QTextStream in(&file);
    QVector<ProcessData> processes(5);
    QVector<int> available(3); // Only three resources

    for (int i = 0; i < 5; ++i) {
        ProcessData process;
        process.allocation.resize(3);
        process.max.resize(3);
        process.need.resize(3);

        QString allocLine = in.readLine();
        QString maxLine = in.readLine();
        QStringList allocList = allocLine.split(" ");
        QStringList maxList = maxLine.split(" ");

        if (allocList.size() != 3 || maxList.size() != 3) {
            outputBox->setText("Error: Each allocation and max input should have exactly 3 values.");
            return;
        }

        for (int j = 0; j < 3; ++j) {
            process.allocation[j] = allocList[j].toInt();
            process.max[j] = maxList[j].toInt();
            process.need[j] = process.max[j] - process.allocation[j];
        }

        processes[i] = process;
    }

    QString availableLine = in.readLine();
    QStringList availableList = availableLine.split(" ");
    if (availableList.size() != 3) {
        outputBox->setText("Error: Available input should have exactly 3 values.");
        return;
    }
    for (int i = 0; i < 3; ++i) {
        available[i] = availableList[i].toInt();
    }

    outputBox->setText("File loaded successfully.");
    file.close();

    // Fill the inputs with file data
    for (int i = 0; i < 5; ++i) {
        allocationInputs[i]->setText(QString("%1 %2 %3").arg(processes[i].allocation[0]).arg(processes[i].allocation[1]).arg(processes[i].allocation[2]));
        maxInputs[i]->setText(QString("%1 %2 %3").arg(processes[i].max[0]).arg(processes[i].max[1]).arg(processes[i].max[2]));
    }
    availableInput->setText(availableLine);
}

void BankersWidget::loadRequestFromFile() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Request File"), "", tr("Text Files (*.txt)"));
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        outputBox->setText("Error: Cannot open file.");
        return;
    }

    QTextStream in(&file);

    QString requestProcessLine = in.readLine();
    QString requestResourcesLine = in.readLine();

    requestProcessInput->setText(requestProcessLine);
    requestResourceInput->setText(requestResourcesLine);

    outputBox->setText("Request file loaded successfully.");
    file.close();

    requestLoaded = true;
}