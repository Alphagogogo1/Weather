#ifndef CFORECASTWIDGET_H
#define CFORECASTWIDGET_H

#include "StWeatherForecast.h"
#include <QObject>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>

QT_CHARTS_USE_NAMESPACE // 使用chart的命名空间

namespace Ui {
class CForecastWidget;
}


class CForecastWidget : public QObject
{
    Q_OBJECT
public:
    explicit CForecastWidget(QObject *parent = nullptr);
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

    void updateWeatherInfo(const QList<stForecastsInfo> &infoList);

    void setCurWeatherInfo(const stForecastsInfo &info);

    void releaseBtn();

signals:

public slots:

private:
    Ui::CForecastWidget     *ui;

    QChart                  *m_lineChart;   // 折线图的chart

    QLineSeries             *m_daySeries;   // 日间温度序列

    QLineSeries             *m_nightSeries; // 夜间温度序列

};

#endif // CFORECASTWIDGET_H
