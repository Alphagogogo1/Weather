#include "CForecastWidget1.h"
#include "ui_CForecastWidget.h"
#include "CForecastBtn.h"

CForecastWidget::CForecastWidget(QObject *parent)
    : QObject(parent)
    , ui(nullptr)
    , m_lineChart(nullptr)
    , m_daySeries(nullptr)
    , m_nightSeries(nullptr)
{

}

CForecastWidget::~CForecastWidget()
{

}

int CForecastWidget::initialize()
{
    // 返回值对象
    int ret = 0;

    if(nullptr == ui)
    {
        ui = new Ui::CForecastWidget;
        ui->setupUi(this);
    }

    // 创建qchart对象
    if(nullptr == m_lineChart)
    {
        m_lineChart = new QChart;
        ret = 1;
    }

    // 创建日间序列对象
    if(nullptr == m_daySeries)
    {
        m_daySeries = new QLineSeries;
        ret = 2;
    }

    // 创建夜间序列对象
    if(nullptr == m_nightSeries)
    {
        m_nightSeries = new QLineSeries;
        ret = 3;
    }

    // 将序列对象添加到chart对象中并将chart对象添加到ui中的GraphiView对象中
    m_lineChart->addSeries(m_daySeries);
    m_lineChart->addSeries(m_nightSeries);
//    m_lineChart->axisX()->setRange(  )
    return ret;
}

int CForecastWidget::unInitialize()
{
    // 返回值对象
    int ret = 0;

    // 释放夜间序列对象
    if(nullptr != m_nightSeries)
    {
        delete m_nightSeries;
        m_nightSeries = nullptr;
        ret = 1;
    }

    // 释放日间序列对象
    if(nullptr != m_daySeries)
    {
        delete m_daySeries;
        m_daySeries = nullptr;
        ret = 2;
    }

    // 释放qchart对象
    if(nullptr != m_lineChart)
    {
        delete m_lineChart;
        m_lineChart = nullptr;
        ret = 3;
    }

    return ret;
}

void CForecastWidget::updateWeatherInfo(const QList<stForecastsInfo> &infoList)
{
    // 清空日间、夜间序列内容
    m_daySeries->clear();
    m_nightSeries->clear();

    // 遍历预报天气结构体信息容器
    foreach(stForecastsInfo info, infoList)
    {

    }
}

void CForecastWidget::setCurWeatherInfo(const stForecastsInfo &info)
{

}

void CForecastWidget::releaseBtn()
{

}
