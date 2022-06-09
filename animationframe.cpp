#include "animationframe.h"

#include <QImage>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QTimeLine>
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
            //                // dy/dt
            //                float y = -3 * p0.y * pow(1 - currentTime,2) +
            //                          3 * p1.y *(1 - currentTime) *(1 - 3 * currentTime) +
            //                          3* p2.y * currentTime * (2 - 3 *currentTime) +
            //                          3 * p3.y * pow(currentTime ,2);
            //                float x = -3 * p0.x * pow(1 - currentTime, 2) +
            //                    3 * p1.x * (1 - currentTime) * (1 - 3 * currentTime) +
            //                    3 * p2.x * currentTime * (2 - 3 * currentTime) +
            //                    3 * p3.x * pow(currentTime, 2);;
            //                float res = y / x;
            //                float angle = MATH_RAD_TO_DEG(std::atan(res));
            //                node->drawLine(Vec2(0, 0), Vec2(20,0), Color4F::RED);
            //                auto label = Label::createWithSystemFont(std::to_string(angle), "Arial", 10);
            //                _target->getParent()->addChild(label);
            //                label->setPosition(position);
            //                node->setPosition(position);
            //                p->setPosition(position);
            //                node->setRotation(-angle);
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
    if(_pathType==Line && _points.size() >1) {
        return;
    } else if(_pathType ==Bezier && _points.size() >3) {
        return;
    }
    _points.push_back(event->pos());
}

void AnimationFrame::mouseReleaseEvent(QMouseEvent *event)
{
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
