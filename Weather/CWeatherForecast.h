#ifndef CWEATHERFORECAST_H
#define CWEATHERFORECAST_H

#include "StWeatherForecast.h"
#include "CForecastWidget.h"

#include <QMainWindow>
#include <QDomElement>
#include <QNetworkAccessManager>    // 导入请求/接受天气查询API的类
#include <QCompleter>
#include <QMessageBox>

namespace Ui {
class CWeatherForecast;
}

class CWeatherForecast : public QMainWindow
{
    Q_OBJECT

public:
    explicit CWeatherForecast(QWidget *parent = nullptr);
    ~CWeatherForecast();

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
     * @brief loadConfig 加载配置文件
     * @param path 指定文件路径
     * @return 是否加载成功
     */
    bool loadConfig(QString path);

    /**
     * @brief loadWeatherForecastCfg 加载天气预报配置文件
     * @param path 文件路径
     * @return 是否加载成功
     */
    bool loadWeatherForecastCfg(QString path);

    /**
     * @brief readCityInfo 读取城市信息
     * @param root root节点
     * @return 是否读取成功
     */
    bool readCityInfo(const QDomElement &root);

    /**
     * @brief readAPiInfo 读取API信息
     * @param root root节点
     * @return 是否读取成功
     */
    bool readAPiInfo(const QDomElement &root);

    /**
     * @brief overloadStyle 样式重载函数
     */
    void overloadStyle();

private:
    /**
     * @brief parseJson json语句转换槽函数
     * @param jsonArray json语句容器
     */
    int parseJson(QByteArray jsonData);

    /**
     * @brief parseLiveJson 解析实时天气信息
     * @param weatherInfo 实时天气信息对象
     */
    void parseLiveJson(QJsonObject weatherInfo);

    /**
     * @brief parseForecastJson 解析预报天气信息
     * @param weatherInfo 预报天气信息对象
     */
    void parseForecastJson(QJsonObject weatherInfo);

    /**
     * @brief loadUiInfo 根据标记值加载ui
     * @param isChecked 标记值
     */
    void loadUiInfo(bool isChecked);

    /**
     * @brief loadForeCastInfo 加载指定预报信息结构体的内容
     * @param info 信息结构体
     */
    void loadForeCastInfo(const stForecastsInfo &info);

    /**
     * @brief judgeCityEditText 判断城市编辑栏的文本信息
     * @param text 存放文本框信息对应的城市编码
     * @return 文本是否正确, 若是更新文本框信息不正确则不修改编码存储容器传入时的值
     */
    bool judgeCityEditText(QString &text);

    /**
     * @brief sendWeatherRequest 发送天气信息请求函数
     */
    void sendWeatherInfoRequest();

    /**
     * @brief responseMenuAction 响应右键菜单
     * @param action 右键菜单点击对象
     */
    void responseMenuAction(QAction *action);

private slots:
    /**
     * @brief on_recvNetworkReply 接收API返回数据对象槽函数
     * @param reply 包含API数据的对象
     */
    void on_recvNetworkReply(QNetworkReply *reply);


    /**
     * @brief on_switchModeBtn_clicked 模式切换按钮槽函数
     * @param checked 当前按钮选中状态
     */
    void on_switchModeBtn_clicked(bool checked);

    /**
     * @brief on_forecastBtnClicked 预报天气界面按钮点击槽函数
     * @param index 点击按钮的索引
     */
    void on_forecastBtnClicked(int index);

    /**
     * @brief on_provinceComboBox_currentIndexChanged 省份下拉列表框改变槽函数
     * @param arg1 改变的文本
     */
    void on_provinceComboBox_currentIndexChanged(const QString &arg1);

    /**
     * @brief on_cityComboBox_currentIndexChanged 城市下拉列表框改变槽函数
     * @param arg1 改变的文本
     */
    void on_cityComboBox_currentIndexChanged(const QString &arg1);

    /**
     * @brief on_countyComboBox_currentIndexChanged 区县下拉列表框改变槽函数
     * @param arg1 改变的文本
     */
    void on_countyComboBox_currentIndexChanged(const QString &arg1);

    /**
     * @brief on_customContextMenuRequested 定制菜单信号槽函数
     */
    void on_customContextMenuRequested();

    /**
     * @brief on_cityEdit_textChanged 城市编辑框文本改变槽函数
     * @param arg1 改变的文本
     */
    void on_cityEdit_textChanged(const QString &arg1);

    // QWidget interface
protected:
    /**
     * @brief mousePressEvent 鼠标按下事件
     * @param event 鼠标事件对象
     */
    void mousePressEvent(QMouseEvent *event);

    /**
     * @brief mouseReleaseEvent 鼠标释放事件
     * @param event 鼠标事件对象
     */
    void mouseReleaseEvent(QMouseEvent *event);

    /**
     * @brief mouseMoveEvent 鼠标移动事件
     * @param event 鼠标事件对象
     */
    void mouseMoveEvent(QMouseEvent *event);

private:
    Ui::CWeatherForecast                        *ui;

    QNetworkAccessManager                       *m_networkAccMgr;       // 访问天气查询API的网络对象

    stApiInfo                                   m_stApiInfo;            // api信息结构体对象

    stWeatherInfo                               m_stWeatherInfo;        // 天气信息结构体

    CForecastWidget                             *m_forecastWgt;         // 预报天气信息图表显示板块

    QMap<QString, QMap<QString, QStringList>>   m_cityInfoMap;          // 城市信息容器 <省份，<城市，区县>>

    QMap<QString, QString>                      m_codeInfoMap;          // 编码信息容器 <城市，编码>

    QCompleter                                  *m_completer;           // 过滤器对象

    bool                                        m_pressFlag;            // 鼠标按下标记

    QPoint                                      m_startPos;             // 鼠标按下位置


};

#endif // CWEATHERFORECAST_H
