#ifndef CFORECASTWIDGET_H
#define CFORECASTWIDGET_H

#include "StWeatherForecast.h"
#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QValueAxis>
#include <QDateTimeAxis>

QT_CHARTS_USE_NAMESPACE // 使用chart的命名空间

namespace Ui {
class CForecastWidget;
}


class CForecastWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CForecastWidget(QWidget *parent = nullptr);
    ~CForecastWidget();

    /**
     * @brief initialize 初始化类（使构造函数不做过多的操作）
     * @return 初始化标志值（通过该值区分是否初始化成功或初始化到哪一步）
     */
    int initialize();

    /**
     * @brief unInitialize 初始化类（使析构函数不做过多的操作）
     * @return 反初始化标志值（通过该值区分是否释放成功或释放到哪一步）
     */
    int unInitialize();

    /**
     * @brief updateWeatherInfo 更新按钮和图表信息
     * @param infoList 预报天气结构体联邦
     */
    void updateWeatherInfo(const QList<stForecastsInfo> &infoList);

    /**
     * @brief releaseBtn 释放自定义按钮对象
     */
    void releaseBtn();

signals:
    /**
     * @brief forecastBtnClickedSig 转发按钮点击信号
     * @param index 点击按钮索引
     */
    void forecastBtnClickedSig(int index);

public slots:

private:
    Ui::CForecastWidget     *ui;

    QChart                  *m_lineChart;   // 折线图的chart

    QLineSeries             *m_daySeries;   // 日间温度序列

    QLineSeries             *m_nightSeries; // 夜间温度序列

    QDateTimeAxis           *m_dateXAxis;   // 折线图的X轴

    QValueAxis              *m_valueYAxis;  // Y轴对象

};

#endif // CFORECASTWIDGET_H
