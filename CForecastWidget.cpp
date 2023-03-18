#include "CForecastWidget.h"
#include "ui_CForecastWidget.h"
#include "CForecastBtn.h"

#include <QtCharts/QChartView>

CForecastWidget::CForecastWidget(QWidget *parent)
    : QWidget(parent)
    , ui(nullptr)
    , m_lineChart(nullptr)
    , m_daySeries(nullptr)
    , m_nightSeries(nullptr)
    , m_dateXAxis(nullptr)
    , m_valueYAxis(nullptr)
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
        ret |= 0x1;
    }

    // 创建qchart对象
    if(nullptr == m_lineChart)
    {
        // 创建图表对象
        m_lineChart = new QChart;
        // 将图例对象放到图表的左边
        m_lineChart->legend()->setAlignment(Qt::AlignLeft);
        // 创建默认坐标轴
        m_lineChart->createDefaultAxes();
        ret |= 0x10;
    }

    // 创建日间序列对象
    if(nullptr == m_daySeries)
    {
        m_daySeries = new QLineSeries;
        // 设置序列名
        m_daySeries->setName("日间温度");
        ret |= 0x100;
    }

    // 创建夜间序列对象
    if(nullptr == m_nightSeries)
    {
        m_nightSeries = new QLineSeries;
        // 设置序列名
        m_nightSeries->setName("夜间温度");
        ret |= 0x1000;
    }

    // 创建时间（X）轴对象
    if(nullptr == m_dateXAxis)
    {
        m_dateXAxis = new QDateTimeAxis;
        // 设置坐标轴标签显示格式
        m_dateXAxis->setFormat("yyyy/MM/dd");
        ret |= 0x10000;
    }

    // 创建Y轴对象
    if(nullptr == m_valueYAxis)
    {
        m_valueYAxis = new QValueAxis;
        ret |= 0x100000;
    }

    // 通过返回值对象判断所有对象是否初始化完成，然后做关联设置操作
    if(0x111111 == ret)
    {
        // 将序列对象添加到chart对象中并将chart对象添加到ui中的GraphiView对象中
        m_lineChart->addSeries(m_daySeries);
        m_lineChart->addSeries(m_nightSeries);
        // 添加X、Y轴对象到图表中
        m_lineChart->addAxis(m_dateXAxis, Qt::AlignBottom);
        m_lineChart->addAxis(m_valueYAxis, Qt::AlignLeft);

        //! 序列对象关联X、Y坐标轴
        // 关联X轴
        m_daySeries->attachAxis(m_dateXAxis);
        m_nightSeries->attachAxis(m_dateXAxis);
        // 关联Y轴
        m_daySeries->attachAxis(m_valueYAxis);
        m_nightSeries->attachAxis(m_valueYAxis);

        // 将图表对象添加到ui中
        ui->weatherChartView->setChart(m_lineChart);
    }
    return ret;
}

int CForecastWidget::unInitialize()
{
    // 返回值对象
    int ret = 0;

    // 释放自定义按钮对象
    releaseBtn();

    // 释放Y轴
    if(nullptr != m_valueYAxis)
    {
        delete m_valueYAxis;
        m_valueYAxis = nullptr;
        ret |= 0x1;
    }

    // 释放X轴
    if(nullptr != m_dateXAxis)
    {
        delete m_dateXAxis;
        m_dateXAxis = nullptr;
        ret |= 0x10;
    }

    // 释放夜间序列对象
    if(nullptr != m_nightSeries)
    {
        delete m_nightSeries;
        m_nightSeries = nullptr;
        ret |= 0x100;
    }

    // 释放日间序列对象
    if(nullptr != m_daySeries)
    {
        delete m_daySeries;
        m_daySeries = nullptr;
        ret |= 0x1000;
    }

    // 释放qchart对象
    if(nullptr != m_lineChart)
    {
        delete m_lineChart;
        m_lineChart = nullptr;
        ret |= 0x10000;
    }

    // 释放Ui对象
    if(nullptr != ui)
    {
        delete  ui;
        ui = nullptr;
        ret |= 0x100000;
    }

    return ret;
}

void CForecastWidget::updateWeatherInfo(const QList<stForecastsInfo> &infoList)
{
    // 释放前一次设置的按钮对象
    releaseBtn();
    // 清空日间、夜间序列内容
    m_daySeries->clear();
    m_nightSeries->clear();

    // 获取温度最值，方便设置图表Y轴的最值
    int maxTemp = infoList.first().daytemp.toInt(); // 从日间温度取最大值
    int minTemp = infoList.first().nighttemp.toInt(); //从夜间温度取最小值

    // 设置时间坐标轴日期范围
    m_dateXAxis->setRange(QDateTime::fromString(infoList.first().date, "yyyy-MM-d")
                          , QDateTime::fromString(infoList.last().date, "yyyy-MM-dd"));

    // 遍历预报天气结构体信息容器
    for(int index = 0; index != infoList.size(); ++index)
    {
        // 获取当前日期信息对象
        const stForecastsInfo &info = infoList.at(index);
        // 创建按钮对象
        CForecastBtn *btn = new CForecastBtn;
        // 将按钮添加到布局器中
        ui->forecastBtnLayout->addWidget(btn);
        // 设置按钮对按钮对象的显示信息
        btn->setBtnInfo(info.week, info.dayweather, info.daytemp, info.nighttemp);
        // 连接按钮信号信号槽
        connect(btn, &CForecastBtn::clicked, [=]()
        {
            // 转发点击信号
            emit forecastBtnClickedSig(index);
        });
        // 获取日/夜间温度
        int dayTemp = info.daytemp.toInt();
        int nightTemp = info.nighttemp.toInt();
        //! 时间轴对象需要将时间转换为毫秒来定位索引
        qint64 timeIndex = QDateTime::fromString(infoList[index].date, "yyyy-MM-d").toMSecsSinceEpoch();
        // 将日/夜间温度追加到对应的序列对象中
        m_daySeries->append(timeIndex, dayTemp);
        m_nightSeries->append(timeIndex, nightTemp);

        // 通过三目运算符比较当前日/夜间温度，并取出较大/小值
        maxTemp = dayTemp > maxTemp ? dayTemp : maxTemp;
        minTemp = nightTemp < minTemp ? nightTemp : minTemp;
    }

    // 设置Y轴的值范围, 并在最大/小值加/减2使图像不那么靠近边界
    m_valueYAxis->setRange(minTemp - 2, maxTemp + 2);
}

void CForecastWidget::releaseBtn()
{
    // 定义布局器item对象
    QLayoutItem *child;
    // 循环获取布局器item对象，并判其不为空
    while(nullptr != (child = ui->forecastBtnLayout->takeAt(0)))
    {
        // 获取item对象中的widget
        QWidget *wgt = child->widget();
        // 判断widget是否为空
        if(nullptr != wgt)
        {
            // 释放widget空间
            delete wgt;
        }
        // 释放widget空间
        delete child;
    }
}
