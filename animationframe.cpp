#include "animationframe.h"

#include <QImage>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QTimeLine>
namespace  {
const int kPickedTolerance = 8;
}
AnimationFrame::AnimationFrame(QWidget *parent)
    :QFrame(parent)
{
}

void AnimationFrame::playAnimation()
{
    if (_pathType == Line && _points.size() > 1) {
        auto btn = new QPushButton(this);
        btn->show();
        QPropertyAnimation *animation = new QPropertyAnimation(btn, "pos");
        animation->setDuration(_duration * 1000);
        animation->setStartValue(_points[0]);
        animation->setEndValue(_points[1]);
        animation->setEasingCurve(_easingType);
        animation->start();
        connect(animation,&QPropertyAnimation::finished,[=](){
            btn->deleteLater();
        });
    } else if(_pathType == Bezier && _points.size() > 3) {
        auto btn = new QPushButton(this);
        auto btnSize = btn->size();
        auto center = btn->pos() - QPoint(btnSize.width()/2,btnSize.height()/2);
        btn->move(center);
        btn->show();
        QTimeLine* timeLine = new QTimeLine(_duration * 1000, this);
        timeLine->setEasingCurve(QEasingCurve(_easingType));
        connect(timeLine,&QTimeLine::valueChanged,[=](qreal value){
            qDebug()<<value;
            float currentTime = value;
            QPoint p0 = _points[0];
            QPoint p1 = _points[1];
            QPoint p2 = _points[2];
            QPoint p3 = _points[3];

            QPoint position = p0 * pow(1 - currentTime, 3) +
                    3 * p1 * currentTime * pow(1 - currentTime, 2) +
                    3 * p2 * pow(currentTime, 2) * (1 - currentTime) +
                    p3 * pow(currentTime, 3);
            auto s = QPoint(btnSize.width()/2,btnSize.height()/2);
            btn->move(position);
        });
        connect(timeLine,&QTimeLine::finished,[=](){
            btn->deleteLater();
        });
        timeLine->start();
    }
}

void AnimationFrame::onDurationChanged(double duration)
{
    _duration = duration;
}

void AnimationFrame::onEasingChanged(QEasingCurve::Type type)
{
    _easingType = type;
}

void AnimationFrame::onPathTypeChanged(PathType pathType)
{
    _pathType = pathType;
    _points.clear();
    update();
}

void AnimationFrame::onResetPath()
{
    _points.clear();
    update();
}

void AnimationFrame::mousePressEvent(QMouseEvent *event)
{
    if(_pathType==Line) {
        if (_points.size() == 2) {
            _pickedPointIndex = pickedPointIndex(event->pos());
             return;
        }

    } else if(_pathType ==Bezier) {
        if (_points.size() == 4) {
            _pickedPointIndex = pickedPointIndex(event->pos());
             return;
        }
    }
    _points.push_back(event->pos());
}

void AnimationFrame::mouseMoveEvent(QMouseEvent *event) {
    QRect moveRegion = this->rect();
    moveRegion.adjust(10, 10, -10, -10);
    if (_pickedPointIndex != -1 && moveRegion.contains(event->pos())) {
        _points[_pickedPointIndex] = event->pos();
        update();
    }
    qDebug() << event;
}

void AnimationFrame::mouseReleaseEvent(QMouseEvent *event)
{
    if (_pickedPointIndex != -1) {
        _pickedPointIndex = -1;
    }
    update();
}

void AnimationFrame::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QImage image(this->size(), QImage::Format_ARGB32);
    image.fill(Qt::white);
    QPen pen;
    pen.setCapStyle(Qt::SquareCap);
    pen.setColor(Qt::blue);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawImage(QPoint(0, 0), image);
    if( _points.size() < 1) {
        return;
    }
    if (_pathType == Line && _points.size() ==2) {
        painter.drawLine(_points[0],_points[1]);
        painter.drawEllipse(_points[1],5 , 5);
        painter.drawEllipse(_points[0],5 , 5);
    } else if (_pathType == Bezier && _points.size()==4) {
        QPainterPath path;
        path.moveTo(_points[0]);
        path.cubicTo(_points[1],_points[2],_points[3]);
        path.addEllipse(_points[0],5,5);
        path.addEllipse(_points[1],5,5);
        path.addEllipse(_points[2],5,5);
        path.addEllipse(_points[3],5,5);
        painter.drawPath(path);
    } else {
        for(auto point : _points){
            painter.drawEllipse(point,5,5);
        }
    }
}

int AnimationFrame::pickedPointIndex(const QPoint &mousePoint)
{
    auto distance = [](QPoint p1, QPoint p2) {
        return  sqrt(pow(p1.x() - p2.x(), 2) + pow(p1.x() - p2.x(), 2));
    };
    int i = 0;
    for (auto point : _points) {
        if (distance(point, mousePoint) <= kPickedTolerance) {
            return i;
        }
        i++;
    }
    return -1;
}
