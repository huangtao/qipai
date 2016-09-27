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

    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    Ui::MainWindow *ui;
    QPixmap _imgMj[43]; // 1-42
};

#endif // MAINWINDOW_H
