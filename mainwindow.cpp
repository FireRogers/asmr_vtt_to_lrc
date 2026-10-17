#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "orgenizeRj.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileInfo>
#include <QTextEdit>
#include <QList>
#include <QDebug>
#include <QUrl>
#include <QMimeData>
#include <QString>
#include <QComboBox>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QLineEdit>
#include <QTextDocument>
#include <QTextBlock> 
#include <QCheckBox>

#include <filesystem>
namespace fs = std::filesystem;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->helpwidget->setVisible(false); 
    ui->settingwidget->setVisible(false);
    ui->textEdit->setAcceptDrops(false); 
    loadSettings();
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

void MainWindow::loadSettings()
{
    QFile save("save.txt");

    if(!save.open(QIODevice::Text | QIODevice::ReadOnly)) 
    {
        save.close();
        qInfo()<<"未能打开设置文件";
        return;
    }
    

    QComboBox *combobox = ui->DefaultChoose;
    QLineEdit *photopath = ui->PhotoPath;
    QTextStream line(&save);

    line.setEncoding(QStringConverter::Utf8);

    qDebug() << "当前工作目录:" << QDir::currentPath();

    QString line1 = line.readLine();
    QString line2 = line.readLine();
    QString line3 = line.readLine();

    if(!line1.isEmpty()) photopath->setText(line1); 
    else photopath->clear();

    if (!line2.isEmpty()) combobox->setCurrentText(line2);
    else combobox->setCurrentText("需转移图片");

    if (!line3.isEmpty() && line3 == "1") ui->checkBox->setChecked(1);
    else ui->checkBox->setChecked(0);

    save.close();
}

void MainWindow::saveSettings()
{
    QFile save("save.txt");
    if(!save.open(QIODevice::Text | QIODevice::ReadWrite|QIODevice::Truncate)) qInfo() << "未能打开设置文件";
    QTextStream text(&save);
    QComboBox *combobox = ui->DefaultChoose;
    QLineEdit *photopath = ui->PhotoPath;

    bool del = ui->checkBox->isChecked();

    text.setEncoding(QStringConverter::Utf8); 

    if(photopath->text().isEmpty()) text << '\n';
    else text << photopath->text() << '\n';

    text << combobox->currentText() << '\n';
    
    text << del << '\n';
    
    text.flush();
    save.close();
}

void MainWindow::on_help_triggered()
{
    ui->settingwidget->setVisible(false);
    ui->helpwidget->setVisible(true); // 让 helpwidget 显示出来
}

void MainWindow::on_setting_triggered()
{
    ui->helpwidget->setVisible(false); 
    ui->settingwidget->setVisible(true);
}

void MainWindow::on_pushout_triggered()
{
    orgenizeRj RJ;

    QTextEdit * textedits = ui->textEdit;//ui输出行

    std::string photoPath = ui->PhotoPath->text().toUtf8().toStdString();//输出时的默认图片路径

    QComboBox *combobox = ui->DefaultChoose;

    std::string choose = combobox->currentText().toStdString();

    QTextDocument *DocumentEdit = textedits->document();
    int lineN = DocumentEdit->blockCount();
    bool del = ui->checkBox->isChecked();

    for(int i = 0; i < lineN;i++)
    {
        QString Qline = DocumentEdit->findBlockByLineNumber(i).text();

        fs::path path(Qline.toStdWString());

        std::string line = Qline.toUtf8().toStdString();

        if(line.size() < 4) 
        {
            qInfo() << "输入过短,不要直接输入一个硬盘啊";
            continue;
        }
        else if(!fs::is_regular_file(path) && !fs::is_directory(path))
        {
            qInfo() << "没有找到这个路径的文件";
            continue;
        }

        if(Qline.endsWith(".vtt",Qt::CaseInsensitive))
        {
            if(!RJ.organizeRj(line,"",3,del))
            {
                qInfo() << "失败";
                continue;
            }
        }
        else if(fs::is_directory(path) && choose == "需转移图片")
        {
            if(!RJ.organizeRj(line,photoPath,2,del))
            {
                qInfo() << "失败";
                continue;
            }
        }
        else if(fs::is_directory(path) && choose == "所有vtt文件")
        {
            if(!RJ.organizeRj(line,"",1,del))
            {
                qInfo() << "失败";
                continue;
            }
        }

    }
    textedits->clear();
    
}

void MainWindow::on_pushout_2_triggered()
{
    on_pushout_triggered();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
    else event->ignore();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QList<QUrl> urls = event->mimeData()->urls();

    QTextEdit * textEdit = ui->textEdit;

    if(urls.isEmpty())
    {
        return;
    }
    
    for(const QUrl &url : urls)
    {
        if (!url.isLocalFile()) continue;
        
        QString path = url.toLocalFile();
        QFileInfo info(path);
        if (!info.exists()) continue;

        path = info.absoluteFilePath();

        textEdit->append(path);
        
    }

    event->acceptProposedAction();  // 告诉系统拖拽成功

}


