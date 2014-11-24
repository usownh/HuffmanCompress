#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&huffman,SIGNAL(message(QString)),this,SLOT(message(QString)));
    connect(&huffman,SIGNAL(error(QString)),this,SLOT(error(QString)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_OpenFile_clicked()
{
    QString fileName=QFileDialog::getOpenFileName(this,tr("Open File"), "/", "");
    ui->label_FilePath->setText(fileName);
    if(fileName.endsWith(".huf"))
    {
        fileName=fileName.mid(0,fileName.lastIndexOf("."));
        ui->lineEdit->setText(fileName+".dec.txt");
    }
    else
    {
        fileName=fileName.mid(0,fileName.lastIndexOf("."));
        ui->lineEdit->setText(fileName+".huf");
    }
}

void MainWindow::message(QString m)
{
    ui->textBrowser->append(m);
}
void MainWindow::error(QString m)
{
    QMessageBox::warning(this,"error",m);
    this->message("task failed!");

}

void MainWindow::on_pushButton_Compress_clicked()
{
    if(check())
    {
        this->message("start to Compress!");
        huffman.Compress(ui->label_FilePath->text(),ui->lineEdit->text());
    }
}

void MainWindow::on_pushButton_Decompress_clicked()
{
    if(check())
    {
        this->message("start to Decompress!");
        huffman.Decompress(ui->label_FilePath->text(),ui->lineEdit->text());
    }
}
bool MainWindow::check()
{
    if(ui->label_FilePath->text().isEmpty())
    {
        this->error("file name can't be empty!");
        return false;
    }
    return true;
}

void MainWindow::on_pushButton_Save_clicked()
{
    QString fileName=QFileDialog::getSaveFileName(this,tr("Save File"), "/", ".huf");
    ui->lineEdit->setText(fileName+".huf");
}
