#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Команды для отправки на ESP-01 для Меги
/** Состав команды
  1 символ command[0] игнорируем
  2 символ command[1] группа параметров
    &0 - Поезд
      &00 - остановить
      &01 - начать движение вправо
      &02 - начать движение влево
      &03 - увеличить скорость
      &04 - уменьшить скорость
      &05 - установить скорость
        &05ХХХ - 3 символа собрать в строку и перевести в инт
    &1 - Механика мира
      &10 - шлакбаум открыть/закрыть
      &11 - стрелка депо
      &12 - стрелки мира
    &2 - Свет
      &20 - вкл/выкл свет улица
      &21 - свет дома
      &22 - свет 2
      &23 - свет 3

    &8 - ping
**/

#include <QMainWindow>
#include <QTcpSocket>
#include <QSettings>
#include <QRegularExpressionValidator>
#include <QPixmap>
#include <QTimer>
#include <QFile>

enum STATES {
    TRAIN_STOPPED = 1,
    TRAIN_MOVING_LEFT = 2,
    TRAIN_MOVING_RIGHT = 4,
    BARRIER_OPEN = 8,
    DEPO_ARROW = 16,
    BIG_ROUND = 32,
    STREET_LIGHT = 64,
    HOUSE1_LIGHT = 128,
    HOUSE2_LIGHT = 256
};

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void message(QString msg);

public slots:
    void socketReady();
    void socketDisconected();
    void socketConnected();

private slots:
    void connectionTimerSlot();
    void uiTimerSlot();

    void on_pushButtonCtrl_clicked();

    void on_pushButtonSettins_clicked();

    void on_pushButtonClose_clicked();

    void on_pushButtonUp_clicked();

    void on_pushButtonDown_clicked();

    void on_pushButtonLeft_clicked();

    void on_pushButtonRight_clicked();

    void on_pushButtonCenter_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_pushButtonConnect_clicked();

    void on_pushButtonDisconnect_clicked();

    void on_pushButtonRoundArrow_clicked();

    void on_pushButtonDepoArrow_clicked();

    void on_lineEditPort_textChanged(const QString &arg1);

    void on_pushButtonBarrier_clicked();

    void on_pushButtonOutLightStreet_clicked();

    void on_pushButtonOutLightHouses_clicked();

    void on_checkBoxLog_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;

    bool l_btn_active = false;
    bool r_btn_active = false;
    bool barrier_btn_active = false;

    void setStyle();

    QTimer *pingTimer;
    int pingTimerMs = 10000;
    bool isConnected = false;
    bool ping = false;

    void startPingTimer();

    QTimer *uiTimer;
    int uiTimerMs = 1000;

    void startUiTimer();
    void uiAnimation();
    int states;

    QTcpSocket* socket = nullptr;
    QByteArray data;
    QRegularExpression ipRegex;

    void sendCommand(QString command);
    void connectToHost();
    void disconnectFromHost();
    void readSettings();
    void writeSettings();
    bool checkIp(QString &ip);
    bool checkPort(int &port);

    void showWorldStatus(QString &data);
    void setWorldLeft(QString img);
    void setWorldRight(QString img);

    bool bigRound = true;
};
#endif // MAINWINDOW_H

/** Состав команды
  1 символ command[0] игнорируем
  2 символ command[1] группа параметров
    &0 - Поезд
      &00 - остановить
      &01 - начать движение вправо
      &02 - начать движение влево
      &03 - увеличить скорость
      &04 - уменьшить скорость
      &05 - установить скорость
        &05ХХХ - 3 символа собрать в строку и перевести в инт
    &1 - Механика мира
      &10 - шлакбаум открыть/закрыть
      &11 - стрелка депо
      &12 - стрелки мира
    &2 - Свет
      &20 - вкл/выкл весь свет
      &21 - свет 1
      &22 - свет 2
      &23 - свет 3

    &8 - PING
**/

// Принимаемые команды от ESP-01
/** Состав команды
  1 символ command[0] игнорируем
  2 символ command[1] группа параметров
    &0 - Поезд
      &00 - Поезд остановлен
      &01 - Поезд едет вправо
      &02 - Поезд едет влево
      &03 - Текущая скорость
        &03ХХХ - 3 символа скорость
    &1 - Механика мира
      &10Х - шакбаум Х = 1 открыт/ Х = 0 закрыт
      &11Х - стрелка депо Х = 1 в депо / 0 по кругу
      &12Х - стрелки мира Х = 1 большой круг / 0 маленький
    &2 - Свет Х = 1 вкл / 0 выкл
      &20Х - весь свет
      &21Х - свет 1
      &22Х - свет 2
      &23Х - свет 3
**/
