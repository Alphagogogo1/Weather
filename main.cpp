#include "CWeatherForecast.h"
#include <QApplication>

/**
 * @brief loadStyleSheet 加载样式表
 * @param path 样式文件路径
 */
void loadStyleSheet(QString path)
{
    // 指定文件并打开
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(nullptr, "提示", path + ":文件打开失败");
    }
    else
    {
        // 读取所有样式并设置
        qApp->setStyleSheet(file.readAll());
    }
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CWeatherForecast w;
    w.initialize();

    // 加载样式表
    QString fileName = "./StyleTheme/WeatherForecast.qss";
    loadStyleSheet(fileName);

    w.show();
    int exec = a.exec();
    w.unInitialize();
    return exec;
}
