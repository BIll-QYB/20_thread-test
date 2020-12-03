#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread> //
//#include <QtDebug>//
#include <QSerialPort>
#include <QString>
#include <QtDebug>
#include <QByteArray>
#include <QLabel>
#include <QTime>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

//尝试模仿qt_demo22_mutes，中的number定义一个全局变量
//static int number = 0;
extern QByteArray to_th; //声明全局变量to_th,并去cpp文件定义
//static int status_btn_th_send = 0; //因为用static会出现。h文件和。cpp打印的值不一样的情况，所以用extern试试
extern int status_btn_th_send; //只是声明全局变量，去。cpp里面去定义

//先声明线程类，以防在主窗口类中使用线程类时报错
class SystemTimeThread;
class MyThread;    //

//线程类
class MyThread:public QThread//--------------------------user code---------------------------------------------------------------//
{
    Q_OBJECT
public:
      explicit MyThread(QWidget *parent = nullptr)
    {

    }

    void run() override
    {
        //只执行一次
        //子线程处理事务的地方，默认这个函数不能维护长期运行
        //线程函数循环体，打印两行东西

        while(1)
        {
                QThread::sleep(1);
                qDebug() << " ==== myThread" << QThread::currentThread();
                qDebug() << " ==== myThread" <<QThread::currentThread()->objectName();

                //-------------------------------------------------串口配置---------------------------------------------
                QSerialPort global_port;
                global_port.setParity(QSerialPort::NoParity);
                global_port.setDataBits(QSerialPort::Data8);
                global_port.setStopBits(QSerialPort::OneStop);
                //端口设置成UNO-142401
                //global_port.setPortName("/dev/cu.usbmodem142401");
                //端口设置成MEGA2560连接的CP2102，查看主程序能否发送
               //global_port.setPortName("/dev/cu.SLAB_USBtoUART");
                //WINDOW端口设置成COM3---UNO
               // global_port.setPortName("COM3");
                //端口设置成MEGA-142301
               global_port.setPortName("/dev/cu.usbmodem142201");

               //波特率设置成9600
               global_port.setBaudRate(QSerialPort::Baud9600);

               //-------------------------------------------------打开串口-------------------------------------------------
                global_port.open(QIODevice::ReadWrite);
  //              qDebug() << "Thread open the port";
               // global_port.write("TH send 10 to arduino uno");

                 //-------------------------------------------------读取串口-------------------------------------------------
                //读取串口数据并打印
                qDebug()<< "1.port name is:" <<global_port.portName();
                qDebug()<<"2.WaitForReadyRead status:"<< global_port.waitForReadyRead();




                 //------------------------------------------------判读UI中鼠标是否按下，去串口发送信息出去---------------------------------
                 if (status_btn_th_send == 1 )
                 {
                     qDebug()<<"9.判断后输出鼠标数值："<< status_btn_th_send;
                      qDebug()<<"判断后输出to_th数值："<< to_th;
                      qDebug()<<"7.子线程输出to_th数值："<< to_th;
                     // qDebug()<<"8.子线程输出鼠标数值：" << status_btn_th_send;
                      global_port.write(to_th);

                 }
                 if (global_port.waitForReadyRead())
                 {
                     //              qDebug()<<"after Wait-For-ReadyRead signal";
                                   //qDebug()<<global_port.clear();
                                   QByteArray array_r = global_port.readAll();
                      //              qDebug()<< "3.array_r:"<<array_r << "--end";
                                    //读取串口缓冲区的所有数据给临时变量temp
                     //               QString a =global_port.readAll();
                     //               qDebug()<< "4.Qstring a:"<< a <<"--end";
                                    qDebug()<<"5.readfinish";
                                    //qDebug()<<"6.port clear:" << global_port.clear();
                                    //global_port.close();

                                    //------------------------------------------------发送读取数据去主流程------------------------------------------------
                                    //尝试emit信号出去
                                    // 模仿SystemTimeThread，去emit信号出去
                                    //emit sendInfoTime(QTime::currentTime().toString("hh:mm:ss"));
                                    global_port.clear();

                                    emit sendthread_read(array_r);

                 }




                 //尝试判断btn_th_send按钮是否按下.没有找到解决方案






         }
    }

   ~MyThread()//
    {

    }
    //尝试新建一个信号，用于线程读取的数值，传出去
signals:                                                                                             //
     //模仿SystemTimeThread类里面新建一个信号，用于子线程emit去传参                                                                           //
     //void sendInfoTime(const QString&time);
    void sendthread_read(const QByteArray array_r);



};                       //--------------------------user code------end---------------------------------------------------------//
class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

     QLabel* getTimeLabel();

private slots:
    //启动线程槽函数
    void on_startBtn_clicked();
    //关闭线程槽函数
    void on_stopBtn_clicked();

    void on_startBtn_2_clicked();

    void on_stopBtn_2_clicked();

    void on_startBtn_3_clicked();

     void main_receive_data();

     void update_time1(const QString& time);

     void display_th(QByteArray);

     void main_send_data(bool);

     void transfer_th(bool);//尝试新建一个槽函数，用于接收子线程的发送按钮动作。并讲label数值赋值给static QByteArray to_th

     void btn_th_send_clicked(bool);//尝试新疆一个槽函数，用于接收子线程的发送按钮的抬起动作。将status_btn_th_send 置2

     void btn_th_send_pressed();//尝试新建一个槽函数，用于用于接收子线程的发送按钮的按下动作。将status_btn_th_send 置2

private:
    Ui::MainWindow *ui;
    MyThread* thread; //add here
    QSerialPort Main_port;
    QByteArray array_mr;
    SystemTimeThread* timeThread;

};


//线程更新时间
class SystemTimeThread:public QThread                    //
{                                                        //
    Q_OBJECT                                             //
public:                                                  //
    explicit SystemTimeThread(QWidget *parent = nullptr) //
    {
                                                         //
        obj =parent;                                     //
    }                                                    //
    void run() override                                  //
    {                                                    //
        while (1){                                       //
            QThread::sleep(1);                           //
            //方式可以在子线程更新UI，但不建议，容易引起ANR现象 //
            MainWindow* p = (static_cast<MainWindow*>(obj));                                         //
          //  ((MainWindow*)obj)->getTimeLabel()->setText(QTime::currentTime().toString("hh:mm:ss"));  //
            emit sendInfoTime(QTime::currentTime().toString("hh:mm:ss"));                            //  Why emit out????????????????
            //emit sendInfo();  //
        }
    }
signals:                                                                                             //
     //该信号只发通知，不带数据                                                                          //
    // void sendInfo();                                                                                 //What use of sendInfo()?
    //该信号发通知并带数据                                                                              //
     void sendInfoTime(const QString&time);                                                          //
private:                                                                                             //
     QObject* obj;                                                                                   //
};




#endif // MAINWINDOW_H

//there is private slots in the video,need to type. two private slots is:open and close.should be linked with the button












