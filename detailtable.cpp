#include "detailtable.h"

#include <QScrollBar>
#include <QHeaderView>

DetailTable::DetailTable(QWidget *parent) : QTableWidget(parent)
{
    setup();
}

DetailTable::~DetailTable()
{
    
}

void DetailTable::setup()
{
    this->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
    this->setColumnCount(2);
    this->setMinimumSize(QSize(320, 250));
    this->setFocusPolicy(Qt::NoFocus);
    this->setFrameShape(QFrame::NoFrame);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setProperty("showDropIndicator", QVariant(false));
    this->setDragDropOverwriteMode(false);
    this->setSelectionMode(QAbstractItemView::NoSelection);
    this->setStyleSheet(QLatin1String(
        "QHeaderView\n"
        "{\n"
        "	background:white;\n"
        "}"));    
    
    this->horizontalHeader()->setDefaultSectionSize(45);
    this->horizontalHeader()->setMinimumSectionSize(45);
    this->horizontalHeader()->setStretchLastSection(true);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    
    this->verticalHeader()->setDefaultSectionSize(22);
    this->verticalHeader()->setMinimumSectionSize(22);
    this->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    this->verticalHeader()->setDefaultAlignment(Qt::AlignRight|Qt::AlignVCenter);
    this->verticalHeader()->setMinimumWidth(22);
}

void DetailTable::clearDetail()
{
    this->clear();
    this->setRowCount(0);
    this->setHorizontalHeaderLabels({"得分","详情"});
    this->verticalScrollBar()->setValue(0);
}

void DetailTable::addTitleDetail(int&row,const QString&title)
{
    isScrollBarAtBottom=this->verticalScrollBar()->value()>=this->verticalScrollBar()->maximum()-5;
    
    QTableWidgetItem*tmp=new QTableWidgetItem(title);
    tmp->setTextColor(Qt::white);
    tmp->setBackgroundColor(QColor(120,120,120));
    tmp->setToolTip(title);
    
    this->insertRow(++row);
    this->setItem(row,0,tmp);
    this->setSpan(row,0,1,2);
    this->setVerticalHeaderItem(row,new QTableWidgetItem);
}

void DetailTable::addNoteDetail(int&row,const QString&note,const QString&state)
{
    isScrollBarAtBottom=this->verticalScrollBar()->value()>=this->verticalScrollBar()->maximum()-5;
    
    QTableWidgetItem*tmp=new QTableWidgetItem(note);
    tmp->setToolTip(tmp->text());
    tmp->setTextColor(QColor(80,80,80));
    tmp->setBackgroundColor(QColor(180,180,180));
    if (state=="E") tmp->setTextColor(QColor(0,0,0)),tmp->setBackgroundColor(QColor(227,58,218));
    if (state==" ") tmp->setTextColor(QColor(100,100,100)),tmp->setBackgroundColor(QColor(235,235,235));
    
    int a=tmp->text().split('\n').count(),b=min(a,4)*17+5;
    this->insertRow(++row);
    this->setItem(row,0,tmp);
    this->setSpan(row,0,1,2);
    this->setVerticalHeaderItem(row,new QTableWidgetItem);
    this->verticalHeader()->resizeSection(row,b);
}

void DetailTable::addPointDetail(int&row,int num,const QString&note,const QString&state,const QString&file)
{
    isScrollBarAtBottom=this->verticalScrollBar()->value()>=this->verticalScrollBar()->maximum()-5;
    
    QTableWidgetItem*tmp=new QTableWidgetItem(note);
    tmp->setToolTip(tmp->text());
    
    QColor o(255,255,255);
    if (state=="conf") o.setRgb(0,161,241); //Config
    if (state.length()==1) switch (state[0].toLatin1())
    {
    case 'A':
        o.setRgb(51,185,6); //AC
        break;
    case 'C':
    case 'E':
        o.setRgb(227,58,218); //Error
        break;
    case 'I':
    case 'U':
        o.setRgb(235,235,235); //Ignore/UnSubmit
        break;
    case 'M':
    case 'R':
        o.setRgb(247,63,63); //MLE/RE
        break;
    case 'O':
        o.setRgb(180,180,180); //No Output
        break;
    case 'P':
        o.setRgb(143,227,60); //Partial
        break;
    case 'W':
        o.setRgb(246,123,20); //WA
        break;
    case 'T':
        o.setRgb(255,187,0); //TLE
        break;
    }
    tmp->setBackgroundColor(o);
    
    this->insertRow(++row);
    this->setItem(row,1,tmp);
    
    QTableWidgetItem*t=new QTableWidgetItem(QString::number(num));
    t->setToolTip(file);
    this->setVerticalHeaderItem(row,t);
}

void DetailTable::addScoreDetail(int row,int len,int score,int sumScore)
{
    QTableWidgetItem*tmp=new QTableWidgetItem(QString::number(score));
    tmp->setTextAlignment(Qt::AlignCenter);
    tmp->setToolTip(tmp->text());
    tmp->setBackgroundColor(Global::ratioColor(235,235,235,0,161,241,score,sumScore));
    this->setItem(row-len+1,0,tmp);
}

void DetailTable::showProblemDetail(Player*player,Problem*problem)
{   
    int row=this->rowCount()-1;
    QString title=player->name;
    if (Global::isListUsed&&!player->type&&player->name_list.size()) title=QString("%1 [%2]").arg(player->name,player->name_list);
    if (title=="std") title=QString("\"%1\" 的标程").arg(problem->name);
    else title+=+" - "+problem->name;
    addTitleDetail(row,title);
    
    QFile file(Global::resultPath+problem->name+"/"+player->name+".res");
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        addNoteDetail(row,"无测评结果"," ");
        return;
    }
    
    QDomDocument doc;
    if (!doc.setContent(&file)) {file.close(),addNoteDetail(row,"无效的测评结果"," "); return;}
    QDomElement root=doc.documentElement();
    if (root.isNull()||root.tagName()!="task") {file.close(),addNoteDetail(row,"无效的测评结果"," "); return;}
    
    QDomNodeList list=root.childNodes();
    for (int i=0; i<list.count(); i++) if (list.item(i).toElement().tagName()=="note") addNoteDetail(row,list.item(i).toElement().text(),root.attribute("state"));
    
    for (int i=0,tot=0,tasktot=0; i<list.count(); i++)
    {
        QDomElement a=list.item(i).toElement();
        if (a.tagName()=="subtask")
        {
            QDomNodeList l=a.childNodes();
            int k=0;
            for (int j=0; j<l.count(); j++)
            {
                QDomElement b=l.item(j).toElement();
                if (b.tagName()=="point")
                {
                    QString inout;
                    if (tot<problem->que.size())
                    {
                        inout=QString("标准输入:\"%1\" 标准输出:\"%2\"").arg(problem->que[tot].in).arg(problem->que[tot].out);
                        if (problem->type==ProblemType::AnswersOnly) inout+=QString(" 选手提交:\"%1\"").arg(problem->que[tot].sub);
                    }
                    addPointDetail(row,tot+1,b.attribute("note"),b.attribute("state"),inout);
                    if (k) this->setSpan(row-k,0,k+1,1);
                    tot++,k++;
                }
            }
            
            if (k) addScoreDetail(row,k,a.attribute("score").toInt(),tasktot<problem->tasks.size()?problem->tasks[tasktot].score:0);
            tasktot++;
        }
    }
    file.close();
}

void DetailTable::showConfigDetail()
{
    int row=this->rowCount()-1;    
    for (auto i:Global::problemOrder)
    {
        Problem*prob=&Global::problems[i];
        addTitleDetail(row,QString("\"%1\" 的配置结果").arg(prob->name));
        
        int t=0;
        for (auto i:prob->tasks)
        {
            int k=0;
            for (auto j:i.point)
            {
                Problem::Info*x=&prob->que[j];
                QString inout=QString("标准输入:\"%1\" 标准输出:\"%2\"").arg(x->in).arg(x->out);
                if (prob->type==ProblemType::AnswersOnly) inout+=QString(" 选手提交:\"%1\"").arg(x->sub);
                addPointDetail(row,++t,inout,"conf",inout);
                if (k) this->setSpan(row-k,0,k+1,1);
                k++;
            }
            addScoreDetail(row,i.point.size(),i.score,i.score);
        }
    }
}

void DetailTable::showDetailEvent(int r, int c)
{
    if (Global::alreadyJudging||(Global::clickTimer.isValid()&&Global::clickTimer.elapsed()<1000)) return;
    clearDetail();
    r=Global::logicalRow(r);
    if (c>1) showProblemDetail(&Global::players[r],&Global::problems[c-2]);
    else for (auto i:Global::problemOrder) showProblemDetail(&Global::players[r],&Global::problems[i]);
}

void DetailTable::adjustScrollbar()
{
    QCoreApplication::processEvents();
    QScrollBar*bar=this->verticalScrollBar();
    if (isScrollBarAtBottom) bar->setValue(bar->maximum());
}
