#include "widget.h"
#include <QTextCodec>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("system"));
}

Widget::~Widget()
{

}
