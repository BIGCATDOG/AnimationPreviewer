#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->comboBox_pathType->addItem("Line");
    ui->comboBox_pathType->addItem("Bezier");
    ui->comboBox_easaingType->addItems({
                                           "Linear",
                                           "InQuad", "OutQuad", "InOutQuad", "OutInQuad",
                                           "InCubic", "OutCubic", "InOutCubic", "OutInCubic",
                                           "InQuart", "OutQuart", "InOutQuart", "OutInQuart",
                                           "InQuint", "OutQuint", "InOutQuint", "OutInQuint",
                                           "InSine", "OutSine", "InOutSine", "OutInSine",
                                           "InExpo", "OutExpo", "InOutExpo", "OutInExpo",
                                           "InCirc", "OutCirc", "InOutCirc", "OutInCirc",
                                           "InElastic", "OutElastic", "InOutElastic", "OutInElastic",
                                           "InBack", "OutBack", "InOutBack", "OutInBack",
                                           "InBounce", "OutBounce", "InOutBounce", "OutInBounce",
                                           "InCurve", "OutCurve", "SineCurve", "CosineCurve",
                                           "BezierSpline", "TCBSpline", "Custom", "NCurveTypes"
                                       });
}

MainWindow::~MainWindow()
{
    delete ui;
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

