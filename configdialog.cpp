#include "configdialog.h"
#include "ui_configdialog.h"
#include "problem.h"

#include <QVector>
#include <QCheckBox>
#include <QMessageBox>
#include <QTableWidget>

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint);
    
    alreadyChangingData=false;
    //ui->tableView->horizontalHeader()->setMaximumHeight(25);
    ui->tableView->horizontalHeader()->setSectionsMovable(true);
    //ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    //ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    
    loadProblems();
    
    connect(&model,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(dataChangedEvent(QModelIndex,QModelIndex)));
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::setModelData(int c)
{
    Problem*problem=NULL;
    if (c<Global::problemNum) problem=&Global::problems[c];
        
    auto configNew=[&]()
    {
        model.item(0,c)->setData("传统型",Qt::EditRole);
        model.item(1,c)->setData(1,Qt::EditRole);
        model.item(2,c)->setData(128,Qt::EditRole);
        model.item(3,c)->setData("全文比较",Qt::EditRole);
        model.item(4,c)->setData(2,Qt::EditRole);
        //model.item(4,c)->setToolTip("true");
        model.item(5,c)->setData(0,Qt::EditRole);
        model.item(6,c)->setData(-1,Qt::EditRole);

        for (int i=0; i<rownum; i++) model.item(i,c)->setFont(Global::boldFont);
        model.horizontalHeaderItem(c)->setFont(Global::boldFont);
    };
    
    if (!problem/*||!problem->que.size()*/) { configNew(); return;}
    
    //model.item(4,c)->setToolTip("false");
    model.item(3,c)->setData((problem->checker=="fulltext"||problem->checker=="fulltext.exe")?"全文比较":problem->checker==".exe"?"":problem->checker,Qt::EditRole);
    if (problem->type==ProblemType::Traditional)
    {
        model.item(0,c)->setData("传统型",Qt::EditRole);
        double minT=1e9,maxT=0,minM=1e9,maxM=0;
        for (auto info:problem->que)
        {
            minT=min(minT,info.timeLim),maxT=max(maxT,info.timeLim);
            minM=min(minM,info.memLim),maxM=max(maxM,info.memLim);
        }
        //if (minT>maxT||minM>maxM) {configNew(); return;}
        if (minT==maxT) model.item(1,c)->setData(minT,Qt::EditRole);
        else if (0<=minT&&maxT<=3600) model.item(1,c)->setData(QString("%1~%2").arg(minT).arg(maxT),Qt::EditRole);
        else
        {
            model.item(1,c)->setData(QString("无效"),Qt::EditRole);
            model.item(1,c)->setFont(Global::boldFont);
        }
        
        if (minM==maxM) model.item(2,c)->setData(minM,Qt::EditRole);
        else if (0<=minM&&maxM<=8192) model.item(2,c)->setData(QString("%1~%2").arg(minM).arg(maxM),Qt::EditRole);
        else
        {
            model.item(2,c)->setData(QString("无效"),Qt::EditRole);
            model.item(2,c)->setFont(Global::boldFont);
        }
        bool optflag=false;
        for(auto info: problem->compilers)
        {
            QString opt=info.file.endsWith(".c")||info.file.endsWith(".cpp")?" -O2":info.file.endsWith(".pas")?" -O2":"";
            if (info.cmd.indexOf(opt)!=-1){
                optflag=true;
                break;
            }
        }
        model.item(5,c)->setData(int(optflag),Qt::EditRole);
        int stacksize=-1;
        for(auto info: problem->compilers)
        {
            QString opt=info.file.endsWith(".c")||info.file.endsWith(".cpp")?" -Wl,-stack=":info.file.endsWith(".pas")?" -Cs":"";
            int loc=info.cmd.indexOf(opt);
            if (loc!=-1){
                loc+=opt.length();
                int j=0;
                for(;loc+j<info.cmd.length()&&info.cmd[loc+j]!=' ';j++);
                stacksize=info.cmd.mid(loc,j).toInt();
                break;
            }
        }
        model.item(6,c)->setData(stacksize,Qt::EditRole);
    }
    else if (problem->type==ProblemType::AnswersOnly)
    {
        model.item(0,c)->setData("提交答案型",Qt::EditRole);
        model.item(1,c)->setText("");
        model.item(1,c)->setEditable(false);
        model.item(2,c)->setText("");
        model.item(2,c)->setEditable(false);
        model.item(5,c)->setEditable(false);
        model.item(6,c)->setText("");
        model.item(6,c)->setEditable(false);
    }
    else
    {
        model.item(0,c)->setData("无效",Qt::EditRole);
        model.item(0,c)->setFont(Global::boldFont);
        model.item(1,c)->setText("");
        model.item(1,c)->setEditable(false);
        model.item(2,c)->setText("");
        model.item(2,c)->setEditable(false);
        model.item(5,c)->setEditable(false);
        model.item(6,c)->setText("");
        model.item(6,c)->setEditable(false);
    }
}

void ConfigDialog::loadProblems()
{
    for (auto i:Global::problemOrder) problemList.append(Global::problems[i].name);
    QStringList tmp=QDir(Global::dataPath).entryList(QDir::Dirs|QDir::NoDotAndDotDot);
    for (auto i:tmp) if (!problemList.count(i)) problemList.append(i);
    num=problemList.size();
    
    model.setRowCount(rownum);
    model.setVerticalHeaderLabels({"题目类型","时间限制","内存限制","比较方式","清空原配置","编译优化","栈大小"});
    model.verticalHeaderItem(0)->setToolTip("试题的类型。");
    model.verticalHeaderItem(1)->setToolTip("试题每个测试点拥有的运行时间上限(仅限传统型试题)。单位: 秒(s)");
    model.verticalHeaderItem(2)->setToolTip("试题每个测试点拥有的运行内存上限(仅限传统型试题)。单位: 兆字节(MB)");
    model.verticalHeaderItem(3)->setToolTip("选手程序输出文件(或答案文件)与标准输出文件的比较方式。");
    model.verticalHeaderItem(4)->setToolTip("清空原来的所有配置。");
    model.verticalHeaderItem(5)->setToolTip("让编译器优化你的代码，即-O2优化。");
    model.verticalHeaderItem(6)->setToolTip("系统栈大小(仅限Windows)。 -1 表示不开栈。单位：字节(Byte)");
    for(int i=0; i<rownum; i++) model.verticalHeaderItem(i)->setTextAlignment(Qt::AlignCenter);
    
    model.setColumnCount(num);
    model.setHorizontalHeaderLabels(problemList);
    ui->tableView->setModel(&model);
    ui->tableView->setItemDelegate(&delegate);
    delegate.problemList=problemList;
    ui->tableView->horizontalHeader()->setTextElideMode(Qt::ElideRight);
    
    for (int i=0; i<num; i++)
    {
        //model.horizontalHeaderItem(i)->setToolTip(problemList[i]);
        for (int j=0; j<rownum; j++) model.setData(model.index(j,i),Qt::AlignCenter,Qt::TextAlignmentRole);
        setModelData(i);
        //for (int j=0; j<4; j++) model.item(j,i)->setToolTip(model.item(j,i)->text());
    }

    /*
    int w=num*ui->tableView->horizontalHeader()->defaultSectionSize()+ui->tableView->verticalHeader()->width()+2;
    int h=5*ui->tableView->verticalHeader()->defaultSectionSize()+ui->tableView->horizontalHeader()->height()+2;
    w=min(max(w,ui->tableView->minimumWidth()),ui->tableView->maximumWidth()-5);
    if (num>12) h+=17;
    ui->tableView->setFixedSize(w,h);
    this->setFixedSize(w+22,h+100);
    //qDebug()<<w<<h;
    */
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void ConfigDialog::dataChangedEvent(const QModelIndex &tl,const QModelIndex &br)
{
    if (alreadyChangingData) return;
    alreadyChangingData=true;
    
    int r=tl.row(),c=tl.column();
    QString text=model.item(r,c)->text();
    model.item(r,c)->setFont(Global::boldFont);
    //model.item(r,c)->setToolTip(text);
    model.horizontalHeaderItem(c)->setFont(Global::boldFont);
    
    if (r==4)
    {
        if (c>=Global::problemNum) model.item(r,c)->setData(2,Qt::EditRole);
        else if (model.item(r,c)->data(Qt::EditRole).toBool())
        {
            model.item(0,c)->setData("传统型",Qt::EditRole);
            model.item(1,c)->setData(1,Qt::EditRole);
            model.item(2,c)->setData(128,Qt::EditRole);
            model.item(3,c)->setData("全文比较",Qt::EditRole);
            model.item(5,c)->setData(0,Qt::EditRole);
            model.item(6,c)->setData(-1,Qt::EditRole);
            for (int i=0; i<rownum; i++) if (i!=4)
            {
                //model.item(i,c)->setToolTip(model.item(i,c)->text());
                model.item(i,c)->setFont(Global::boldFont);
                model.item(i,c)->setEditable(true);
            }
        }
    }
    if (r==5)
    {
        if (c>=Global::problemNum) ;
        else
        if (model.item(r,c)->data(Qt::EditRole).toBool())
        {
            vector<Problem::CompilerInfo> &compilers=Global::problems[c].compilers;
            for (int i=0;i<int(compilers.size());i++) {
                Problem::CompilerInfo &info=compilers[i];
                QString opt=info.file.endsWith(".c")||info.file.endsWith(".cpp")?" -O2":info.file.endsWith(".pas")?" -O2":"";
                if (info.cmd.indexOf(opt)==-1)
                    info.cmd+=opt;
            }
        }
        else
        {
            vector<Problem::CompilerInfo> &compilers=Global::problems[c].compilers;
            for (int i=0;i<int(compilers.size());i++) {
                Problem::CompilerInfo &info=compilers[i];
                QString opt=info.file.endsWith(".c")||info.file.endsWith(".cpp")?" -O2":info.file.endsWith(".pas")?" -O2":"";
                int loc=info.cmd.indexOf(opt);
                if (loc!=-1)
                    info.cmd.remove(loc,opt.length());
            }
        }
    }
    if (r==6)
    {
        if (c>=Global::problemNum) ;
        else{
            vector<Problem::CompilerInfo> &compilers=Global::problems[c].compilers;
            for (int i=0;i<int(compilers.size());i++) {
                Problem::CompilerInfo &info=compilers[i];
                QString opt=info.file.endsWith(".c")||info.file.endsWith(".cpp")?" -Wl,-stack=":info.file.endsWith(".pas")?" -Cs":"";
                int loc=info.cmd.indexOf(opt);
                if (loc!=-1){
                    int j=opt.length();
                    for(;loc+j<info.cmd.length()&&info.cmd[loc+j]!=' ';j++);
                    info.cmd.remove(loc,j);
                }
            }
            if(model.data(tl)!=-1){
                for (int i=0;i<int(compilers.size());i++) {
                    Problem::CompilerInfo &info=compilers[i];
                    QString opt=info.file.endsWith(".c")||info.file.endsWith(".cpp")?" -Wl,-stack=":info.file.endsWith(".pas")?" -Cs":"";
                    info.cmd+=opt+model.data(tl).toString();
                }
            }
        }
    }
    if (r==0)
    {
        if (model.data(tl)=="提交答案型")
        {
            for (int i=1; i<=2; i++)
            {
                model.item(i,c)->setData("",Qt::EditRole);
                //model.item(i,c)->setToolTip("");
                model.item(i,c)->setEditable(false);
            }
            model.item(5,c)->setData("",Qt::EditRole);
            model.item(5,c)->setEditable(false);
            model.item(6,c)->setData("",Qt::EditRole);
            model.item(6,c)->setEditable(false);
        }
        else if (model.item(r,c)->text()=="传统型")
        {
            for (int i=1; i<=2; i++)
            {
                model.item(i,c)->setData(i==1?1:128,Qt::EditRole);
                //model.item(i,c)->setToolTip(model.item(i,c)->text());
                model.item(i,c)->setFont(Global::boldFont);
                model.item(i,c)->setEditable(true);
            }
            model.item(5,c)->setData(0,Qt::EditRole);
            model.item(5,c)->setFont(Global::boldFont);
            model.item(5,c)->setEditable(true);
            model.item(6,c)->setData(-1,Qt::EditRole);
            model.item(6,c)->setFont(Global::boldFont);
            model.item(6,c)->setEditable(true);
        }
    }
    alreadyChangingData=false;
}

void ConfigDialog::on_pushButton_clicked()
{
    QMessageBox::information(this,"Sorry","Will coming soon...");
}

void ConfigDialog::accept()
{
    for (int i=0; i<num; i++)
        for (int j=0; j<3; j++) if (model.item(j,i)->text()=="无效")
        {
            QMessageBox::critical(this,"保存配置失败","存在无效的设置！");
            return;
        }
    
    QStringList list;
    for (int i=0; i<num; i++)
    {
        int t=ui->tableView->horizontalHeader()->logicalIndex(i);
        if (model.horizontalHeaderItem(t)->font().bold())
        {
            QString type=model.item(0,t)->data(Qt::EditRole).toString();
            QString checker=model.item(3,t)->data(Qt::EditRole).toString();
            double tim=model.item(1,t)->data(Qt::EditRole).toDouble();
            double mem=model.item(2,t)->data(Qt::EditRole).toDouble();
            bool opt=model.item(5,t)->data(Qt::EditRole).toBool();
            int stack=model.item(6,t)->data(Qt::EditRole).toInt();
            if (!model.item(0,t)->font().bold()) type="";
            if (!model.item(3,t)->font().bold()) checker="";
            if (!model.item(1,t)->font().bold()) tim=-1;
            if (!model.item(2,t)->font().bold()) mem=-1;
            Problem prob(problemList[t]);
            //qDebug()<<problemList[t]<<type<<checker<<tim<<mem;
            if (model.item(4,t)->data(Qt::EditRole).toBool()) prob.configureNew(type,tim,mem,checker,opt,stack);
            else
            {
                prob=Global::problems[t];
                prob.configure(type,tim,mem,checker);
            }
            if (!prob.saveConfig())
            {
                QMessageBox::critical(this,"保存配置失败","无法写入配置文件！");
                return;
            }
        }
        list.append(problemList[t]);
    }
    //qDebug()<<list;
    Global::saveProblemOrder(list);
    QDialog::accept();
}
