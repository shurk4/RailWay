#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    readSettings();

    // Проверка IP в LineEdit
    ipRegex.setPattern("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-"
                       "5]|[01]?\\d\\d?)");
    QRegularExpressionValidator *ipValidator =
        new QRegularExpressionValidator(ipRegex, this);
    ui->lineEditIp->setValidator(ipValidator);

    ui->textBrowser->hide();

    setStyle();

    QString ip = ui->lineEditIp->text();
    if (checkIp(ip)) {
        connectToHost();
    }

    startPingTimer();
}

void MainWindow::startPingTimer()
{
    // Таймер
    pingTimer = new QTimer(this);
    connect(pingTimer, &QTimer::timeout, this, &MainWindow::connectionTimerSlot);
    pingTimer->start(pingTimerMs);
}

void MainWindow::connectionTimerSlot() {
    message("Ping timer");
    if (!isConnected) {
        connectToHost();
    }
    else
    {
        sendCommand("&8");
    }
}

MainWindow::~MainWindow() {
    disconnectFromHost();
    delete pingTimer;
    delete ui;
}

void MainWindow::readSettings() {
    QSettings settings("ShurkSoft", "RailWayRC");
    settings.beginGroup("connectionSettings");
    ui->lineEditIp->setText(settings.value("ip").toString());
    ui->lineEditPort->setText(settings.value("port").toString());
    settings.endGroup();
}

void MainWindow::writeSettings() {
    QString ip = ui->lineEditIp->text();
    int port = ui->lineEditPort->text().toInt();
    if (checkIp(ip) && checkPort(port)) {
        QSettings settings("ShurkSoft", "RailWayRC");
        settings.beginGroup("connectionSettings");
        settings.setValue("ip", ip);
        settings.setValue("port", ui->lineEditPort->text());
        settings.endGroup();
    }
}

bool MainWindow::checkIp(QString &ip) {
    QRegularExpressionMatch match = ipRegex.match(ip);
    if (match.hasMatch()) {
        return true;
    }
    qDebug() << "Not correct IP";
    return false;
}

bool MainWindow::checkPort(int &port) {
    if (port > 0 && port < 65536) {
        return true;
    }
    return false;
}

void MainWindow::socketReady() {
    isConnected = true;
    QString recivedData = socket->readAll();
    // qDebug() << "Data recived: " << recivedData;
    // ui->textBrowser->append("Data recived: " + recivedData);

    char command;
    QString worldStatus = "";
    QString speed = "";

    for (int i = 0; i < recivedData.size(); i++) {
        if (recivedData[i] == '$') {
            command = '$';
            continue;
        } else if (recivedData[i] == '&') {
            command = '&';
            continue;
        }

        if (command == '$') {
            worldStatus += recivedData[i];
        } else if (command == '&' && recivedData[i] >= '0' &&
                   recivedData[i] <= '9') {
            speed += recivedData[i];
        }
    }

    if (recivedData[0] == '$') {
        // ui->textBrowser->append("World status string = " + worldStatus);
        // qDebug() << "World status string = " << worldStatus;
        // ui->textBrowser->append("Speed string = " + speed);
        // qDebug() << "Speed string = " << speed;
        showWorldStatus(worldStatus);
        ui->lcdNumber->display(speed.toInt());
    }
}

void MainWindow::showWorldStatus(QString &data) {

    int states = data.toInt();

    ui->textBrowser->append("Status int = " + QString::number(states));

    ui->textBrowser->append("Switches:");

    if (states & TRAIN_MOVING_LEFT) {
        message("Train moving left");
        ui->pushButtonLeft->setChecked(true);
        ui->pushButtonRight->setChecked(false);
        ui->pushButtonCenter->setChecked(false);
    }

    if (states & TRAIN_MOVING_RIGHT) {
        message("Train moving right");
        ui->pushButtonLeft->setChecked(false);
        ui->pushButtonRight->setChecked(true);
        ui->pushButtonCenter->setChecked(false);
    }

    if (states & TRAIN_STOPPED) {
        message("Train stopped");
        ui->pushButtonLeft->setChecked(false);
        ui->pushButtonRight->setChecked(false);
        ui->pushButtonCenter->setChecked(true);
    }

    if (states & BARRIER_OPEN && states & DEPO_ARROW) {
        setWorldRight(":/source/world_depo_open.png");
        message("BARRIER_OPEN & DEPO_ARROW");
        ui->pushButtonDepoArrow->setChecked(true);
        ui->pushButtonBarrier->setChecked(true);
    }
    else if(states & BARRIER_OPEN)
    {
        ui->pushButtonDepoArrow->setChecked(false);
        ui->pushButtonBarrier->setChecked(true);
        setWorldRight(":/source/world_big_round_open.png");
        message("BARRIER_OPEN");
    }
    else if(states & DEPO_ARROW){
        ui->pushButtonDepoArrow->setChecked(true);
        ui->pushButtonBarrier->setChecked(false);
        setWorldRight(":/source/world_depo_close.png");
        message("DEPO_ARROW");
    }
    else{
        ui->pushButtonDepoArrow->setChecked(false);
        ui->pushButtonBarrier->setChecked(false);
        setWorldRight(":/source/world_big_round_close.png");
        message("BARRIER_OPEN & DEPO_CLOSED");
    }

    if (states & BIG_ROUND) {
        ui->pushButtonRoundArrow->setChecked(false);
        setWorldLeft(":/source/world_big_round.png");
        message("BIG_ROUND");

    } else {
        ui->pushButtonRoundArrow->setChecked(true);
        setWorldLeft(":/source/world_small_round.png");
        message("SMALL_ROUND");
    }

    if (states & STREET_LIGHT) {
        message("STREET_LIGHT - ON");
        ui->pushButtonOutLightStreet->setChecked(true);
    } else {
        message("STREET_LIGHT - OFF");
        ui->pushButtonOutLightStreet->setChecked(false);
    }

    if (states & HOUSE1_LIGHT) {
        message("HOUSE1_LIGHT_ON");
        ui->pushButtonOutLightHouses->setChecked(true);
    } else {
        message("HOUSE1_LIGHT_OFF");
        ui->pushButtonOutLightHouses->setChecked(false);
    }

    if (states & HOUSE2_LIGHT) {
        ui->textBrowser->append("HOUSE2_LIGHT");
    }
}

void MainWindow::setWorldLeft(QString img)
{
    QPixmap world(img);
    int w = ui->labelLeft->width();
    int h = ui->labelLeft->height();
    ui->labelLeft->setPixmap(world.scaled(
        w, h,
        Qt::KeepAspectRatio)); // Последний параметр разрешает масштабировать
    // изображение при изменении размера окна
}

void MainWindow::setWorldRight(QString img)
{
    QPixmap world(img);
    int w = ui->labelRight->width();
    int h = ui->labelRight->height();
    ui->labelRight->setPixmap(world.scaled(
        w, h,
        Qt::KeepAspectRatio));
}

void MainWindow::socketDisconected() {
    disconnectFromHost();
}

void MainWindow::socketConnected() {

    isConnected = true;

    ui->labelStatus_2->setText("online");
    ui->labelStatus->setText("online");
    message("Connected!");
}

void MainWindow::connectToHost() {
    QString ip = ui->lineEditIp->text();
    int port = ui->lineEditPort->text().toInt();
    message("Try to connect");

    if (checkIp(ip) && checkPort(port)) {
        if (socket == nullptr) {
            message("To: " + ip + ":" + QString::number(port));

            socket = new QTcpSocket(this);
            connect(socket, SIGNAL(readyRead()), this, SLOT(socketReady()));
            connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconected()));
            connect(socket, SIGNAL(connected()), this, SLOT(socketConnected()));
            socket->connectToHost(ip, port);
        } else if (socket->state() != QAbstractSocket::ConnectedState) {
            disconnectFromHost();
        } else {
            message("Alredy connected!");
        }
    } else {
        qDebug() << "Wrong IP or PORT";
    }
}

void MainWindow::disconnectFromHost() {
    // if(socket->isValid())
    ui->labelStatus_2->setText("offline");
    ui->labelStatus->setText("offline");
    if (socket != nullptr) {
        socket->disconnect();
        socket->deleteLater();
    }
    socket = nullptr;
    message("Disconnected\n");
    isConnected = false;
}

void MainWindow::message(QString msg) {
    qDebug() << msg;
    ui->textBrowser->append(msg);
}

void MainWindow::on_pushButtonCtrl_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_pushButtonSettins_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButtonClose_clicked() {
    sendCommand("&00");
    this->close();
}

void MainWindow::on_pushButtonUp_clicked() { sendCommand("&03"); }

void MainWindow::on_pushButtonDown_clicked() { sendCommand("&04"); }

void MainWindow::on_pushButtonLeft_clicked() { sendCommand("&02"); }

void MainWindow::on_pushButtonRight_clicked() { sendCommand("&01"); }

void MainWindow::on_pushButtonCenter_clicked() { sendCommand("&00"); }

void MainWindow::on_horizontalSlider_valueChanged(int value) {
    ui->lcdNumber->display(value);
}

void MainWindow::on_pushButtonConnect_clicked() {
    writeSettings();
    connectToHost();
}

void MainWindow::on_pushButtonDisconnect_clicked() { disconnectFromHost(); }

void MainWindow::sendCommand(QString command) {
    if (isConnected) {
        if (socket->isValid()) {
            try {
                data = command.toUtf8();
                socket->write(data);
            } catch (...) {
                message("Disconnected!");
                disconnectFromHost();
                ui->labelStatus_2->setText("offline");
            }
            return;
        }
    }
}

void MainWindow::on_pushButtonRoundArrow_clicked() {
    sendCommand("&12");
}

void MainWindow::on_pushButtonDepoArrow_clicked() {
    sendCommand("&11");
}

void MainWindow::on_lineEditPort_textChanged(const QString &arg1) {
    if (arg1.toInt() < 0)
        ui->lineEditPort->setText("0");
    else if (arg1.toInt() > 65535)
        ui->lineEditPort->setText("65535");
}

void MainWindow::on_pushButtonBarrier_clicked() {
    sendCommand("&10");
}

void MainWindow::on_pushButtonOutLightStreet_clicked() {
    sendCommand("&20");
}

void MainWindow::on_pushButtonOutLightHouses_clicked() {
    // ui->labelBtns->setText("Houses light on/off");
    sendCommand("&21");
}

void MainWindow::on_checkBoxLog_stateChanged(int arg1) {
    if (arg1) {
        ui->textBrowser->show();
        return;
    }
    ui->textBrowser->hide();
}

void MainWindow::setStyle()
{
    QFile styleF;
    styleF.setFileName(":/qss/baseStyle.css");
    styleF.open(QFile::ReadOnly);
    QString qssStr = styleF.readAll();
    this->setStyleSheet(qssStr);
}
