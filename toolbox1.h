#ifndef TOOLBOX1_H
#define TOOLBOX1_H

#include <QToolBox>
#include <QToolButton>
#include <QEvent>
 #include <QTcpSocket>

#include "widget.h"
#include <QCloseEvent>

#define CLIENTCOUNT 256
#define MSGBODYBUF 1024

//定义消息结构体
struct msg_t
{
    unsigned char head[4];
    char body[MSGBODYBUF];
};


class toolbox1 : public QToolBox
{
    Q_OBJECT
public:
    explicit toolbox1(QWidget *parent = 0);
    void send_Msg(int d_userid, const char *msgBody);

signals:

public slots:
    bool eventFilter(QObject *watched, QEvent *event);


private:
    int userid;
    QString passwd;
    QString hostip;
    int hostport;

    QString username[CLIENTCOUNT];
    QToolButton *toolBtn[CLIENTCOUNT];
    Widget *child[CLIENTCOUNT];
    void init_toolBtn();
    void init_username();

    QTcpSocket *sockClient;
    void login_Msg();
    void recv_send_Msg(int o_userid, const char *msgBody);
    void userStatus_Msg(const char *msgBody);
    void system_Msg(int msgcode);

    void closeEvent(QCloseEvent *event);

private slots:
    void sock_Error(QAbstractSocket::SocketError sockErr);//socket出错的时候触发的槽函数
    void read_Msg();//socket接收到消息触发的槽函数
    void socket_connected();//socket成功连接


};

#endif // TOOLBOX1_H
