#ifndef TOOLBOX1_H
#define TOOLBOX1_H

#include <QToolBox>
#include <QToolButton>
#include <QEvent>

#include "widget.h"

#define CLIENTCOUNT 256


class toolbox1 : public QToolBox
{
    Q_OBJECT
public:
    explicit toolbox1(QWidget *parent = 0);

signals:

public slots:
    //bool eventFilter(QObject *watched, QEvent *event);


private:
    QString username[CLIENTCOUNT];
    QToolButton *toolBtn[CLIENTCOUNT];
    Widget *child[CLIENTCOUNT];
    void init_toolBtn();
    void init_username();


};

#endif // TOOLBOX1_H
