#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class QDragEnterEvent;
class QDropEvent;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // 拖拽进入窗口时触发
    void dragEnterEvent(QDragEnterEvent *event) override;

    // 在窗口上松开鼠标时触发
    void dropEvent(QDropEvent *event) override;

    

private:
    Ui::MainWindow *ui;

    void loadSettings(); // 加载设置
    void saveSettings(); // 保存设置
    QWidget* createRow();

private slots:
    void on_help_triggered(); 
    void on_setting_triggered();
    void on_pushout_triggered();

};
