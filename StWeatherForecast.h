#ifndef STWEATHERFROECAST_H
#define STWEATHERFROECAST_H

#include <QString>
#include <QHash>
#include <QDate>

// 定义api信息结构体
typedef struct stApiInfo
{
    QString link;   // 链接
    QString apiKey; // API Key
    QString cityCode;   // 城市编码
    QString extensions; // 天气查看类型

    // api信息结构体初始化
    stApiInfo()
    {
        link = "http://restapi.amap.com/v3/weather/weatherInfo?";   // 链接
        apiKey = "8b7fa0f3304b71426c27eaaaa15622d5";    // apiKey
        cityCode = "110000";    // 城市码
        extensions = "base";    // 天气查看类型（实时、预报）
    }

    /**
     * @brief joinApiLink 拼接API链接
     * @return 返回拼接好的API链接
     */
    QString joinApiLink()
    {
        QString ret = "";
        // 拼接API_KEY
        ret = link + "key=" + apiKey;
        ret += "&city=" + cityCode;
        ret += "&extensions=" + extensions;
        return ret;
    }

    /**
     * @brief fieldIsEmpty 判断是否有字段值为空
     * @return 是否为空
     */
    bool fieldIsEmpty()
    {
        return link.isEmpty() || apiKey.isEmpty() || cityCode.isEmpty() || extensions.isEmpty();
    }
}stApiInfo;

// 实时天气信息结构体
typedef struct stLiveInfo
{
    QString weather;        // 天气
    QString temperature;    // 温度
    QString winddirection;  // 风向
    QString windpower;      // 风力等级
    QString humidity;       // 湿度

    // 无参构造
    stLiveInfo()
    {
    }

    // 使用构造函数使其将数据传入，通过初始化列表赋值
    stLiveInfo(const QString &weather, const QString &temperature
               , const QString &winddirection, const QString &windpower, const QString &humidity)
        : weather(weather)
        , temperature(temperature)
        , winddirection(winddirection)
        , windpower(windpower)
        , humidity(humidity)
    {
    }

    /**
     * @brief setWeatherInfo 设置天气信息
     * @param weather 天气
     * @param temperature 温度
     * @param winddirection 风向
     * @param windpower 风力等级
     * @param humidity 湿度
     */
    void setWeatherInfo(const QString &weather, const QString &temperature
                   , const QString &winddirection, const QString &windpower, const QString &humidity)
    {
        this->weather = weather;
        this->temperature = temperature;
        this->winddirection = winddirection;
        this->windpower = windpower;
        this->humidity = humidity;
    }

    /**
     * @brief fieldIsEmpty 判断是否有字段值为空
     * @return 是否为空
     */
    bool fieldIsEmpty()
    {
        return weather.isEmpty() || temperature.isEmpty()
                 || winddirection.isEmpty() || windpower.isEmpty() || humidity.isEmpty();
    }
}stLiveInfo;

// 预报天气信息结构体
typedef struct stForecastsInfo
{
private:
    QHash<int, QString> weekHash;

public:
    QString date;           // 日期
    QString week;           // 星期
    QString dayweather;     // 日间天气
    QString nightweather;   // 夜间天气
    QString daytemp;        // 日间温度
    QString nighttemp;      // 夜间温度
    QString daywind;        // 日间风向
    QString nightwind;      // 夜间风向
    QString daypower;       // 日间风力等级
    QString nightpower;     // 夜间风力等级

    // 创建无参构造
    stForecastsInfo()
    {
        //! 初始化Hash的值，为方便获取周数据
        //! 此次可通过配置文件读取，不过我在这里就偷一下懒了，hhh
        weekHash[1] = "周一";
        weekHash[2] = "周二";
        weekHash[3] = "周三";
        weekHash[4] = "周四";
        weekHash[5] = "周五";
        weekHash[6] = "周六";
        weekHash[7] = "周日";
    }

    /**
     * @brief setDate 设置日期（自动设置星期）
     * @param dateStr 日期字符串
     */
    void setDate(const QString &dateStr)
    {
        // 结构体日期变量赋值
        this->date = dateStr;
        // 将日期字符串转为QDate对象(必须设置字符串的日期格式哦，否则识别不到日期)
        QDate date = QDate::fromString(dateStr, "yyyy-MM-dd");
        // 结构体星期变量赋值
        this->week = weekHash[date.dayOfWeek()];
    }

    /**
     * @brief setWeather 设置日/夜间天气
     * @param dayweather 日间天气
     * @param nightweather 夜间天气
     */
    void setWeather(const QString &dayweather, const QString &nightweather)
    {
        this->dayweather = dayweather;
        this->nightweather = nightweather;
    }

    /**
     * @brief setTemperature 设置日/夜间温度
     * @param daytemp 日间温度
     * @param nighttemp 夜间温度
     */
    void setTemperature(const QString &daytemp, const QString &nighttemp)
    {
        this->daytemp = daytemp;
        this->nighttemp = nighttemp;
    }

    /**
     * @brief setWindDirection 设置日/夜间风向
     * @param daywind 日间风向
     * @param nightwind 夜间风向
     */
    void setWindDirection(const QString &daywind, const QString &nightwind)
    {
        this->daywind = daywind;
        this->nightwind = nightwind;
    }

    /**
     * @brief setWindPower 设置日/夜间风力等级
     * @param daypower 日间风力等级
     * @param nightpower 夜间风力等级
     */
    void setWindPower(const QString &daypower, const QString &nightpower)
    {
        this->daypower = daypower;
        this->nightpower = nightpower;
    }
}stForecastsInfo;

// 天气信息结构体
typedef struct stWeatherInfo
{
    // 基本信息（实时、预报天气都有以下属性）
    QString province;           // 省份
    QString city;               // 城市
    QString adcode;             // 区域编码
    QString reporttime;         // 报告时间

    // 因为实时信息和预报信息内容不同，需分开存储
    stLiveInfo              stliveInfo;         // 实时天气信息
    // 实时信息包含多天天气，需用容器存储
    QList<stForecastsInfo>  stForecastInfoList; // 预报天气信息

    /**
     * @brief setBaseInfo 设置基础信息函数
     * @param province  省份
     * @param city 成都
     * @param adcode 区域编码
     * @param reportTime 报告时间
     */
    void setBaseInfo(const QString &province, const QString &city
                     , const QString &adcode, const QString &reportTime)
    {
        this->province = province;
        this->city = city;
        this->adcode = adcode;
        this->reporttime = reportTime;
    }

}stWeatherInfo;

#endif // STWEATHERFROECAST_H
