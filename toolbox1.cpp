#include <QTextCodec>
#include "toolbox1.h"
#include "logindlg.h"
#include <QGroupBox>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QHostAddress>
#include <QDebug>

bool toolbox1::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)//如果是鼠标按键信号
    {
        int i = watched->objectName().toInt();
        child[i]->showNormal();//将toolbtn下对应的child显示dao 屏幕
    }
    return QToolBox::eventFilter(watched, event);
}

void toolbox1::init_username()
{
    for(int i = 0;i<CLIENTCOUNT;i++ )//将username[]数组中所有的成员初始化
    {
        username[i] = tr("用户") + QString::number(i);
    }
}

void toolbox1::init_toolBtn()
{
    QString imagename;
    for(int i = 0;i<CLIENTCOUNT;i++ )
    {
        toolBtn[i] = new QToolButton();//new 256个toolbutton
        toolBtn[i]->setObjectName(QString::number(i));//为每个toolBtn设置ObjectName属性
        toolBtn[i]->installEventFilter(this);//toolBtn的点击事件由toolbox1来处理
        toolBtn[i]->setText(username[i]);//设置toolBtn标题
        imagename.sprintf(":/images/%d.png", i);
        toolBtn[i]->setIcon(QPixmap(imagename));//设置toolBtn图像
        toolBtn[i]->setIconSize(QPixmap(imagename).size());//设置toolBtn大小和图像一致
        toolBtn[i]->setAutoRaise(true);//设置toolBtn自动浮起界面风格
        toolBtn[i]->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);//设置toolBtn文字在图像旁边
        toolBtn[i]->setToolTip(tr("未知"));//设置toolBtn 鼠标提示为“未知”
        child[i] = new Widget(toolBtn[i]->icon(), i, toolBtn[i]->text(), this);
    }
    QGroupBox *groupBox[8];
    QVBoxLayout *layout[8];
    static int index = 0;
    for(int i=0;i<8;i++)
    {
        groupBox[i] = new QGroupBox;
        layout[i] = new QVBoxLayout(groupBox[i]);
        layout[i]->setMargin(10);//设置布局中各窗体的显示间距
        layout[i]->setAlignment(Qt::AlignHCenter);//设置布局中各窗体的位置
        for(int j=0;j<32;j++)
        {
            layout[i]->addWidget(toolBtn[index++]);//代表一个layout中加入32个toolbtn
        }
        layout[i]->addStretch();//插入一个占位符
    }
    addItem((QWidget *)groupBox[0], tr("好友"));
    addItem((QWidget *)groupBox[1], tr("陌生人"));
    addItem((QWidget *)groupBox[2], tr("家人"));
    addItem((QWidget *)groupBox[3], tr("同学"));
    addItem((QWidget *)groupBox[4], tr("同事"));
    addItem((QWidget *)groupBox[5], tr("网友"));
    addItem((QWidget *)groupBox[6], tr("驴友"));
    addItem((QWidget *)groupBox[7], tr("社区"));
}


toolbox1::toolbox1(QWidget *parent) :
    QToolBox(parent)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("system"));

    setWindowIcon(QPixmap(":/images/3.png"));//设置主窗口图标
    init_username();//--------------设置用户名------------------
    init_toolBtn();//--------------初始化主窗口控件------------------

    loginDlg login;
    login.exec();

    if(login.islogin)
    {
        if((login.userid < 0) || (login.userid > 255))
        {
            QMessageBox::information(this, tr("错误"), tr("无效用户ID"));
        }else
        {
            setWindowTitle(username[login.userid]);
            userid = login.userid;
            passwd = login.passwd;
            hostip = login.hostip;
            hostport = login.hostport;

            qDebug() << passwd << endl;

            //根据登录对话框中用户输入的服务器IP和端口号链接到服务器
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

void toolbox1::sock_Error(QAbstractSocket::SocketError sockErr)
{
    switch(sockErr)
    {
    case QAbstractSocket::RemoteHostClosedError://如果服务器正常关闭了socket，就直接break;
        break;
    default:
        QMessageBox::information(this, tr("错误"), sockClient->errorString());
    }
}

//socket接受来自server端的 消息时调用的槽函数
void toolbox1::read_Msg()
{
    struct msg_t msg;
    while(sockClient->bytesAvailable() > 0)
    {
        memset(&msg, 0, sizeof(msg));
        sockClient->read((char *)&msg, sizeof(msg));

        switch(msg.head[0])
        {
        case 0://来自server的send消息
            recv_send_Msg(msg.head[1], msg.body);
            break;
        case 1://来自server的用户状态消息
            userStatus_Msg(msg.body);
            break;
        case 2://来自server的系统消息
            system_Msg(msg.head[1]);
            break;
        default:
            ;//位置消息，忽略
            break;

        }
    }
}

void toolbox1::login_Msg()
{
    const char *pw = passwd.toStdString().data();
    qDebug() << pw << endl;
    if(sockClient->isOpen())//判断socket是否已经链接到远程服务端
    {
        if(sockClient->state() == QAbstractSocket::ConnectedState)
        {
            struct msg_t msg;
            memset(&msg, 0, sizeof(msg));
            msg.head[0] = 1;//设置消息为登录消息
            msg.head[1] = userid;
            msg.head[2] = 0;
            msg.head[3] = 0;
            strncpy(msg.body, pw, strlen(pw));
            qDebug() << pw << endl;
            sockClient->write((const char *)&msg, sizeof(msg.head) + strlen(msg.body));
        }
    }
}

void toolbox1::recv_send_Msg(int o_userid, const char *msgBody)//来自server的send消息
{
    child[o_userid]->add_msg(username[o_userid], msgBody);
    child[o_userid]->show();
}

//来自server的deliver消息
void toolbox1::userStatus_Msg(const char *msgBody)
{
    for(int i = 0; i < CLIENTCOUNT; i++)
    {
        if(msgBody[i] == '1')
        {
            toolBtn[i]->setToolTip(tr("在线"));
            child[i]->pushButton->setEnabled(true);
        }else
        {
            toolBtn[i]->setToolTip(tr("离线"));
            child[i]->pushButton->setEnabled(false);
        }
    }
}

//发往server的send消息
void toolbox1::send_Msg(int d_userid, const char *msgBody)
{
    if(sockClient->isOpen())
    {
        if(sockClient->state() == QAbstractSocket::ConnectedState)
        {
            struct msg_t msg;
            memset(&msg, 0, sizeof(msg));
            msg.head[0] = 0;//设置消息为send消息
            msg.head[1] = userid;//设置消息源用户ID
            msg.head[2] = d_userid;//设置消息目的的用户ID
            msg.head[3] = 0;//暂时保留填0
            strncpy(msg.body, msgBody, strlen(msgBody));
            sockClient->write((const char *)&msg, strlen(msg.body) + sizeof(msg.head));
        }
    }
}

void toolbox1::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton button = QMessageBox::question(this, tr("退出程序"),
                                                               QString(tr("是否退出?")),
                                                               QMessageBox::Yes | QMessageBox::No);
    if(button == QMessageBox::Yes)
    {
        event->accept();
        if(sockClient->isOpen())
            sockClient->close();
        for(int i = 0; i < CLIENTCOUNT; i++)
        {
            delete child[i];
        }
    }else{
        event->ignore();
    }
}

//来自server的系统消息
void toolbox1::system_Msg(int msgcode)
{
    switch(msgcode)
    {
    case 0:
        QMessageBox::information(this, tr("来自server的消息"), tr("无法识别的消息"));
        break;
    case 1:
        QMessageBox::information(this, tr("来自server的消息"), tr("无效userID"));
        break;
    case 2:
        QMessageBox::information(this, tr("来自server的消息"), tr("无效密码"));
        break;
    case 3:
        QMessageBox::information(this, tr("来自server的消息"), tr("userID已经登陆"));
        break;
    case 4:
        QMessageBox::information(this, tr("来自server的消息"), tr("其他"));
        break;
    default:
        QMessageBox::information(this, tr("来自server的消息"), tr("未知消息"));
    }
}

void toolbox1::socket_connected()
{
    login_Msg();
}








