#include <QTextCodec>

#include "toolbox1.h"
#include "logindlg.h"
#include <QGroupBox>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QHostAddress>

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

    if(login.islogin)
    {
        if((login.userid < 0) || (login.userid > 255))
        {
            QMessageBox::information(this, tr("����"), tr("��Ч�û�ID"));
        }else
        {
            setWindowTitle(username[login.userid]);
            userid = login.userid;
            passwd = login.passwd;
            hostip = login.hostip;
            hostport = login.hostport;

            //���ݵ�¼�Ի������û�����ķ�����IP�Ͷ˿ں����ӵ�������
            sockClient = new QTcpSocket(this);

            connect(sockClient, SIGNAL(error(QAbstractSocket::SocketError)), this,
                    SLOT(sock_Error(QAbstractSocket::SocketError)));
            connect(sockClient, SIGNAL(readyRead()), this, SLOT(read_Msg()));
            connect(sockClient, SIGNAL(connected()), this, SLOT(socket_connected()));

            QHostAddress hostAddr(hostip);
            sockClient->connectToHost(hostAddr, hostport);
        }
    }else
    {
        exit(0);
    }
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



void toolbox1::sock_Error(QAbstractSocket::SocketError sockErr)
{
    switch(sockErr)
    {
    case QAbstractSocket::RemoteHostClosedError://��������������ر���socket����ֱ��break;
        break;
    default:
        QMessageBox::information(this, tr("����"), sockClient->errorString());
    }
}

void toolbox1::read_Msg()
{
}

void toolbox1::socket_connected()
{
    login_Msg();
}

void toolbox1::login_Msg()
{
    const char *pw = passwd.toStdString().data();
    if(sockClient->isOpen())//�ж�socket�Ƿ��Ѿ����ӵ�Զ�̷����
    {
        if(sockClient->state() == QAbstractSocket::ConnectedState)
        {
            struct msg_t msg;
            memset(&msg, 0, sizeof(msg));
            msg.head[0] = 1;//������ϢΪ��¼��Ϣ
            msg.head[1] = userid;
            msg.head[2] = 0;
            msg.head[3] = 0;
            strncpy(msg.body, pw, strlen(pw));
            sockClient->write((const char *)&msg, sizeof(msg.head) + strlen(msg.body));
        }
    }
}
