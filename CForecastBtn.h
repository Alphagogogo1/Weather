#ifndef CFORECASTBTN_H
#define CFORECASTBTN_H

#include <QWidget>

namespace Ui {
class CForecastBtn;
}

class CForecastBtn : public QWidget
{
    Q_OBJECT

public:
    explicit CForecastBtn(QWidget *parent = nullptr);
    ~CForecastBtn();

    /**
     * @brief setBtnInfo 设置按钮显示信息
     * @param week 周几
     * @param weather 天气
     * @param dayTemp 日间温度
     * @param nightTemp 夜间温度
     */
    void setBtnInfo(QString week, QString weather, QString dayTemp, QString nightTemp);
signals:
    /**
     * @brief clicked 点击信号
     */
    void clicked();

private:
    Ui::CForecastBtn *ui;

    // QWidget interface
protected:
    /**
     * @brief mouseReleaseEvent 鼠标释放时间
     * @param event 事件对象
     */
    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // CFORECASTBTN_H
