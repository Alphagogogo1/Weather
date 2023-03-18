#include "CForecastBtn.h"
#include "ui_CForecastBtn.h"

#include <QMouseEvent>

CForecastBtn::CForecastBtn(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CForecastBtn)
{
    ui->setupUi(this);

}

CForecastBtn::~CForecastBtn()
{
    delete ui;
}

void CForecastBtn::setBtnInfo(QString week, QString weather, QString dayTemp, QString nightTemp)
{
    // 设置周标签
    ui->weekLab->setText(week);
    //! 设置天气属性，不显示天气文本显示天气属性，且设置悬浮文本
    //    ui->weatherLab->setText(weather);
    ui->weatherLab->setProperty("WEATHER", weather);
    ui->weatherLab->setToolTip(weather);
    // 设置温度标签
    ui->tempLab->setText(nightTemp + "°~" + dayTemp + "°");
}

void CForecastBtn::mouseReleaseEvent(QMouseEvent *event)
{
    // 当前为左键按钮时进入
    if(Qt::LeftButton == event->button())
    {
        // 发出点击信号
        emit clicked();
    }
}
