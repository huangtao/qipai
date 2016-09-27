#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>

#include "../../src/mjhz.h"

mjhz_t mjhz;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    for (int i = 1; i < 43; ++i) {
        QString str = QString(":/res/mah/mj%1.png").arg(i);
        _imgMj[i].load(str);
    }

    mjhz_init(&mjhz, 0, 4);
    mjhz_start(&mjhz);
    mjhz_sort(mjhz.players[0].tiles);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent*)
{
    int i,j;
    int x,y,n;

    QPainter painter(this);
    QRectF rcAll = rect();
    QBrush brush(QColor(16, 114, 16));
    painter.fillRect(rcAll, brush);

    x = y = 2;
    n = 1;
    for (i = 0; i < 3; ++i) {
        for (j = 0; j < 9; ++j) {
            painter.drawPixmap(x, y, _imgMj[n]);
            n++;
            x += _imgMj[1].width() + 2;
        }
        x = 2;
        y += _imgMj[1].height() + 2;
    }
    for (i = 0; i < 7; ++i) {
        painter.drawPixmap(x, y, _imgMj[n]);
        n++;
        x += _imgMj[1].width() + 2;
    }

    x = (rcAll.width() - (14 * _imgMj[1].width() + 6)) / 2;
    y = rcAll.bottom() - _imgMj[1].height();
    for (i = 0; i < MJHZ_MAX_PAIS; ++i) {
        painter.drawPixmap(
                    x,
                    y,
                    _imgMj[mjhz.players[0].tiles[i]]);
        x += _imgMj[1].width() + 2;
    }
}
