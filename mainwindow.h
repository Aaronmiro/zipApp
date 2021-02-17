#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QString password = "123456";
    int code = 66; //加密密码
    QString uncompress_suffix = "";
    QString file_path_name = "";
    QString file_name = "";
    bool is_logined = false;
    bool uncom_batch_success = true;
    QStringList fileNames;
    QString dirpath;

    void show_user_labels();
    void show_manager_lables();
    void zipWriter(QString path, QString fileName);
    void zipReader(QString path, QString fileName);
    void zipReader_batch(QString path, QString fileName);
    void manager_login();
    void manager_logout();
    void progress_show();
    void progress_hide();
    QStringList getFileNames(QString &path);

};
#endif // MAINWINDOW_H
