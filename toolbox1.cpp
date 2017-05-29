
#include <QTextCodec>

#include "toolbox1.h"
#include "logindlg.h"
#include <QGroupBox>
#include <QVBoxLayout>

toolbox1::toolbox1(QWidget *parent) :
    QToolBox(parent)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("system"));

    setWindowIcon(QPixmap(":/images/3.png"));//����������ͼ��
    init_username();//--------------�����û���------------------
    init_toolBtn();//--------------��ʼ�������ڿؼ�------------------

    loginDlg login;
    login.exec();
}



void toolbox1::init_toolBtn()
{
    QString imagename;
    for(int i = 0;i<CLIENTCOUNT;i++ )
    {
        toolBtn[i] = new QToolButton();//new 256��toolbutton
        toolBtn[i]->setObjectName(QString::number(i));//Ϊÿ��toolBtn����ObjectName����
        toolBtn[i]->installEventFilter(this);//toolBtn�ĵ���¼���toolbox1������
        toolBtn[i]->setText(username[i]);//����toolBtn����
        imagename.sprintf(":/images/%d.png", i);
        toolBtn[i]->setIcon(QPixmap(imagename));//����toolBtnͼ��
        toolBtn[i]->setIconSize(QPixmap(imagename).size());//����toolBtn��С��ͼ��һ��
        toolBtn[i]->setAutoRaise(true);//����toolBtn�Զ����������
        toolBtn[i]->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);//����toolBtn������ͼ���Ա�
        toolBtn[i]->setToolTip(tr("δ֪"));//����toolBtn �����ʾΪ��δ֪��
        child[i] = new Widget(toolBtn[i]->icon(), i, toolBtn[i]->text(), this);
    }
    QGroupBox *groupBox[8];
    QVBoxLayout *layout[8];
    static int index = 0;
    for(int i=0;i<8;i++)
    {
        groupBox[i] = new QGroupBox;
        layout[i] = new QVBoxLayout(groupBox[i]);
        layout[i]->setMargin(10);//���ò����и��������ʾ���
        layout[i]->setAlignment(Qt::AlignHCenter);//���ò����и������λ��
        for(int j=0;j<32;j++)
        {
            layout[i]->addWidget(toolBtn[index++]);//����һ��layout�м���32��toolbtn
        }
        layout[i]->addStretch();//����һ��ռλ��
    }
    addItem((QWidget *)groupBox[0], tr("����"));
    addItem((QWidget *)groupBox[1], tr("İ����"));
    addItem((QWidget *)groupBox[2], tr("����"));
    addItem((QWidget *)groupBox[3], tr("ͬѧ"));
    addItem((QWidget *)groupBox[4], tr("ͬ��"));
    addItem((QWidget *)groupBox[5], tr("����"));
    addItem((QWidget *)groupBox[6], tr("¿��"));
    addItem((QWidget *)groupBox[7], tr("����"));
}

void toolbox1::init_username()
{
    for(int i = 0;i<CLIENTCOUNT;i++ )//��username[]���������еĳ�Ա��ʼ��
    {
        username[i] = tr("�û�") + QString::number(i);
    }
}

bool toolbox1::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)//�������갴���ź�
    {
        int i = watched->objectName().toInt();
        child[i]->showNormal();//��toolbtn�¶�Ӧ��child��ʾdao ��Ļ
    }
    return QToolBox::eventFilter(watched, event);
}
