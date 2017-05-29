#ifndef TOOLBOX1_H
#define TOOLBOX1_H

#include <QToolBox>
#include <QToolButton>
#include <QEvent>
 #include <QTcpSocket>

#include "widget.h"

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

private slots:
    void sock_Error(QAbstractSocket::SocketError sockErr);//socket出错的时候触发的槽函数
    void read_Msg();//socket接收到消息触发的槽函数
    void socket_connected();//socket成功连接


};

#endif // TOOLBOX1_H
