#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <private/qzipwriter_p.h>
#include <private/qzipreader_p.h>
#include <QDir>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QTextCodec>

using namespace std;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    show_user_labels();

    connect(ui->upload_btn,&QPushButton::clicked,[=]{
        ui->file_name_lable->setText("");
        ui->compressed_file_lable->setText("");
        ui->compress_btn->setDisabled(true);
        file_path_name = "";
        file_name = "";
        progress_hide();

//        指定父类、标题、默认打开后显示的目录、右下角的文件过滤器
        file_path_name = QFileDialog::getOpenFileName(this,"please choose a file",".","*");

        if(file_path_name == ""){
            return ;
        }

        file_name = file_path_name.mid(file_path_name.lastIndexOf("/")+1);

        if(file_name == ""){
            return ;
        }

        if(file_name.mid(file_name.lastIndexOf(".")+1) == "zip"){
            QMessageBox::warning(this, "warning", "the suffix of the file cannot be the zip！");
            return ;
        }

        ui->file_name_lable->setText(file_name);

        if(file_name!= nullptr){
            ui->compress_btn->setDisabled(false);
        }
        QMessageBox::information(this, "information", "uploaded successfully！");
    });

    connect(ui->compress_btn,&QPushButton::clicked,[=]{
        qDebug() << "开始压缩";
        qDebug() << "file_path_name"<<file_path_name;
        qDebug() << "file_name"<<file_name;
        zipWriter(file_path_name,file_name);
    });

    connect(ui->manager_login_btn,&QPushButton::clicked,[=]{
        if(is_logined){
            manager_logout();
        }else{
            manager_login();
        }
    });

    connect(ui->choose_btn,&QPushButton::clicked,[=]{
        ui->choose_dir_label->hide();
        ui->choose_file_label->show();
        ui->file_name_lable->setText("");
        ui->compressed_file_lable->setText("");
        ui->uncompress_btn->setDisabled(true);
        ui->uncompress_batch_btn->setDisabled(true);
        file_path_name = "";
        file_name = "";
        progress_hide();

        file_path_name = QFileDialog::getOpenFileName(this,"please choose a file",".","*.zip");
        qDebug() << file_path_name;
        if(file_path_name == ""){
            return ;
        }
        file_name = file_path_name.mid(file_path_name.lastIndexOf("/")+1);
        qDebug() << file_name;

        if(file_name == ""){
            return ;
        }

        if(file_name.mid(file_name.lastIndexOf(".")+1) != "zip"){
            QMessageBox::warning(this, "warning", "the suffix of the file must be the zip！");
            return ;
        }

        ui->file_name_lable->setText(file_name);

        if(file_name!= nullptr){
            ui->uncompress_btn->setDisabled(false);
        }

    });

    connect(ui->uncompress_btn,&QPushButton::clicked,[=]{
        qDebug() << "开始解压";
        uncompress_suffix = QInputDialog::getText(this,"suffix","please input the suffix of the decompressed file(like .pdf/.docx/.xlsx)",QLineEdit::Normal,".xlsx");
        if(uncompress_suffix == ""){
            return ;
        }

//        QTextCodec *pcode = QTextCodec::codecForName("UTF-8");
//        auto file_path_name_std = pcode->fromUnicode(file_path_name).data();
//        auto file_name_std = pcode->fromUnicode(file_name).data();
//        zipReader(QString::fromStdString(file_path_name_std),QString::fromStdString(file_name_std));

        zipReader(file_path_name,file_name);

    });

    connect(ui->choose_dir_btn,&QPushButton::clicked,[=]{
        progress_hide();
        ui->uncompress_batch_btn->setDisabled(true);
        ui->uncompress_btn->setDisabled(true);
        ui->file_name_lable->setText("");
        ui->compressed_file_lable->setText("");
        ui->choose_dir_label->show();
        ui->choose_file_label->hide();
        fileNames.clear();
        dirpath="";
        dirpath = QFileDialog::getExistingDirectory(this, "choose a directory", "./", QFileDialog::ShowDirsOnly);
        qDebug() << "dirpath" << dirpath;
        if(dirpath!=""){
            fileNames = getFileNames(dirpath);
            qDebug() << fileNames;
            ui->file_name_lable->setText(dirpath+"\t"+"the number of zip files："+QString::number(fileNames.size()));
            if(fileNames.size()!=0){
                ui->uncompress_batch_btn->setDisabled(false);
            }
        }
    });

    connect(ui->uncompress_batch_btn,&QPushButton::clicked,[=]{
        uncom_batch_success = true;
        uncompress_suffix = QInputDialog::getText(this,"suffix","please input the suffix of decompressed files(like.pdf/.docx/.xlsx)",QLineEdit::Normal,".xlsx");
        if(uncompress_suffix == ""){
            return ;
        }

        ui->progressBar->setRange(0,fileNames.size()-1);
        progress_show();
        for(int i = 0; i< fileNames.size();i++)
        {
            QString fileName = fileNames.at(i);
            qDebug()<<"fileName ="<< fileName;
            QString file_path = dirpath+"/"+fileName;
            qDebug()<<"file_path ="<< file_path;
            ui->progressBar->setValue(i);
            zipReader_batch(file_path,fileName);
        }
        if(uncom_batch_success){
            ui->compressed_file_lable->setText(dirpath);
            QMessageBox::information(this, "information", "decompressed successfully！");
        }
        progress_hide();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::show_user_labels()
{
    ui->compress_file_lable->show();
    ui->compress_file_lable_2->hide();
    ui->upload_file_label->show();
    ui->choose_file_label->hide();
    ui->choose_dir_label->hide();
    ui->choose_btn->setDisabled(true);
    ui->uncompress_btn->setDisabled(true);
    ui->choose_dir_btn->setDisabled(true);
    ui->uncompress_batch_btn->setDisabled(true);
    ui->upload_btn->setDisabled(false);
    ui->compress_btn->setDisabled(true);
    ui->file_name_lable->setText("");
    ui->compressed_file_lable->setText("");

    progress_hide();
}

void MainWindow::show_manager_lables()
{
    ui->file_name_lable->setText("");
    ui->compressed_file_lable->setText("");

    ui->compress_file_lable_2->show();
    ui->compress_file_lable->hide();

    ui->choose_file_label->show();
    ui->upload_file_label->hide();

    ui->choose_btn->setDisabled(false);
    ui->uncompress_btn->setDisabled(true);

    ui->choose_dir_btn->setDisabled(false);
    ui->uncompress_batch_btn->setDisabled(true);

    ui->upload_btn->setDisabled(true);
    ui->compress_btn->setDisabled(true);

    progress_hide();
}

void MainWindow::zipWriter(QString file_path_name, QString fileName)
{
    progress_show();
    ui->progressBar->setRange(1,100);

    //添加文件夹
//    writer->addDirectory(path + "/ss");
    //添加文件
    QFile file(file_path_name);
    if (!file.exists())
    {
        QMessageBox::warning(this, "warning", "the file does not exist！");
        progress_hide();
        ui->compressed_file_lable->setText("");
        file.close();
        return;
    }

    QString zip_path = file_path_name.mid(0,file_path_name.lastIndexOf(".")) +".zip";
    QZipWriter writer(zip_path);

    file.open(QIODevice::ReadOnly);
    ui->progressBar->setValue(1);
    QByteArray fileArray = file.readAll().toBase64();

    for (int i = 0;i<fileArray.size();i++)
    {
        fileArray[i] = fileArray[i] + code;//加密密码
    }
    ui->progressBar->setValue(50);
    writer.addFile(fileName, fileArray);

    file.close();
    writer.close();
    qDebug()<<"压缩结束";
    ui->progressBar->setValue(100);
    ui->compressed_file_lable->setText(zip_path);
    QMessageBox::information(this, "information", "compressed successfully！");
    progress_hide();
}

void MainWindow::zipReader(QString path, QString fileName)
{
    progress_show();
    ui->progressBar->setRange(1,100);
    QZipReader reader(path);
//    reader.extractAll(path+"/output");

    QString uncompress_file_name = fileName.mid(0,fileName.lastIndexOf(".")) + uncompress_suffix;
    QString uncompress_file_path = path.mid(0,path.lastIndexOf("/")+1);

    qDebug()<<uncompress_file_name;
    qDebug()<<uncompress_file_path;

    QString file_path = uncompress_file_path + uncompress_file_name;
    qDebug()<<"file_path:"<<file_path;

    QFile file(file_path);
    file.open(QIODevice::WriteOnly);
    ui->progressBar->setValue(1);
    QByteArray readArray = reader.fileData(uncompress_file_name);
    if(readArray.isEmpty()) {
        progress_hide();
        ui->compressed_file_lable->setText("");
        file.close();
        file.remove();
        reader.close();
        QMessageBox::warning(this, "warning", "failed decompression！");
        return;
    }

    ui->progressBar->setValue(30);
    for (int i = 0;i<readArray.size();i++)
    {
        readArray[i] = readArray[i] - code;//加密密码
    }
    ui->progressBar->setValue(50);
    readArray = QByteArray::fromBase64(readArray);

    file.write(readArray);
    file.close();
    reader.close();
    qDebug()<<"解压结束";
    ui->progressBar->setValue(100);
    ui->compressed_file_lable->setText(uncompress_file_path + uncompress_file_name);
    QMessageBox::information(this, "information", "decompressed successfully！");
    progress_hide();
}

void MainWindow::zipReader_batch(QString path, QString fileName)
{
    QZipReader reader(path);
    QString uncompress_file_name = fileName.mid(0,fileName.lastIndexOf(".")) + uncompress_suffix;
    QString uncompress_file_path = path.mid(0,path.lastIndexOf("/")+1);
    QString file_path = uncompress_file_path + uncompress_file_name;
    QFile file(file_path);
    file.open(QIODevice::WriteOnly);
    QByteArray readArray = reader.fileData(uncompress_file_name);
    if(readArray.isEmpty()) {
        ui->compressed_file_lable->setText("");
        file.close();
        file.remove();
        reader.close();
        QMessageBox::warning(this, "warning", fileName+": "+"failed decompression！");
        uncom_batch_success = false;
        return;
    }

    for (int i = 0;i<readArray.size();i++)
    {
        readArray[i] = readArray[i] - code;//加密密码
    }
    readArray = QByteArray::fromBase64(readArray);
    file.write(readArray);
    file.close();
    reader.close();
}

void MainWindow::manager_login()
{
    QString password = QInputDialog::getText(this,"login","please enter the password",QLineEdit::Password,"");
    if(this->password == password){
        show_manager_lables();
        qDebug() <<"password: "<<password;
        is_logined = true;
        ui->manager_login_btn->setText("manager logout");
        QMessageBox::information(this, "information", "login successfully！");
    }else if (password == "") {
        is_logined = false;
    }else {
        QMessageBox::warning(this, "warning", "wrong password！");
        is_logined = false;
    }
}

void MainWindow::manager_logout()
{
    ui->manager_login_btn->setText("manager login");
    show_user_labels();
    is_logined = false;
}

void MainWindow::progress_show()
{
    ui->progressBar->show();
}

void MainWindow::progress_hide()
{
    ui->progressBar->reset();
    ui->progressBar->hide();
}

QStringList MainWindow::getFileNames(QString &path)
{
    QDir dir(path);
    QStringList nameFilters;
    nameFilters << "*.zip";
    QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);
    return files;
}
