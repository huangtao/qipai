#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool pointInRect(const QPoint &pt, const QRect &rect);

    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QPixmap _imgMj;
    QPixmap _imgHu;

    int _hu;
    int _winH;
    int _winW;
    QRect _rcWan;
    QRect _rcSuo;
    QRect _rcTong;
    QRect _rcHornor;
    QRect _rcPai;
};

#endif // MAINWINDOW_H
