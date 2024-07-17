#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

typedef struct {
    QString timestamps;
    QString Channels;
    QString ID;
    QString Dir;
    QString Type;
    QString DLC;
    QString Data;
}asc_type;
Q_DECLARE_METATYPE(asc_type)
extern QVector<asc_type> asc_data;

class MainWindow : public QMainWindow
{
    Q_OBJECT




public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool readCSV(QString filePath, QList<QStringList>& data);
    bool readSantroll(QString filePath,  QList<QStringList>& data);
    bool readCantools(QString filePath ,  QList<QStringList>& data);
    bool readCantestCSV(QString filePath, QList<QStringList>& data);
    bool readCantestTxt(QString filePath, QList<QStringList>& data);

    bool cantools2asc(QList<QStringList>& data , QVector<asc_type> & asc_data);
    bool cantestCSV2asc(QList<QStringList>& data , QVector<asc_type> & asc_data);
    bool cantesttxt2asc(QList<QStringList>& data , QVector<asc_type> & asc_data);
    bool santrolltxt2asc(QList<QStringList>& data , QVector<asc_type> & asc_data);

    bool CovertToASC();
    bool CovertToCSV();
private slots:
    void on_pushButton_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_pushButton_2_clicked();

    void on_comboBox_2_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
