#ifndef FMWIDGET_H
#define FMWIDGET_H

#include <QWidget>
#include <QStandardItemModel>

#include <QFileSystemWatcher>

QT_BEGIN_NAMESPACE
namespace Ui { class FMWidget; }
QT_END_NAMESPACE

class FMWidget : public QWidget
{
    Q_OBJECT

public:
    FMWidget(QWidget *parent = nullptr);
    void initStyle();
    void initData();
    ~FMWidget();

private slots:
    void on_btn_open_dir_clicked();
    void on_tableView_clicked(const QModelIndex &index);
    void on_btn_del_clicked();
    void on_btn_add_clicked();
    void on_btn_search_clicked();
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_btn_rename_clicked();
    void WatcherFile();

private:
    Ui::FMWidget *ui;
    QStandardItemModel *m_model;
    QStringList m_fileListPath_NO_Name;//保存文件绝对路径列表,不包括文件名
    QStringList m_fileListPath;//保存文件绝对路径列表,包括文件名
    int m_current_row =-1;//当前选中行,-1代表未选中
    QString pathName;//应用程序目录
    QString m_homePath;//保存桌面路径
    bool searchFalg = false;//开始搜索标志，默认关闭
    QString searchStr;//获取输入框的值

    //获取当前点击项的文本
    QStandardItemModel *m_renameModel;//用来接收当前点击项的model指针

    //用来接收当前点击项的model的行、列
    int m_renameModel_row;
    int m_renameModel_col;

    QFileSystemWatcher m_fileWatcher;



};
#endif // FMWIDGET_H
