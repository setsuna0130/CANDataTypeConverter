#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QProcess>
#include <QDir>
#include <QFileDialog>

#include <QDebug>
#include <QDate>
#include<QStringList>
#include<QMessageBox>
#include <QVector>

QProcess process;

uint8_t TransType = 0;
QString fileName="";
QString SaveFileName="";


QVector<asc_type> asc_data;
QList<QStringList> output_data;
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

    switch (TransType) {
    case 0:
        fileName = QFileDialog::getOpenFileName(this, tr("选择cantools保存文件"), "", tr(" cantools保存txt文件 (*.txt *.xls)"));
        if(false ==readCantools(fileName, output_data))
            QMessageBox::information(this,"提示信息","源文件格式不正确，请进行第一步");
        else
        {
            ui->lineEdit->setText(fileName);
            QMessageBox::information(this,"提示信息","源文件读取成功");
            ui->label_3->setText(QString::number(output_data.size()));
        }
        break;
    case 1:
        fileName = QFileDialog::getOpenFileName(this, tr("选择Santroll保存文件"), "", tr(" Santroll保存txt文件 (*.txt)"));
        if(false == readSantroll(fileName,output_data))
            QMessageBox::information(this,"提示信息","源文件格式不正确，请进行第一步");
        else
        {
            ui->lineEdit->setText(fileName);
            QMessageBox::information(this,"提示信息","源文件读取成功");
            ui->label_3->setText(QString::number(output_data.size()));
        }
        break;
    case 2:
        fileName = QFileDialog::getOpenFileName(this, tr("选择cantest保存文件"), "", tr(" cantools保存txt文件 (*.txt)"));
        if(false == readCantestTxt(fileName,output_data))
            QMessageBox::information(this,"提示信息","源文件格式不正确，请进行第一步");
        else
        {
            ui->lineEdit->setText(fileName);
            QMessageBox::information(this,"提示信息","源文件读取成功");
            ui->label_3->setText(QString::number(output_data.size()));
        }
        break;

    case 3:
        fileName = QFileDialog::getOpenFileName(this, tr("选择cantest保存文件"), "", tr(" cantools保存csv文件 (*.csv)"));
        if(false==readCantestCSV(fileName,output_data))
            QMessageBox::information(this,"提示信息","源文件格式不正确，请进行第一步");
        else
        {
            ui->lineEdit->setText(fileName);
            QMessageBox::information(this,"提示信息","源文件读取成功");
            ui->label_3->setText(QString::number(output_data.size()));
        }
        break;
    default:
        break;
    }
//    if(fileName != "")
//    {
//        QFile aFile(fileName);
//        if(!aFile.open(QIODevice::ReadOnly))
//            return ;

//      array =   aFile.readLine();
//         array =   aFile.readLine();
////        while (!aFile.atEnd())//判断文件是否读完
////        {
////             array = aFile.readLine().trimmed();
////        }

//        QStringList array_cut = array.split("\t");//以逗号分割
//        aFile.close();
//    }



}


void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    TransType = ui->comboBox->currentIndex();
    qDebug()<<TransType<<Qt::endl;
}


void MainWindow::on_pushButton_2_clicked()
{
    if(fileName == "")
    {
        QMessageBox::information(this,"提示信息","源文件名为空，请进行第二步选择源文件!");
        return ;
    }


    SaveFileName = QFileDialog::getSaveFileName(this, tr("另存为"), "", tr(" asc格式文件 (*.asc)"));
    qDebug()<<SaveFileName<<Qt::endl;


    QFile file(SaveFileName);
    if(SaveFileName == "")
    {
        QMessageBox::information(this,"提示信息","文件名为空，请输入文件名!");
        ui->label_6->setText("失败");
        return ;
    }

    if(file.exists())
    {
        file.remove();
    }

    file.open(QIODevice::ReadWrite);


    QLocale locale = QLocale::English;
    QString format = "ddd MMM dd hh:mm:ss AP yyyy";
    QString currentTime =locale.toString(QDateTime::currentDateTime(), format) ;
    qDebug()<<currentTime<<Qt::endl;
    file.write("date "+currentTime.toUtf8()+"\n");
    file.write("base hex timestamps absolute\n");

    switch (TransType) {
    case 0:
        MainWindow::cantools2asc(output_data,asc_data);
        break;
    case 1:
        MainWindow::santrolltxt2asc(output_data,asc_data);
        break;
    case 2:
        MainWindow::cantesttxt2asc(output_data,asc_data);
        break;
    case 3:
        MainWindow::cantestCSV2asc(output_data,asc_data);
        break;
    default:
        break;
    }

    for(int i =0 ; i<asc_data.size();i++)
    {
        file.write(asc_data.at(i).timestamps.toUtf8()+'\t'+\
                   asc_data.at(i).Channels.toUtf8()+' '+\
                   asc_data.at(i).ID.toUtf8()+'\t'+\
                   asc_data.at(i).Dir.toUtf8()+'\t'+\
                   asc_data.at(i).Type.toUtf8()+'\t'+\
                   asc_data.at(i).DLC.toUtf8()+'\t'+\
                   asc_data.at(i).Data.toUtf8()+'\t'+'\n');
    }
    QMessageBox::information(this,"提示信息","转换成功");
    ui->label_6->setText("成功");
}


bool readCSV(QString filePath, QList<QStringList>& data)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream stream(&file);
    while (!stream.atEnd())
    {
        QString line = stream.readLine();
        QStringList row = line.split(',', Qt::SkipEmptyParts);
        data.append(row);
    }

    file.close();
    return true;
}

bool MainWindow::readSantroll(QString filePath,  QList<QStringList>& data)
{
    data.clear();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream stream(&file);


    QStringList aaa = {"110743.235", "04F0D000", "0000000000000000"};
    QStringList headline = stream.readLine().split('\t', Qt::SkipEmptyParts);
    if(aaa.at(0).size() == headline.at(0).size() && aaa.size() == headline.size())
    {
        data.append(headline);
    }
    else
    {
        return false;
    }
    while (!stream.atEnd())
    {
        QString line = stream.readLine();
        QStringList row = line.split('\t', Qt::SkipEmptyParts);
        data.append(row);
    }

    return true ;
}

bool MainWindow::readCantools(QString filePath,  QList<QStringList>& data)
{
    data.clear();
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream stream(&file);
    stream.setCodec("GB18030");
    QStringList headline;
    QStringList aaa = {"Seq" ,"Time" , "CANInd","Orentation","ID","Frame","Type","DLC","DATA"} ;

    headline = stream.readLine().trimmed().split('\t');
    if(aaa != headline)
    {
        return false;
    }
    while (!stream.atEnd())
    {
        QString line = stream.readLine();
        QStringList row = line.split('\t', Qt::SkipEmptyParts);
        data.append(row);
    }

    file.close();
    return true;
}

bool MainWindow::readCantestCSV(QString filePath , QList<QStringList>& data)
{
    data.clear();
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream stream(&file);
    stream.setCodec("GB18030");
    QStringList aaa = {"序号" ,"传输方向" , "时间戳","ID","帧格式","帧类型","长度","数据",""} ;
    QStringList bbb = {"序号" ,"传输方向" , "时间标识","帧ID","帧格式","帧类型","数据长度","数据(HEX)"} ;
    QStringList headline = stream.readLine().trimmed().split(',');
    if(aaa != headline && bbb != headline)
    {
        return false;
    }
    while (!stream.atEnd())
    {
        QString line = stream.readLine();
        QStringList row = line.split(',', Qt::SkipEmptyParts);
        data.append(row);
    }
    file.close();
    return true;

}

bool MainWindow::readCantestTxt(QString filePath, QList<QStringList>& data)
{
    data.clear();
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream stream(&file);
    stream.setCodec("GB18030");
    QStringList aaa = {"序号" ,"传输方向" , "时间戳","ID","帧格式","帧类型","长度","数据"} ;

    QStringList headline = stream.readLine().trimmed().split('\t');
    if(aaa != headline)
    {
        return false;
    }
    while (!stream.atEnd())
    {
        QString line = stream.readLine();
        QStringList row = line.split('\t', Qt::SkipEmptyParts);
        data.append(row);
    }

    file.close();
    return true;

}

bool MainWindow::cantools2asc(QList<QStringList>& data , QVector<asc_type> & asc_data)
{
    asc_data.clear();
    QString time = data.at(0).at(1);
    QDateTime start = QDateTime::fromString(time, "hh:mm:ss:zzz");
//    QString time1 = data.at(1).at(1);
//    QDateTime end = QDateTime::fromString(time1, "hh:mm:ss:zzz");
//    qDebug()<<end.secsTo(start)<<Qt::endl;
//    qDebug()<<end.msecsTo(start)<<Qt::endl;
//    qDebug()<< time  << Qt::endl;
//    qDebug()<< time1 << Qt::endl;
    asc_type row ;
    for (int i = 0; i < data.size(); ++i)
    {
        QString timetmp = data.at(i).at(1);
        QDateTime end = QDateTime::fromString(timetmp, "hh:mm:ss:zzz");

        qint64 totalmsec = start.msecsTo(end);

        qint64 sec = totalmsec/1000;
        qint64 msec = totalmsec%1000;
        QString msec_str = QString::number(msec);
        QString strNew = QString("%1").arg(msec_str.toInt(), 3, 10, QLatin1Char('0'));
        row.timestamps = QString::number(sec) + "."+strNew;
        row.Channels = "1";
        row.ID = data.at(i).at(4);
        row.ID.replace(" ","");
        if(data.at(i).at(3) == "接收")
            row.Dir = "Rx";
        if(data.at(i).at(6) == "扩展帧")
            row.Type ="d" ;
        row.DLC = data.at(i).at(7);
        row.Data = data.at(i).at(8);
        asc_data.append(row);

    }

    return true;
}


bool MainWindow::cantestCSV2asc(QList<QStringList>& data , QVector<asc_type> & asc_data)
{
    asc_data.clear();
    QString time = data.at(0).at(2);
    QDateTime start = QDateTime::fromString(time, "hh:mm:ss.zzz.0");

     asc_type row ;
    for (int i = 0; i < data.size(); ++i)
    {
        QString timetmp = data.at(i).at(2);
        QDateTime end = QDateTime::fromString(timetmp, "hh:mm:ss.zzz.0");
        qint64 totalmsec = start.msecsTo(end);

        qint64 sec = totalmsec/1000;
        qint64 msec = totalmsec%1000;
        QString msec_str = QString::number(msec);
        QString strNew = QString("%1").arg(msec_str.toInt(), 3, 10, QLatin1Char('0'));
        row.timestamps = QString::number(sec) + "."+strNew;
        row.Channels = "1";
        row.ID = data.at(i).at(3);
        row.ID.replace("0x","");
        if(data.at(i).at(1) == "接收")
            row.Dir = "Rx";
        if(data.at(i).at(4) == "扩展帧" || data.at(i).at(4) == "数据帧")
            row.Type ="d" ;

        if(data.at(i).at(6) == "0x00")
            row.DLC = "0";
        else if(data.at(i).at(6) == "0x01")
            row.DLC = "1";
        else if(data.at(i).at(6) == "0x02")
            row.DLC = "2";
        else if(data.at(i).at(6) == "0x03")
            row.DLC = "3";
        else if(data.at(i).at(6) == "0x04")
            row.DLC = "4";
        else if(data.at(i).at(6) == "0x05")
            row.DLC = "5";
        else if(data.at(i).at(6) == "0x06")
            row.DLC = "6";
        else if(data.at(i).at(6) == "0x07")
            row.DLC = "7";
        else if(data.at(i).at(6) == "0x08")
            row.DLC = "8";
        row.Data = data.at(i).at(7);
        asc_data.append(row);

    }

    return true;
}

bool MainWindow::cantesttxt2asc(QList<QStringList>& data , QVector<asc_type> & asc_data)
{
    asc_data.clear();
    QString time = data.at(0).at(2);
    QDateTime start = QDateTime::fromString(time, " hh:mm:ss.zzz.0");

    asc_type row ;
    for (int i = 0; i < data.size(); ++i)
    {
        QString timetmp = data.at(i).at(2);
        QDateTime end = QDateTime::fromString(timetmp, " hh:mm:ss.zzz.0");
        qint64 totalmsec = start.msecsTo(end);

        qint64 sec = totalmsec/1000;
        qint64 msec = totalmsec%1000;
        QString msec_str = QString::number(msec);
        QString strNew = QString("%1").arg(msec_str.toInt(), 3, 10, QLatin1Char('0'));
        row.timestamps = QString::number(sec) + "."+strNew;
        row.Channels = "1";
        row.ID = data.at(i).at(3);
        row.ID.replace("0x","");
        if(data.at(i).at(1) == "接收")
            row.Dir = "Rx";
        if(data.at(i).at(5) == "扩展帧")
            row.Type ="d" ;

        row.DLC = data.at(i).at(6);


        row.Data = data.at(i).at(7);
        asc_data.append(row);

    }

    return true;
}


bool MainWindow::santrolltxt2asc(QList<QStringList>& data , QVector<asc_type> & asc_data)
{
    asc_data.clear();
    QString time = data.at(0).at(0);
    QDateTime start = QDateTime::fromString(time, "hhmmss.zzz");



    asc_type row ;
    for (int i = 0; i < data.size(); ++i)
    {
        QString timetmp = data.at(i).at(0);
        QDateTime end = QDateTime::fromString(timetmp, "hhmmss.zzz");
        qint64 totalmsec = start.msecsTo(end);

        qint64 sec = totalmsec/1000;
        qint64 msec = totalmsec%1000;
        QString msec_str = QString::number(msec);
        QString strNew = QString("%1").arg(msec_str.toInt(), 3, 10, QLatin1Char('0'));
        row.timestamps = QString::number(sec) + "."+strNew;
        row.Channels = "1";
        row.ID = data.at(i).at(1);
        row.Dir = "Rx";
        row.Type ="d" ;
        row.DLC = "8";
        QString dataStr = data.at(i).at(2);
        int n = dataStr.length();
        while(n-2 > 0)
        {
            n = n - 2;
            dataStr.insert(n," ");
        }
        row.Data = dataStr;
        asc_data.append(row);

    }
    return true ;
}
