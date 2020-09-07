#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QClipboard>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    QTextStream standardOutput(stdout);
    for (QSerialPortInfo portInfo : ports) {
        QString portName = portInfo.portName();
        if (portInfo.manufacturer().contains("Arduino")) {
            qDebug() << "Found arduino";
            QSerialPort serialPort;
            QString serialPortName = portInfo.portName();
            serialPort.setPortName(serialPortName);
            const int serialPortBaudRate = QSerialPort::Baud9600;
            serialPort.setBaudRate(serialPortBaudRate);
            if (!serialPort.open(QIODevice::WriteOnly)) {

                standardOutput << QObject::tr("Failed to open port %1, error: %2")
                                  .arg(serialPortName).arg(serialPort.errorString())
                               << "\n";
                return;
            }

            qDebug() << "opened it, sending some data";


            QByteArray writeData("");
            writeData.append("");


            writeData.append(QApplication::clipboard()->text());

            const qint64 bytesWritten = serialPort.write(writeData);

            if (bytesWritten == -1) {
                standardOutput << QObject::tr("Failed to write the data to port %1, error: %2")
                                  .arg(serialPortName).arg(serialPort.errorString())
                               << "\n";
                return;
            } else if (bytesWritten != writeData.size()) {
                standardOutput << QObject::tr("Failed to write all the data to port %1, error: %2")
                                  .arg(serialPortName).arg(serialPort.errorString())
                               << "\n";
                return;
            } else if (!serialPort.waitForBytesWritten(5000)) {
                standardOutput << QObject::tr("Operation timed out or an error "
                                              "occurred for port %1, error: %2")
                                  .arg(serialPortName).arg(serialPort.errorString())
                               << "\n";
                return;
            }

            standardOutput << QObject::tr("Data successfully sent to port %1")
                              .arg(serialPortName)
                           << "\n";
            serialPort.close();

            // on mac it sees two which are the same real device
            // and therefore loads the eeprom w/ double the data.
            // as a fix, make sure to break the loop here.
            break;
        }
    }
}
