#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QResizeEvent>
#include <time.h>

#include "../../src/mjhz.h"

// 麻将牌大小
#define MJPAI_W 42
#define MJPAI_H 60

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

    srand(time(NULL));

    _hu = 0;
    _imgMj.load(":/res/mj.png");
    _imgHu.load(":/res/hu.png");

    mjhz_init(&mjhz, 0, 4);
    mjhz_start(&mjhz);
    mjhz_sort(mjhz.players[0].tiles, MJHZ_MAX_PAIS);
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
}

void MainWindow::paintEvent(QPaintEvent*)
{
    int i;
    int x,y,n;

    QPainter painter(this);
    QRectF rcAll = rect();
    QBrush brush(QColor(16, 114, 16));
    painter.fillRect(rcAll, brush);

    x = y = 0;
    n = 1;
    for (i = 0; i < 34; ++i) {
        QRect src(n * MJPAI_W, 0, MJPAI_W, MJPAI_H);
        QRect dest(x, y, MJPAI_W, MJPAI_H);
        painter.drawPixmap(dest, _imgMj, src);
        n++;
        x += MJPAI_W;
        if ((n - 1) % 9 == 0) {
            x = 0;
            y += MJPAI_H;
        }
    }

    int offset;
    if (mjhz.players[0].tiles[MJHZ_MAX_PAIS-1] != 0)
        offset = 6;
    else
        offset = 0;
    n = 0;
    for (i = 0; i < MJHZ_MAX_PAIS; ++i) {
        if (mjhz.players[0].tiles[i] != 0)
            n++;
    }

    x = (rcAll.width() - (n * MJPAI_W) - offset) / 2;
    y = rcAll.bottom() - MJPAI_H;
    for (i = 0; i < MJHZ_MAX_PAIS; ++i) {
        int id = mjhz.players[0].tiles[i];
        if (id == 0)
            continue;
        if (i == (MJHZ_MAX_PAIS - 1)) {
            x += 6;
        }
        QRect src(id * MJPAI_W, 0, MJPAI_W, MJPAI_H);
        QRect dest(x, y, MJPAI_W, MJPAI_H);
        painter.drawPixmap(dest, _imgMj, src);
        x += MJPAI_W;
    }

    if (_hu) {
        painter.drawPixmap((rcAll.width() - _imgHu.width()) / 2,
                           rcAll.bottom() - MJPAI_H - 2 - _imgHu.height(),
                           _imgHu);
    }
}

bool MainWindow::pointInRect(const QPoint &pt, const QRect &rect)
{
    if (pt.x() >= rect.left() && pt.x() <= rect.right() &&
            pt.y() >= rect.top() && pt.y() <= rect.bottom()) {
        return true;
    } else {
        return false;
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    int i,id,n;
    int out_index = -1;
    int in_index = -1;

    QRectF rcAll = rect();
    if (event->button() == Qt::LeftButton) {
        int offset;
        if (mjhz.players[0].tiles[MJHZ_MAX_PAIS-1] != 0)
            offset = 6;
        else
            offset = 0;
        n = 0;
        for (i = 0; i < MJHZ_MAX_PAIS; ++i) {
            if (mjhz.players[0].tiles[i] != 0)
                n++;
        }

        int x = (rcAll.width() - (n * MJPAI_W) - offset) / 2;
        int y = rcAll.bottom() - MJPAI_H;

        QPoint pos = event->pos();
        for (i = 0; i < MJHZ_MAX_PAIS; i++) {
            if (mjhz.players[0].tiles[i] == 0)
                continue;
            if (i == (MJHZ_MAX_PAIS - 1))
                x += 6;
            QRect rcPai(x, y, MJPAI_W, MJPAI_H);
            if (pointInRect(pos, rcPai)) {
                // 选中了牌
                out_index = i;
                break;
            }
            x += MJPAI_W;
        }
        if (out_index != -1) {
            id = mjhz.players[0].tiles[out_index];
            mjhz.players[0].tiles_js[id]--;
            mjhz.players[0].tiles[out_index] = 0;
            mj_trim(mjhz.players[0].tiles, MJHZ_MAX_PAIS);
            mjhz_sort(mjhz.players[0].tiles, MJHZ_MAX_PAIS);
            _hu = 0;
            ui->listWidget->clear();
            update();
        } else {
            QRect rcIn(0, 0, 9 * MJPAI_W, MJPAI_H);
            if (pointInRect(pos, rcIn)) {
                // 选了万子
                in_index = MJ_ID_1W + pos.x() / MJPAI_W;
            }
            rcIn.setRect(0, MJPAI_H, 9 * MJPAI_W, MJPAI_H);
            if (pointInRect(pos, rcIn)) {
                // 选了索子
                in_index = MJ_ID_1S + pos.x() / MJPAI_W;
            }
            rcIn.setRect(0, 2 * MJPAI_H, 9 * MJPAI_W, MJPAI_H);
            if (pointInRect(pos, rcIn)) {
                // 选了筒子
                in_index = MJ_ID_1T + pos.x() / MJPAI_W;
            }
            rcIn.setRect(0, 3 * MJPAI_H, 7 * MJPAI_W, MJPAI_H);
            if (pointInRect(pos, rcIn)) {
                // 字牌
                in_index = MJ_ID_DONG + pos.x() / MJPAI_W;
            }
            if (in_index != -1) {
                for (i = 0; i < MJHZ_MAX_PAIS; ++i) {
                    if (mjhz.players[0].tiles[i] == 0) {
                        mjhz.players[0].tiles[i] = in_index;
                        mjhz.players[0].tiles_js[in_index]++;
                        update();
                        break;
                    }
                }
            }
        }
    }
}

void MainWindow::on_pushButton_clicked()
{
    ui->listWidget->clear();
    if (mjhz_can_hu(&mjhz, 0) > 0) {
        // 胡了
        _hu = 1;
        if (mjhz.players[0].hu.is_pair7) {
            ui->listWidget->addItem(tr("七对子"));
        }
        if (mjhz.players[0].hu.pair7_h4 > 0) {
            QString str =
                    QString("豪华对数:%1").arg(mjhz.players[0].hu.pair7_h4);
            ui->listWidget->addItem(str);
        }
    } else {
        _hu = 0;
    }
    update();
}
