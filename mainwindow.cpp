#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QByteArray>

#include <QSerialPort>//
#include <QDebug>

//尝试定义全局变量
 int status_btn_th_send = 5;
 QByteArray to_th = "hello";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //主程序读串口：
    //学习serial例子，进行connect信号连接
    //connect(&global_port,&QSerialPort::readyRead,this,&serial::receive_data); //teacher write &receive_data
    connect(&Main_port,&QSerialPort::readyRead,this,&MainWindow::main_receive_data);
    //主程序发送
    //模仿serial_twoport进行按钮信号与发送槽函数的连接
    //connect(ui->btn_send,&QPushButton::clicked,this,&serial::btn_send_data);
    connect(ui->btn_main_send,&QPushButton::clicked,this,&MainWindow::main_send_data);
    //连接子线程发送按钮信号与transfer槽函数，将子线程label的输入值赋值给QByteArray to_th
    //connect(ui->btn_th_send,&QPushButton::pressed,this,&MainWindow::transfer_th);//clicked要连接bool类型的transfer函数
    connect(ui->btn_th_send,&QPushButton::pressed,this,&MainWindow::btn_th_send_pressed); //pressed连接无类型的pressed函数
    //尝试连接鼠标抬起的事件，
    connect(ui->btn_th_send,&QPushButton::clicked,this,&MainWindow::btn_th_send_clicked);


   //线程1:读取串口信息并打印
    thread = new MyThread;//
    thread->setObjectName("th");//
   //尝试子线程的读取信号连接主程序的显示槽函数
    connect(thread,&MyThread::sendthread_read,this,&MainWindow::display_th);


    //线程2:设置系统时间到控件上
  //  ui->timeLabel->setText(QTime::currentTime().toString("hh:mm:ss"));       //Why set it in main process?We do it in the thread,didn't we?
    timeThread = new SystemTimeThread(this);



    //   connect(timeThread,&SystemTimeThread::sendInfo,
    //           this,&MainWindow::update_time);
    connect(timeThread,&SystemTimeThread::sendInfoTime,                      //
            this,&MainWindow::update_time1);                                 //
   //启动线程
    timeThread->start();

   //尝试判断btn_th_send按钮是否按下



}

MainWindow::~MainWindow()
{
    delete ui;
}

//------------------------------------Right cliked slots for two button-----automatic created-----------------------------------------------------------------//

void MainWindow::on_startBtn_clicked()
{
    thread->start();
}

void MainWindow::on_stopBtn_clicked()
{
    thread->terminate();
    thread->wait();
}

void MainWindow::on_startBtn_2_clicked()
{


    qDebug() << " ==== Main process";

    //串口配置
    //QSerialPort Main_port; 在mainwindow.h中已经定义，这样在开启和关闭槽函数中都可以用
    Main_port.setParity(QSerialPort::NoParity);
    Main_port.setDataBits(QSerialPort::Data8);
    Main_port.setStopBits(QSerialPort::OneStop);
    //端口设置成MEGA-142301
    Main_port.setPortName("/dev/cu.usbmodem142201");
    //端口设置成MEGA2560连接的CP2102，查看主程序能否发送
    //Main_port.setPortName("/dev/cu.SLAB_USBtoUART");

   //波特率设置成9600
    Main_port.setBaudRate(QSerialPort::Baud9600);
   //打开串口
    Main_port.open(QIODevice::ReadWrite);
    qDebug() << "Main process open the port";
    Main_port.write("QT Main porcess send 10 to arduino Mega2560");

    //读取串口数据并打印
    qDebug()<< "M1.port name is:" <<Main_port.portName();
    qDebug()<<"M2.WaitForReadyRead status:"<< Main_port.waitForReadyRead();
    //qDebug()<<Main_port.clear();


}

void MainWindow::on_stopBtn_2_clicked()
{
    //Main_port.clear();
    qDebug()<< "M7.Press close button:WaitForReadyRead status:";
    Main_port.close();
   // qDebug()<< Main_port.waitForReadyRead(); //这个会把关闭按钮卡死，注释掉就解决了哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈
}

void MainWindow::on_startBtn_3_clicked()
{    //当读取按键按下时
    //将读取内容显示在UI上plainTextEdit_main
             //方法：根据serial_twoport的receive函数进行模仿
            //ui->plainTextEdit->insertPlainText(QString(array_r));
            //ui->plainTextEdit_main->insertPlainText(QString(array_mr));


    //读取串口数据并打印
 //   qDebug()<< "M1.port name is:" <<Main_port.portName();
 //   qDebug()<<"M2.WaitForReadyRead status:"<< Main_port.waitForReadyRead();
    //qDebug()<<Main_port.clear();
   // QByteArray array_mr = Main_port.readAll();
    // qDebug()<< "M3.array_mr:"<<array_mr << "--end";
    //   qDebug()<<array_mr;
     //读取串口缓冲区的所有数据给临时变量ma
  //   QString ma =Main_port.readAll();
     //qDebug()<< "M4.Qstring a:"<< ma <<"--end";
 //    qDebug()<<"M5.readfinish";
     //qDebug()<<"M6.port clear:" << Main_port.clear();
     //Main_port.close();

    }

void MainWindow::main_receive_data()
{
      array_mr = Main_port.readAll();
      //qDebug()<< "M3.array_mr:"<<array_mr << "--end";
// qDebug()<<array_mr;
      //读取串口缓冲区的所有数据给临时变量ma
   //   QString ma =Main_port.readAll();
      //qDebug()<< "M4.Qstring a:"<< ma <<"--end";
  //    qDebug()<<"M5.readfinish";
      //qDebug()<<"M6.port clear:" << Main_port.clear();
      //Main_port.close();
        //将读取内容显示在UI上plainTextEdit_main
        ui->plainTextEdit_main->insertPlainText(QString(array_mr));

}

void MainWindow::update_time1(const QString &time)                           //
{
       ui->timeLabel->setText(time);       //Test:seems no use. Since Thread had set label time,why set it in main process?
}

void MainWindow::display_th(QByteArray rfth)
{
    ui->plainTextEdit_th->insertPlainText(rfth);
}

void MainWindow::main_send_data(bool)
{
    //模仿serial_twoport进行显示
    //QString data = ui->lint_send_data->text();
    QString mainsd =this->ui->lint_main_send->text();
    QByteArray mainsd_l = mainsd.toLatin1(); //QString -->QByteArray
    Main_port.write(mainsd_l);
    qDebug()<<mainsd_l;

}

void MainWindow::transfer_th(bool) //鼠标按下执行的程序
{
    QString thsd = ui->lint_th_send->text();
    QByteArray thsd_1 = thsd.toLatin1();
    to_th = thsd_1;
   // return  to_th;

    qDebug() <<"------主程序输出to_th值："<< to_th;
     status_btn_th_send = 1;
    qDebug()<<"鼠标按下状态："<< status_btn_th_send;


}

void MainWindow::btn_th_send_clicked(bool) //鼠标松开执行的程序

{
      status_btn_th_send = 2;
    qDebug()<<"鼠标松开状态："<< status_btn_th_send;
}

void MainWindow::btn_th_send_pressed() //鼠标按下执行的程序
{
      status_btn_th_send = 1;
    qDebug()<<"鼠标按下状态："<< status_btn_th_send;
    QString thsd = ui->lint_th_send->text();
    QByteArray thsd_1 = thsd.toLatin1();
    to_th = thsd_1;
    qDebug() <<"------主程序输出to_th值："<< to_th;
    qDebug()<<"鼠标按下状态："<< status_btn_th_send;



}
