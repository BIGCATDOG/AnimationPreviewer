#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QEasingCurve>
#include <QMetaEnum>
#include <QPainter>
#include <QPainterPath>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    _iconSize = QSize(64, 64);
    ui->setupUi(this);
    ui->easingCurvePicker->setIconSize(_iconSize);
    ui->easingCurvePicker->setMinimumHeight(_iconSize.height() + 50);
    ui->comboBox_pathType->addItem("Line");
    ui->comboBox_pathType->addItem("Bezier");
    createCurveIcons();
    ui->easingCurvePicker->setCurrentRow(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QEasingCurve createEasingCurve(QEasingCurve::Type curveType)
{
    QEasingCurve curve(curveType);

    if (curveType == QEasingCurve::BezierSpline) {
        curve.addCubicBezierSegment(QPointF(0.4, 0.1), QPointF(0.6, 0.9), QPointF(1.0, 1.0));

    } else if (curveType == QEasingCurve::TCBSpline) {
        curve.addTCBSegment(QPointF(0.0, 0.0), 0, 0, 0);
        curve.addTCBSegment(QPointF(0.3, 0.4), 0.2, 1, -0.2);
        curve.addTCBSegment(QPointF(0.7, 0.6), -0.2, 1, 0.2);
        curve.addTCBSegment(QPointF(1.0, 1.0), 0, 0, 0);
    }

    return curve;
}

void MainWindow::on_pushButton_clicked()
{
    ui->frame->playAnimation();
}


void MainWindow::on_comboBox_pathType_currentIndexChanged(int index)
{
    ui->frame->onPathTypeChanged((AnimationFrame::PathType)index);
}


void MainWindow::on_comboBox_easaingType_currentIndexChanged(int index)
{
    ui->frame->onEasingChanged((QEasingCurve::Type)index);
}


void MainWindow::on_doubleSpinBox_valueChanged(double arg1)
{
    ui->frame->onDurationChanged(arg1);
}


void MainWindow::on_pushButton_2_clicked()
{
    ui->frame->onResetPath();
}


void MainWindow::on_spinBox_width_valueChanged(int arg1)
{
    ui->frame->onWidthChanged(arg1);
}


void MainWindow::on_spinBox_height_valueChanged(int arg1)
{
    ui->frame->onHeightChanged(arg1);
}

void MainWindow::createCurveIcons()
{
    QPixmap pix(_iconSize);
    QPainter painter(&pix);
    QLinearGradient gradient(0,0, 0, _iconSize.height());
    gradient.setColorAt(0.0, QColor(240, 240, 240));
    gradient.setColorAt(1.0, QColor(224, 224, 224));
    QBrush brush(gradient);
    const QMetaObject &mo = QEasingCurve::staticMetaObject;
    QMetaEnum metaEnum = mo.enumerator(mo.indexOfEnumerator("Type"));
    // Skip QEasingCurve::Custom
    for (int i = 0; i < QEasingCurve::NCurveTypes - 1; ++i) {
        painter.fillRect(QRect(QPoint(0, 0), _iconSize), brush);
        QEasingCurve curve = createEasingCurve((QEasingCurve::Type) i);
        painter.setPen(QColor(0, 0, 255, 64));
        qreal xAxis = _iconSize.height()/1.5;
        qreal yAxis = _iconSize.width()/3;
        painter.drawLine(0, xAxis, _iconSize.width(),  xAxis);
        painter.drawLine(yAxis, 0, yAxis, _iconSize.height());

        qreal curveScale = _iconSize.height()/2;

        painter.setPen(Qt::NoPen);

        // start point
        painter.setBrush(Qt::red);
        QPoint start(yAxis, xAxis - curveScale * curve.valueForProgress(0));
        painter.drawRect(start.x() - 1, start.y() - 1, 3, 3);

        // end point
        painter.setBrush(Qt::blue);
        QPoint end(yAxis + curveScale, xAxis - curveScale * curve.valueForProgress(1));
        painter.drawRect(end.x() - 1, end.y() - 1, 3, 3);

        QPainterPath curvePath;
        curvePath.moveTo(start);
        for (qreal t = 0; t <= 1.0; t+=1.0/curveScale) {
            QPoint to;
            to.setX(yAxis + curveScale * t);
            to.setY(xAxis - curveScale * curve.valueForProgress(t));
            curvePath.lineTo(to);
        }
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.strokePath(curvePath, QColor(32, 32, 32));
        painter.setRenderHint(QPainter::Antialiasing, false);
        QListWidgetItem *item = new QListWidgetItem;
        item->setIcon(QIcon(pix));
        item->setText(metaEnum.key(i));
        ui->easingCurvePicker->addItem(item);
    }
}

void MainWindow::on_easingCurvePicker_currentRowChanged(int currentRow)
{
    ui->frame->onEasingChanged((QEasingCurve::Type)currentRow);
}

