#include "CWeatherForecast.h"
#include "ui_CWeatherForecast.h"

#include <QNetworkReply>
#include <QUrl>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDomDocument>
#include <QApplication>
#include <QDir>
#include <QStringListModel>

CWeatherForecast::CWeatherForecast(QWidget *parent)
    : QMainWindow(parent)
    , ui(nullptr)
    , m_networkAccMgr(nullptr)
    , m_forecastWgt(nullptr)
    , m_completer(nullptr)
    , m_pressFlag(false)
{
}

CWeatherForecast::~CWeatherForecast()
{
}

int CWeatherForecast::initialize()
{
    // 定义返回对象
    int ret = 0;

    // 初始化ui
    if(nullptr == ui)
    {
        ui = new Ui::CWeatherForecast;
        ui->setupUi(this);

        // 设置右键菜单信号发送
        this->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
        connect(this, &CWeatherForecast::customContextMenuRequested
                , this, &CWeatherForecast::on_customContextMenuRequested);

        // 链接关闭信号槽
        connect(ui->closeBtn, &QPushButton::clicked, this, &CWeatherForecast::close);
        // 链接最小化信号槽
        connect(ui->minBtn, &QPushButton::clicked, this, &CWeatherForecast::showMinimized);
        ret |= 0x1;
    }

    // 初始化网络访问对象
    if(nullptr == m_networkAccMgr)
    {
        // 创建网络访问对象空间
        m_networkAccMgr = new QNetworkAccessManager(this);
        // 连接接受网络返回的信号槽
        connect(m_networkAccMgr, SIGNAL(finished(QNetworkReply *))
                , this, SLOT(on_recvNetworkReply(QNetworkReply *)));
        ret |= 0x10;
    }

    // 初始化预报信息图表模块
    if(nullptr == m_forecastWgt)
    {
        m_forecastWgt = new CForecastWidget;
        m_forecastWgt->initialize();

        // 链接信号槽
        connect(m_forecastWgt, &CForecastWidget::forecastBtnClickedSig, [=](int index)
        {
           loadForeCastInfo(m_stWeatherInfo.stForecastInfoList.at(index));
           overloadStyle();
        });
        ret |= 0x100;

    }

    // 创建过滤器对象
    if(nullptr == m_completer)
    {
        m_completer = new QCompleter;
        ret |= 0x1000;
    }

    // 加载城市信息配置文件
    QString fileName = ".//Config//CityCodeInfo.xml";
    if(!loadConfig(fileName))
    {
        QMessageBox::information(this, "提示", fileName + ":文件加载失败");
    }

    // 加载天气预报配置文件
    fileName = ".//Config//WeatherApi.xml";
    if(!loadConfig(fileName))
    {
        QMessageBox::information(this, "提示", fileName + ":文件加载失败");
    }

    // 通过返回值对象判断所有对象是否初始化完成，然后做关联设置操作
    if(0x1111 == ret)
    {
        // 隐藏城市编辑框
        ui->cityEdit->hide();
        // 将预报天气信息控件添加到ui中
        ui->forecastLayout->addWidget(m_forecastWgt);
        ui->forecastLayout->setStretch(0, 3);
        ui->forecastLayout->setStretch(1, 5);

        // 创建提示信息列表容器，并添加城市名称信息
        QStringList listTemp = m_codeInfoMap.keys();
        // 追加城市编码信息
        listTemp.append(m_codeInfoMap.values());
        // 创建提示信息存储的数据模板，并指定父对象
        QStringListModel *model = new QStringListModel(m_completer);
        // 将提示信息设置到模板中
        model->setStringList(listTemp);
        // 将模板设置到过滤器对象中
        m_completer->setModel(model);
        // 将过滤器对象设置到城市编辑栏中
        ui->cityEdit->setCompleter(m_completer);

        // 设置省份下拉框
        ui->provinceComboBox->addItems(m_cityInfoMap.keys());

        // 设置窗口固定大小
        this->setFixedSize(770, 520);
        // 为切换按钮添加图标
        ui->switchModeBtn->setIcon(QIcon(".//StyleTheme//Base//switchMode.png"));

        // 设置隐藏默认标题
        this->setWindowFlags(Qt::FramelessWindowHint);
    }

    return ret;
}

int CWeatherForecast::unInitialize()
{
    // 定义返回对象
    int ret = 0;
    // 释放过滤器对象
    if(nullptr != m_completer)
    {
        delete m_completer;
        m_completer = nullptr;
        ret |= 0x1;
    }

    // 释放预报信息对象
    if(nullptr != m_forecastWgt)
    {
        m_forecastWgt->unInitialize();
        delete m_forecastWgt;
        m_forecastWgt = nullptr;  
        ret |= 0x10;
    }

    // 释放API访问对象
    if(nullptr != m_networkAccMgr)
    {
        delete m_networkAccMgr;
        m_networkAccMgr = nullptr;
        ret |= 0x100;
    }

    // 释放ui
    if(nullptr != ui)
    {
        delete ui;
        ui = nullptr;
        ret |= 0x1000;
    }
    return ret;
}

bool CWeatherForecast::loadWeatherForecastCfg(QString path)
{
    bool ret = false;
    do
    {
        // 指定文件并打开
        QFile file(path);
        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::information(this, "提示", path + ":文件打开失败");
            ret = false;
            break;
        }

        // 创建QDomDocument对象并设置文档类型名
        QDomDocument domDoc;
        // 设置domDoc的上下文
        if(!domDoc.setContent(&file))
        {
            // 上下文设置失败，关闭QFile对象打开的文件
            file.close();
            QMessageBox::information(this, "提示", path + ":QDomDocument对象上下文设置失败");
            ret = false;
            break;
        }
        // 上下文设置成功不再使用QFile对象打开的文件，将其关闭
        file.close();

        // 从QDomDocument对象中取到对应的顶级节点元素对象
        QDomElement apiInfo = domDoc.firstChildElement("Root");
        // 判断顶级节点元素对象是否为空
        if(apiInfo.isNull())
        {
            QMessageBox::information(this, "提示", path + ":ApiInfo节点元素对象获取失败");
            ret = false;
            break;
        }

        // 一次获取指定的apiInfo子节点，并且获取其值
        m_stApiInfo.link = apiInfo.firstChildElement("Link").attribute("Link");
        m_stApiInfo.apiKey = apiInfo.firstChildElement("ApiKey").attribute("ApiKey");
        m_stApiInfo.cityCode = apiInfo.firstChildElement("CityCode").attribute("CityCode");
        m_stApiInfo.extensions = apiInfo.firstChildElement("Extensions").attribute("Extensions");
        // 返回值变为true
        ret = true;
    }while(false);
    return ret;
}

bool CWeatherForecast::readCityInfo(const QDomElement &root)
{
    // 创建省份名对象
    QString province;
    // 创建城市名对象
    QString city;
    // 获取首个城市信息节点
    QDomElement element = root.firstChildElement("CityInfo");
    // 遍历root节点中的城市信息子节点
    while(!element.isNull())
    {
        // 获取城市名和城市编码
        QString name = element.attribute("Name");
        QString code = element.attribute("Code");
        // 判断城市名和城市编码不为空
        if(code.endsWith("0000"))
        {
            // 省份对象赋值
            province = name;
            // 将省份编码信息填入容器
            m_codeInfoMap[province] = code;
        }
        else if(code.endsWith("00"))
        {
            // 城市对象赋值
            city = name;
        }
        else if(!name.contains("市辖区"))  // 不包含“市辖区”的名才添加
        {
            // 区县值添加
            m_cityInfoMap[province][city].append(name);
            // 添加区县编码信息
            m_codeInfoMap[name] = code;
        }

        // 获取下一个信息子节点
        element = element.nextSiblingElement();
    }

    // 判断容器是否为空并返回
    return !m_cityInfoMap.isEmpty() && !m_codeInfoMap.isEmpty();
}

bool CWeatherForecast::loadConfig(QString path)
{
    bool ret = false;
    do
    {
        // 指定文件并打开
        QFile file(path);
        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::information(this, "提示", path + ":文件打开失败");
            ret = false;
            break;
        }

        // 创建QDomDocument对象并设置文档类型名
        QDomDocument domDoc;
        // 设置domDoc的上下文
        if(!domDoc.setContent(&file))
        {
            // 上下文设置失败，关闭QFile对象打开的文件
            file.close();
            QMessageBox::information(this, "提示", path + ":QDomDocument对象上下文设置失败");
            ret = false;
            break;
        }
        // 上下文设置成功不再使用QFile对象打开的文件，将其关闭
        file.close();

        // 从QDomDocument对象中取到对应的顶级节点元素对象
        QDomElement root = domDoc.firstChildElement("Root");
        // 判断顶级节点元素对象是否为空
        if(root.isNull())
        {
            QMessageBox::information(this, "提示", path + ":root节点元素对象获取失败");
            ret = false;
            break;
        }

        QString type = root.attribute("Type");

        if(0 == type.compare("CityInfo"))
        {
            // 获取返回值
            ret = readCityInfo(root);

        }
        else if(0 == type.compare("ApiInfo"))
        {
            // 获取返回值
            ret = readAPiInfo(root);
        }
    }while(false);
    return ret;
}

bool CWeatherForecast::readAPiInfo(const QDomElement &root)
{
    // 一次获取指定的apiInfo子节点，并且获取其值
    m_stApiInfo.link = root.firstChildElement("Link").attribute("Link");
    m_stApiInfo.apiKey = root.firstChildElement("ApiKey").attribute("ApiKey");
    m_stApiInfo.cityCode = root.firstChildElement("CityCode").attribute("CityCode");
    m_stApiInfo.extensions = root.firstChildElement("Extensions").attribute("Extensions");
    // 判断节点值是否存在空值
    return !m_stApiInfo.fieldIsEmpty();
}

void CWeatherForecast::overloadStyle()
{
    // 卸载程序样式
    qApp->style()->unpolish(qApp);
    // 获取当前时间
    int hour = QTime::currentTime().hour();
    // 判断是否夜晚的标记值
    bool flag = hour >= 8 && hour < 20;
    // 设置是否白日动态属性值
    ui->centralWidget->setProperty("IS_DAY", flag);
    // 重载样式
    qApp->style()->polish(qApp);

    // 获取所有控件链表，遍历重绘
    foreach(QWidget *wgt, qApp->allWidgets())
    {
        wgt->repaint();
    }
}

int CWeatherForecast::parseJson(QByteArray jsonData)
{
    // 创建返回值对象
    int ret = 0;

    //! 创建QJsonParseError对象，用于判断Json是否正确
    //! 尽管从API中返回的Json基本正确，但是流程还是要走，养成好习惯
    QJsonParseError jsonError;
    // 将json数据和jsonError对象传入fromJson函数中
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &jsonError);
    // 判断错误码是否不等于QJsonParseError::NoError，不等于则返回
    if(jsonError.error != QJsonParseError::NoError)
    {
        return ret;
    }

    // 创建QJsonObject对象接收object的返回值
    QJsonObject jsonObj = jsonDoc.object();

    //! 判断接收的Json串是否正确
    //! status为1那么状态为成功，infocode为10000则返回状态为正确
    if("1" != jsonObj.value("status").toString() || "10000" != jsonObj.value("infocode").toString())
    {
        QMessageBox::warning(nullptr, "警告", "数据返回失败/错误");
        return ret;
    }

    // 获取当前模式按钮的选中状态
    bool isChecked = ui->switchModeBtn->isChecked();
    // 通过模式切换按钮判断获取当前模式
    QString infoType = isChecked ? "forecasts" : "lives";
    //! 通过infoType获取对应的内容
    //! 因为infoType下的标识符为‘[’所以先转换为数组
    //! 然后取数组下第一个元素并将其转为QJsonObject对象
    QJsonObject weatherInfo = jsonObj.value(infoType).toArray().at(0).toObject();
    // 读取基础信息（省份、城市、区域编码、报告时间）
    m_stWeatherInfo.setBaseInfo(weatherInfo.value("province").toString()
                                , weatherInfo.value("city").toString()
                                , weatherInfo.value("adcode").toString()
                                , weatherInfo.value("reporttime").toString());

    // 通过按钮选中状态使用不同模式的转换函数
    if(isChecked)
    {
        // 预报天气转换
        parseForecastJson(weatherInfo);
    }
    else
    {
        // 实时天气转换
        parseLiveJson(weatherInfo);
    }

    return ret;
}

void CWeatherForecast::parseLiveJson(QJsonObject weatherInfo)
{
    // 获取天气
    QString weather = weatherInfo.value("weather").toString();
    // 获取温度
    QString temperature = weatherInfo.value("temperature").toString();
    // 获取风向
    QString winddirection = weatherInfo.value("winddirection").toString();
    // 获取风力
    QString windpower = weatherInfo.value("windpower").toString();
    // 获取湿度
    QString humidity = weatherInfo.value("humidity").toString();
    // 设置信息
    m_stWeatherInfo.stliveInfo.setWeatherInfo(weather, temperature
                                              , winddirection, windpower, humidity);
    if(m_stWeatherInfo.stliveInfo.fieldIsEmpty())
    {
        QMessageBox::information(this, "提示", "没有新的天气信息");
        return;
    }
    loadUiInfo(false);
}

void CWeatherForecast::parseForecastJson(QJsonObject weatherInfo)
{
    // 每次添加将上次的预报天气信息清空
    m_stWeatherInfo.stForecastInfoList.clear();

    // 获取cast的内容，并将其转为数组
    QJsonArray forecastArray = weatherInfo.value("casts").toArray();
    // 获取json数组对象的迭代器
    QJsonArray::const_iterator iterator = forecastArray.begin();

    // 判断获取的起始迭代器不等于结束迭代器
    if(forecastArray.end() == iterator)
    {
        QMessageBox::information(this, "提示", "没有新的天气信息");
        return;
    }

    // 遍历天气信息
    for(; iterator != forecastArray.end(); ++iterator)
    {
        // 先追加一个预报结构体
        m_stWeatherInfo.stForecastInfoList.append(stForecastsInfo());
        // 获取最新追加结构体的引用对象
        stForecastsInfo &forecastsInfo = m_stWeatherInfo.stForecastInfoList.last();

        // 获取当前内容的QJsonObject对象
        QJsonObject forecastsInfoObj = (*iterator).toObject();
        // 设置日期
        forecastsInfo.setDate(forecastsInfoObj.value("date").toString());
        // 设置天气
        forecastsInfo.setWeather(forecastsInfoObj.value("dayweather").toString()
                                 , forecastsInfoObj.value("nightweather").toString());
        // 设置温度
        forecastsInfo.setTemperature(forecastsInfoObj.value("daytemp").toString()
                                 , forecastsInfoObj.value("nighttemp").toString());
        // 设置风向
        forecastsInfo.setWindDirection(forecastsInfoObj.value("daywind").toString()
                                 , forecastsInfoObj.value("nightwind").toString());
        // 设置风力
        forecastsInfo.setWindPower(forecastsInfoObj.value("daypower").toString()
                                 , forecastsInfoObj.value("nightpower").toString());
    }

    //加载ui信息
    loadUiInfo(true);
}

void CWeatherForecast::loadUiInfo(bool isChecked)
{
    // 设置通用属性
    ui->reportTimeLab->setText(m_stWeatherInfo.reporttime);
    // 城市字符串 省份+城市
    QString cityStr = m_stWeatherInfo.province + "-" + m_stWeatherInfo.city;
    // 设置预报天气信息
    if(isChecked)
    {
        // 设置预报天气城市
        ui->forecastWeatherCityLab->setText(cityStr);
        // 更新预报图表模块信息
        m_forecastWgt->updateWeatherInfo(m_stWeatherInfo.stForecastInfoList);
        // 加载当前显示的天气信息
        loadForeCastInfo(m_stWeatherInfo.stForecastInfoList.first());
    }
    // 设置实时天气信息
    else
    {
        // 设置实时天气城市
        ui->liveWeatherCityLab->setText(cityStr);
        // 获取实时信息的引用
        const stLiveInfo &info = m_stWeatherInfo.stliveInfo;
        // 设置实时信息
        ui->liveTempLab->setText(info.temperature + "°C");
        ui->liveWeatherLab->setText(info.weather);
        ui->liveHumidityLab->setText(info.humidity);
        ui->liveLindPowerLab->setText(info.windpower);
        ui->liveWindDirectionLab->setText(info.winddirection);

        // 添加天气动态属性以及提示文本
        ui->liveWeatherImgLab->setProperty("WEATHER", info.weather);
        ui->liveWeatherImgLab->setToolTip(info.weather);
    }

    // 重载样式
    overloadStyle();
}

void CWeatherForecast::loadForeCastInfo(const stForecastsInfo &info)
{
    // 将ui上对应的信息设置
    ui->forecastHighTempLab->setText(info.daytemp + "°C");
    ui->forecastLowTempLab->setText(info.nighttemp + "°C");
    ui->forecastWindPowerLab->setText(info.daypower);
    ui->forecastWindDirectionLab->setText(info.daywind);
    ui->forecastDateLab->setText(info.date + " " +info.week);

    // 添加天气动态属性以及提示文本
    ui->forecastWeatherImgLab->setProperty("WEATHER", info.dayweather);
    ui->forecastWeatherImgLab->setToolTip(info.dayweather);
}

bool CWeatherForecast::judgeCityEditText(QString &text)
{
    // 创建返回值对象
    bool ret = false;
    // 获取城市文本
    QString cityCode = ui->cityEdit->isHidden() ? ui->countyComboBox->currentText() : ui->cityEdit->text();

    // 判断文本是否为空
    if(cityCode.isEmpty())
    {
        QMessageBox::information(this, "提示", "请输入城市名称或城市编码");
    }
    else
    {
        // 判断城市名称中是否包含文本
        if(m_codeInfoMap.keys().contains(cityCode))
        {
            // 获取对应城市名的城市编码并赋值
            text = m_codeInfoMap[cityCode];
            ret = true;
        }
        // 获取城市编码是否包含文本框文本
        else if(m_codeInfoMap.values().contains(cityCode))
        {
            // 城市编码正确并赋值
            text = cityCode;
            ret = true;
        }
        else
        {
            QMessageBox::information(this, "提示", "请输入正确的城市名称或城市编码");
        }
    }

    return ret;
}

void CWeatherForecast::on_recvNetworkReply(QNetworkReply *reply)
{
    // 判断错误码是否为QNetworkReply::NoError（若判断条件成立，则reply对象数据错误）
    if(reply->error() != QNetworkReply::NoError)
    {
        // 弹出警告
        QMessageBox::warning(nullptr, "警告", "数据返回错误");
    }
    else
    {
        // 创建array容器接收数据
        QByteArray data;
        // 读取所有json串
        data = reply->readAll();
        // 解析json串
        parseJson(data);

    }
    // 释放内存，防止内存泄漏
    delete reply;
    reply = nullptr;
}

void CWeatherForecast::sendWeatherInfoRequest()
{
    if(judgeCityEditText(m_stApiInfo.cityCode))
    {
        // 调用组合API链接函数申请API
        QUrl url(m_stApiInfo.joinApiLink());
        // 获取天气预报回执；此处返回的QNetworkReply对象将在on_recvNetworkReply槽函数中体现，可不在此接收释放。
        m_networkAccMgr->get(QNetworkRequest(url));
    }
}

void CWeatherForecast::responseMenuAction(QAction *action)
{
    // 当参数值为空时返回
    if(nullptr == action)
    {
        return;
    }

    QString text = action->text();
    if("刷新" == text)
    {
        // 发送天气信息请求
        sendWeatherInfoRequest();
    }
    else
    {
        // 获取城市编辑栏当前显示状态
        bool isHidden = ui->cityEdit->isHidden();
        // 将城市编辑栏显示状态取反设置
        ui->cityEdit->setHidden(!isHidden);
        // 设置城市选择模式显隐状态
        ui->selectModeWgt->setHidden(isHidden);
        if(isHidden)
        {
            // 获取选择模式下当前城市名
            QString text = ui->countyComboBox->currentText();
            // 将当前城市名设置到编辑栏中
            ui->cityEdit->setText(text);
        }
        else
        {
            // 获取当前区县编码值
            QString countyCode = m_stApiInfo.cityCode;
            // 通过当前区县编码值获取当前区县名
            QString countyName = m_codeInfoMap.key(m_stApiInfo.cityCode);
            bool contain = m_codeInfoMap.values().contains(countyCode);

            // 通过当前区县编码值获取当前城市编码值
            QString cityCode = countyCode.replace(countyCode.size() - 2, 2, "00");
            contain = m_codeInfoMap.values().contains(cityCode);

            // 通过省份编码值获取省份名
            QString cityName = m_codeInfoMap.key(cityCode);
            // 包含市辖区时移除市辖区文本
            cityName = cityName.replace("市辖区", "");

            // 通过当前区县编码值获取当前省份编码值
            QString provinceCode = countyCode.replace(countyCode.size() - 4, 4, "0000");
            contain = m_codeInfoMap.values().contains(provinceCode);
            // 通过省份编码值获取省份名
            QString provinceName = m_codeInfoMap.key(provinceCode);

            // 然后将获取到的三个值更新到下拉框中
            // 设置省份
            ui->provinceComboBox->setCurrentText(provinceName);
            // 设置城市
            ui->cityComboBox->setCurrentText(cityName);
            // 设置区县
            ui->countyComboBox->setCurrentText(countyName);
        }
    }
}

void CWeatherForecast::on_switchModeBtn_clicked(bool checked)
{
    // 判断选中状态，获取将要设置对应的文本
    QString text = checked ? "预报天气" : "实时天气";
    // 设置当前显示的文本
    ui->switchModeBtn->setText(text);

    // 设置API扩展参数变量的新值
    m_stApiInfo.extensions = checked ? "all" : "base";

    // 设置当前栈模式对应的窗口
    ui->stackedWidget->setCurrentIndex(checked ? 1 : 0);

    // 发送天气信息请求
    sendWeatherInfoRequest();
}

void CWeatherForecast::on_forecastBtnClicked(int index)
{
    if(index < m_stWeatherInfo.stForecastInfoList.size())
    {
        loadForeCastInfo(m_stWeatherInfo.stForecastInfoList.at(index));
    }
}

void CWeatherForecast::on_provinceComboBox_currentIndexChanged(const QString &arg1)
{
    //! 当省份变化时，城市下拉列表框将要清空
    ui->cityComboBox->clear();
    //! 然后添加对应的城市列表
    ui->cityComboBox->addItems(m_cityInfoMap[arg1].keys());
}

void CWeatherForecast::on_cityComboBox_currentIndexChanged(const QString &arg1)
{
    // 当当前文本内容为空时返回
    if(arg1.isEmpty())
    {
        return;
    }
    //! 当城市变化时，区/县下拉列表框将要清空
    ui->countyComboBox->clear();
    //! 然后添加对应的区县列表
    ui->countyComboBox->addItems(m_cityInfoMap[ui->provinceComboBox->currentText()][arg1]);
}

void CWeatherForecast::on_countyComboBox_currentIndexChanged(const QString &arg1)
{
    // 当当前文本内容为空时返回
    if(arg1.isEmpty())
    {
        return;
    }
    // 区县改变时将要
    m_stApiInfo.cityCode = m_codeInfoMap[arg1];
    // 发送天气信息请求
    sendWeatherInfoRequest();
}

void CWeatherForecast::on_customContextMenuRequested()
{
    // 创建菜单变量
    QMenu menu;
    // 添加菜单选项
    menu.addAction(new QAction("刷新", &menu));
    menu.addAction(new QAction("切换城市选择模式", &menu));
    // 显示菜单，并且显示位置为鼠标位置，并接收返回的对象
    QAction *action = menu.exec(QCursor::pos());
    responseMenuAction(action);
}

void CWeatherForecast::on_cityEdit_textChanged(const QString &arg1)
{
    // 当城市编码容器包含当前文本Key值则进入
    if(m_codeInfoMap.contains(arg1) || m_codeInfoMap.values().contains(arg1))
    {
        // 发送天气信息请求
        sendWeatherInfoRequest();
    }
}

void CWeatherForecast::mousePressEvent(QMouseEvent *event)
{
    // 当标题栏包含鼠标位置，并且鼠标为左键按下时进入
    if(ui->titleWgt->rect().contains(event->pos()) && Qt::LeftButton == event->button())
    {
        m_pressFlag = true; // 鼠标按下变量值更为true
        m_startPos = event->globalPos();    // 获取鼠标的全局位置
    }
}

void CWeatherForecast::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_pressFlag = false;    // 鼠标释放时标记值更为false
}

void CWeatherForecast::mouseMoveEvent(QMouseEvent *event)
{
    // 当标记值为true时进入
    if(m_pressFlag)
    {
        // 当前窗口移动，当前位置 + 最新鼠标全局位置 - 上次鼠标全局位置（此处为了计算出偏移量）
        this->move(this->pos() + event->globalPos() - m_startPos);
        m_startPos = event->globalPos();    // 更新鼠标全局位置
    }
}
