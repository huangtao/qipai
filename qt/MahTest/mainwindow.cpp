#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QResizeEvent>

#include "../../src/mjhz.h"

mjhz_t mjhz;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QRect rc = geometry();
    setFixedSize(rc.width(), rc.height());
    _winH = rc.height();
    _winW = rc.width();

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

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QSize sz = event->size();
    _winW = sz.width();
    _winH = sz.height();
    int center_x = _winW / 2;
    _rcWan.setLeft(2);
    _rcWan.setTop(2);
    _rcWan.setRight(_rcWan.left() + 9 * (_imgMj[1].width() + 2));
    _rcWan.setBottom(_rcWan.top() + _imgMj[1].height());
    _rcSuo.setLeft(2);
    _rcSuo.setTop(_rcWan.bottom() + 2);
    _rcSuo.setRight(_rcSuo.left() + 9 * (_imgMj[1].width() + 2));
    _rcSuo.setBottom(_rcSuo.top() + _imgMj[1].height());
    _rcTong.setLeft(2);
    _rcTong.setTop(_rcSuo.bottom() + 2);
    _rcTong.setRight(_rcTong.left() + 9 * (_imgMj[1].width() + 2));
    _rcTong.setBottom(_rcTong.top() + _imgMj[1].height());
    _rcHornor.setLeft(2);
    _rcHornor.setTop(_rcTong.bottom() + 2);
    _rcHornor.setRight(_rcHornor.left() + 7 * (_imgMj[1].width() + 2));
    _rcHornor.setBottom(_rcHornor.top() + _imgMj[1].height());
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
        if (i == 0) {
            x = _rcWan.left();
            y = _rcWan.top();
        } else if (i == 1) {
            x = _rcSuo.left();
            y = _rcSuo.top();
        } else {
            x = _rcTong.left();
            y = _rcTong.top();
        }
        for (j = 0; j < 9; ++j) {
            painter.drawPixmap(x, y, _imgMj[n]);
            n++;
            x += _imgMj[1].width() + 2;
        }
        x = 2;
        y += _imgMj[1].height() + 2;
    }
    x = _rcHornor.left();
    y = _rcHornor.top();
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
