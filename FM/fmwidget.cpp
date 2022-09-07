#include "fmwidget.h"
#include "ui_fmwidget.h"
#include <QStandardItem>
#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>



const int m_col=6;//显示6列

FMWidget::FMWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FMWidget)
{

    ui->setupUi(this);
    initStyle();


    //创建model
    m_model = new QStandardItemModel();
    m_renameModel =new QStandardItemModel(this);
    pathName = QApplication::applicationDirPath();//可执行文件夹目录

    initData();

//    Qt监控目录文件变化：QFileSystemWatcher用法
//    信号：directoryChanged 是监控提供的目录的变化
//   （1）先声明对象    QFileSystemWatcher m_FileWatcher;
//   （2）添加需要监控的目录   m_FileWatcher.addPath(path);
//   （3）绑定信号槽
    m_fileWatcher.addPath(pathName);
    connect(&m_fileWatcher,&QFileSystemWatcher::directoryChanged,
            this,&FMWidget::WatcherFile);



}


//界面样式设计
void FMWidget::initStyle()
{
    QString btnStyle ="QToolButton{background-color:#409EFF;margin:0px 0px 0px 10px;padding:12px 20px;color:#FFFFFF;font:14px 新黑体;border-radius: 4px;}"
                      "QToolButton:hover{background-color:#66b1ff;}"
                      "QToolButton:pressed{background-color:#409EFF;}"
                      "QToolButton:hover:!pressed{background-color:#66b1ff;}"
                      "QLineEdit{background-color:#FFFFFF;padding:0px 15 px;height: 35px;color: #606266;border: 1px solid #dcdfe6;border-radius: 4px;}"
                      "QLineEdit:hover{border-color: #c0c4cc;border-top-color: rgb(192, 196, 204);border-right-color: rgb(192, 196, 204);border-bottom-color: rgb(192, 196, 204);border-left-color: rgb(192, 196, 204);}"
                      "QLineEdit:focus{border-color:rgb(64,158,255);}"

            ;
    this->setStyleSheet(btnStyle);
    ui->lineEdit->setPlaceholderText("请输入文件名");//设置QLineEdit背景提示文字
    this->setWindowIcon(QIcon(":/images/FM_title.ico"));

    //按钮设置图标
    ui->btn_add->setIcon(QIcon(":/images/uploading.ico"));
    ui->btn_add->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    ui->btn_del->setIcon(QIcon(":/images/del.ico"));
    ui->btn_del->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    ui->btn_rename->setIcon(QIcon(":/images/mod.ico"));
    ui->btn_rename->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    ui->btn_search->setIcon(QIcon(":/images/search.ico"));
    ui->btn_search->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    ui->btn_open_dir->setIcon(QIcon(":/images/open.ico"));
    ui->btn_open_dir->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);


}

void FMWidget::initData()
{
    m_model->clear();//使用前记得清除
    m_fileListPath_NO_Name.clear();//这两个QStringList是成员变量用完之后要清楚数据，不然数据会一直保留
    m_fileListPath.clear();

    QStringList nameFilter;//设置过滤器
    nameFilter<<"*.txt" <<"*.exe";
    QDir m_dir(pathName);
    m_homePath = m_dir.homePath();//保存桌面路径
    QList<QFileInfo>  nameList = m_dir.entryInfoList(nameFilter);
    int m_count =nameList.count();
    //qDebug()<<"m_count: "<<m_count;
    QFileInfo mListInfo[m_count];
    int search_i=0;
    for(int i=0;i<m_count;i++)//i 文件个数
    {
        if(searchFalg == true)//开启搜索
        {
            if(!nameList.at(i).baseName().contains(searchStr))
               continue;
        }else
        {
            search_i = i;
        }

        mListInfo[i] = nameList.at(i);
        m_fileListPath_NO_Name<<mListInfo[i].absolutePath();//保存文件绝对路径列表,不包括文件名
        m_fileListPath<<mListInfo[i].filePath();//保存文件绝对路径列表,包括文件名
       // qDebug()<<"mListInfo[i]: "<<mListInfo[i];


        //名称    创建日期    修改日期    类型  大小  文件路径
        //j 列数
        for(int j=0;j<m_col;j++)//设置model一行数据
        {
            QStandardItem *m_item1 =new QStandardItem(mListInfo[i].baseName());
            QStandardItem *m_item2 =new QStandardItem(mListInfo[i].birthTime().toString("yyyy.MM.dd hh:mm:ss.z"));
            QStandardItem *m_item3 =new QStandardItem(mListInfo[i].lastModified().toString("yyyy.MM.dd hh:mm:ss.z"));
            QStandardItem *m_item4 =new QStandardItem(mListInfo[i].completeSuffix());
            QStandardItem *m_item5 =new QStandardItem(QString::number(mListInfo[i].size()/1024));
            QStandardItem *m_item6 =new QStandardItem(mListInfo[i].filePath());

                m_model->setItem(search_i,0,m_item1);
                m_model->setItem(search_i,1,m_item2);
                m_model->setItem(search_i,2,m_item3);
                m_model->setItem(search_i,3,m_item4);
                m_model->setItem(search_i,4,m_item5);
                m_model->setItem(search_i,5,m_item6);



        }
        search_i++;//只有当搜索开启时search_i才有意义，否则它就是i
    }

    //设置model水平头数据
    QStringList headerNameList;
    headerNameList <<"名称"<<"创建日期"<<"修改日期"<<"类型"<<"大小"<<"文件路径";
    m_model->setHorizontalHeaderLabels(headerNameList);

    //把model模型插入table
    ui->tableView->setModel(m_model);

    //qt tableview表头自适应宽度
    //QHeaderView::Stretch 平均      还是这样比较有美感
    //QHeaderView::ResizeToContents 内容显示最优
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //设置单行选中
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
}

FMWidget::~FMWidget()
{
    delete ui;
}

//表格单击事件
void FMWidget::on_tableView_clicked(const QModelIndex &index)
{
     m_current_row=index.row();//返回当前选中行
     //qDebug()<<"表格单击事件"<<m_current_row;
}

//打开文件所在目录时，必须选择一行
//这里所有的文件所在的目录其实是一样的
void FMWidget::on_btn_open_dir_clicked()
{
//    qDebug()<<"打开目录："<<m_current_row;
//    qDebug()<<m_fileListPath_NO_Name;
//    qDebug()<<m_fileListPath_NO_Name[m_current_row];

    //-1代表未选中
    if(m_current_row == -1)
    {
        QMessageBox::warning(nullptr,QString("提示"),QString("请先选择一个文件"));
        return;
    }
    QString currentRowFilePath =m_fileListPath_NO_Name[m_current_row];
    QFileDialog::getOpenFileName(nullptr,QString("文件"),currentRowFilePath);
}

//上传文件
void FMWidget::on_btn_add_clicked()
{
    if(pathName.isEmpty())
    {
        QMessageBox::warning(nullptr,QString("提示"),QString("根文件路径获取失败,正在重新获取..."));
        pathName = QApplication::applicationDirPath();//可执行文件夹目录
    }
    QFile m_file(pathName);
    QString openfilename = QFileDialog::getOpenFileName(nullptr,QString("文件"),m_homePath,"*.txt;;*.exe");
    int StartPost = openfilename.lastIndexOf('/');
    int EndPost = openfilename.lastIndexOf('.');
    QString newName =pathName+openfilename.mid(StartPost,EndPost);
    bool ret=m_file.copy(openfilename,newName);

     //qDebug()<<"openfilename"<<openfilename;
     //qDebug()<<"newName"<<newName;
    if(true == ret)
    {
        qDebug()<<"上传文件成功.";
        QMessageBox::warning(nullptr,QString("提示"),QString("上传文件成功"));
    }else
    {
        qDebug()<<"上传文件失败.";
        QMessageBox::warning(nullptr,QString("提示"),QString("上传文件失败"));
    }
    //initData();
}


//删除选中行
void FMWidget::on_btn_del_clicked()
{
    if(pathName.isEmpty())
    {
        QMessageBox::warning(nullptr,QString("提示"),QString("根文件路径获取失败,正在重新获取..."));
        pathName = QApplication::applicationDirPath();//可执行文件夹目录
    }

     QString currentRowFilePath =m_fileListPath[m_current_row];
     QDir m_dir(pathName);
     bool ret=m_dir.remove(currentRowFilePath);


      //qDebug()<<"删除 "<<m_fileListPath;
      //qDebug()<<"删除: "<<currentRowFilePath;
     if(true == ret)
     {
         qDebug()<<"删除文件成功.";
         QMessageBox::warning(nullptr,QString("提示"),QString("删除文件成功"));
     }else
     {
        qDebug()<<"删除文件失败.";
        QMessageBox::warning(nullptr,QString("提示"),QString("删除文件失败"));
     }
     //initData();
}



//搜索文件(通过名字)
void FMWidget::on_btn_search_clicked()
{
    searchStr = ui->lineEdit->text();
    if(searchStr.isEmpty())
    {
        initData();
        return;
    }


    searchFalg = true;
    initData();

    searchFalg = false;
    searchStr.clear();
}




//双击表格
void FMWidget::on_tableView_doubleClicked(const QModelIndex &index)
{
    //获取当前点击项的model、行、列
    m_renameModel = (QStandardItemModel*)index.model();

    m_renameModel_row =index.row();
    m_renameModel_col =index.column();

}

//重命名
void FMWidget::on_btn_rename_clicked()
{
    if(pathName.isEmpty())
    {
        QMessageBox::warning(nullptr,QString("提示"),QString("根文件路径获取失败,正在重新获取..."));
        pathName = QApplication::applicationDirPath();//可执行文件夹目录
    }

    if(m_renameModel_col != 0)return;//只能修改第一列，即名字。
    QString tempNewName =m_renameModel->item(m_renameModel_row,m_renameModel_col)->text();
    //qDebug()<<m_renameModel->item(m_renameModel_row,m_renameModel_col)->text();

    QString oldName=m_fileListPath[m_renameModel_row];
    int startPost=oldName.lastIndexOf('.');
    QString str1 =oldName.mid(startPost);
    QString newName=m_fileListPath_NO_Name[m_renameModel_row]+'/'+tempNewName+str1;
    //qDebug()<<newName;
    QFile m_file(pathName);
    bool ret = m_file.rename(oldName,newName);
    if(true == ret)
    {
        qDebug()<<"修改文件名成功.";
        QMessageBox::warning(nullptr,QString("提示"),QString("修改文件名成功"));
    }else
    {
       qDebug()<<"修改文件名失败.";
       QMessageBox::warning(nullptr,QString("提示"),QString("修改文件名失败"));
    }

    //initData();
}

//自动监视文件槽函数
void FMWidget::WatcherFile()
{
    initData();
}
