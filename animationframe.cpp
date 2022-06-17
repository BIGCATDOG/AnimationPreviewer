#include "animationframe.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileInfo>
#include <QFileDialog>
#include <QImage>
#include <QMimeData>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSizePolicy>
#include <QTimeLine>

namespace  {
const int kPickedTolerance = 10;
const int kCircleRadius = 8;
const Qt::GlobalColor colors[4] = {
    Qt::darkGreen, Qt::magenta, Qt::darkCyan, Qt::darkYellow
};
const QSize kDefaultSize = QSize(600, 800);
QEasingCurve::Type kObjecsEasingType[2] = {QEasingCurve::Linear, QEasingCurve::Linear};
QString kMotionObjectImagePath[2] = {};
}
AnimationFrame::AnimationFrame(QWidget *parent)
    :QFrame(parent)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
   _frameSize = kDefaultSize;
   setMaximumSize(_frameSize);
   setMinimumSize(_frameSize);
   setAcceptDrops(true);

}

QSize AnimationFrame::sizeHint() const
{
    return _frameSize;
}

QSize AnimationFrame::minimumSizeHint() const
{
    return QSize(600, 800);
}

QEasingCurve::Type AnimationFrame::getEasingTypeByIndex(int index)
{
    return kObjecsEasingType[index];
}

void AnimationFrame::playAnimation()
{
    if (_pathType == Line && _points.size() > 1) {
        auto object = new QPushButton(this);
        updateMotionObjectSurface(object, kMotionObjectImagePath[0]);
        object->setFixedSize(QSize(40, 40));
        object->show();
        QPropertyAnimation *animation = new QPropertyAnimation(object, "pos");
        animation->setDuration(_duration * 1000);
        animation->setStartValue(_points[0]);
        animation->setEndValue(_points[1]);
        animation->setEasingCurve(kObjecsEasingType[0]);
        animation->start();
        connect(animation, &QPropertyAnimation::finished,[=]() {
            object->deleteLater();
        });
        if (_comparisonMode) {
            auto object = new QPushButton(this);
            object->setFixedSize(QSize(40, 40));
            QPropertyAnimation *animation = new QPropertyAnimation(object, "pos");
            if (kMotionObjectImagePath[1].isEmpty()) {
                object->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(255, 255, 255, 255));");
            } else {
                updateMotionObjectSurface(object, kMotionObjectImagePath[1]);
            }
            object->show();
            animation->setDuration(_duration * 1000);
            animation->setStartValue(_points[0]);
            animation->setEndValue(_points[1]);
            animation->setEasingCurve(kObjecsEasingType[1]);
            animation->start();
            connect(animation, &QPropertyAnimation::finished,[=]() {
                object->deleteLater();
            });
        }


    } else if(_pathType == Bezier && _points.size() > 3) {
        QTimeLine* timeLine = new QTimeLine(_duration * 1000, this);
        auto object = new QPushButton(this);
        object->setFixedSize(QSize(40, 40));
        updateMotionObjectSurface(object, kMotionObjectImagePath[0]);
        object->show();
        timeLine->setEasingCurve(QEasingCurve(kObjecsEasingType[0]));
        connect(timeLine, &QTimeLine::valueChanged, [=](qreal value) {
            qDebug()<<"obj1 :" << value;
            float currentTime = value;
            QPoint p0 = _points[0];
            QPoint p1 = _points[1];
            QPoint p2 = _points[2];
            QPoint p3 = _points[3];

            QPoint position = p0 * pow(1 - currentTime, 3) +
                    3 * p1 * currentTime * pow(1 - currentTime, 2) +
                    3 * p2 * pow(currentTime, 2) * (1 - currentTime) +
                    p3 * pow(currentTime, 3);
            object->move(position);
        });
        connect(timeLine, &QTimeLine::finished, [=]() {
            timeLine->deleteLater();
            object->deleteLater();
        });
        timeLine->start();

        if (_comparisonMode) {
            auto object = new QPushButton(this);
            object->setFixedSize(QSize(40, 40));
            if (kMotionObjectImagePath[1].isEmpty()) {
                object->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(255, 255, 255, 255));");
            } else {
                updateMotionObjectSurface(object, kMotionObjectImagePath[1]);
            }
            object->show();
            QTimeLine* timeLine = new QTimeLine(_duration * 1000, this);
            timeLine->setEasingCurve(QEasingCurve(kObjecsEasingType[1]));
            connect(timeLine, &QTimeLine::valueChanged, [=](qreal value) {
                qDebug()<<"obj2 :" << value;
                float currentTime = value;
                QPoint p0 = _points[0];
                QPoint p1 = _points[1];
                QPoint p2 = _points[2];
                QPoint p3 = _points[3];

                QPoint position = p0 * pow(1 - currentTime, 3) +
                        3 * p1 * currentTime * pow(1 - currentTime, 2) +
                        3 * p2 * pow(currentTime, 2) * (1 - currentTime) +
                        p3 * pow(currentTime, 3);
                object->move(position);
            });
            connect(timeLine, &QTimeLine::finished, [=]() {
                timeLine->deleteLater();
                object->deleteLater();
            });
            timeLine->start();
        }
    }
}

void AnimationFrame::onComparisonModeChanged(bool comparsionMode)
{
    _comparisonMode = comparsionMode;
}

void AnimationFrame::onDurationChanged(double duration)
{
    _duration = duration;
}

void AnimationFrame::onEasingChanged(QEasingCurve::Type type)
{
    kObjecsEasingType[_selectedObjectIndex] = type;
}

void AnimationFrame::onMotionObjectSelected(int index)
{
    _selectedObjectIndex = index;
}

void AnimationFrame::onMotionObjectSurfaceChange(int index, const QString &imgPath)
{
    kMotionObjectImagePath[index] = imgPath;
}

void AnimationFrame::onObjectImageChanged(const QString &imagePath)
{
    _objectImage = imagePath;
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

void AnimationFrame::onWidthChanged(int w)
{
    _frameSize.setWidth(w);
}

void AnimationFrame::onHeightChanged(int h)
{
    _frameSize.setHeight(h);
}

void AnimationFrame::dragEnterEvent(QDragEnterEvent *event)
{
    auto mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QUrl file = mimeData->urls()[0];
        QString filePath = file.toLocalFile();
        QFileInfo info(filePath);
        if (info.suffix()!= "png" &&
            info.suffix()!= "xpm" &&
            info.suffix()!= "jpg" &&
            info.suffix()!= "jpeg" &&
            info.suffix()!= "webp") {
            event->ignore();
        } else {
            event->acceptProposedAction();
        }
    }
}

void AnimationFrame::dropEvent(QDropEvent *event)
{
    auto mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QUrl file = mimeData->urls()[0];
        _backgroundImage = file.toLocalFile();
        update();
    }
}

void AnimationFrame::mousePressEvent(QMouseEvent *event)
{
    if(_pathType == Line) {
        if (_points.size() == 2) {
            _pickedPointIndex = pickedPointIndex(event->pos());
            return;
        }

    } else if(_pathType == Bezier) {
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
    QImage image;
    if (_backgroundImage.isEmpty()) {
        image = QImage(this->size(), QImage::Format_ARGB32);
        image.fill(Qt::white);
    } else {
        image.load(_backgroundImage);
        image = image.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    QPen pen;
    pen.setCapStyle(Qt::SquareCap);
    pen.setColor(QColor(0xd722a7));
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawImage(QPoint(0, 0), image);
    painter.save();
    if( _points.size() < 1) {
        return;
    }
    if (_pathType == Line && _points.size() == 2) {
        QPen pen = painter.pen();
        pen.setColor(colors[0]);
        painter.setPen(pen);
        painter.drawEllipse(_points[0], kCircleRadius, kCircleRadius);

        painter.restore();
        painter.drawLine(_points[0],_points[1]);

        pen.setColor(colors[3]);
        painter.setPen(pen);
        painter.drawEllipse(_points[1], kCircleRadius, kCircleRadius);

    } else if (_pathType == Bezier && _points.size() == 4) {
        QPainterPath path;
        path.moveTo(_points[0]);
        path.cubicTo(_points[1], _points[2], _points[3]);
        int i = 4;
        QPen pen = painter.pen();
        while (i--) {
            pen.setColor(colors[i]);
            painter.setPen(pen);
            painter.drawEllipse(_points[i], kCircleRadius, kCircleRadius);
        }
        painter.restore();
        painter.drawPath(path);
    } else {
        QPen pen = painter.pen();
        for(int i = 0; i < _points.size(); i++){
            pen.setColor(colors[i]);
            painter.setPen(pen);
            painter.drawEllipse(_points[i], kCircleRadius, kCircleRadius);
        }
    }
}

void AnimationFrame::showEvent(QShowEvent *event)
{
    QFrame::showEvent(event);
    initialPath();
}

void AnimationFrame::initialPath()
{
    _points.push_back(QPoint(50,50));
    auto size = this->size();
    auto p2 = QPoint(size.width() - 50, size.height() - 50);
    _points.push_back(p2);
    update();
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

void AnimationFrame::updateMotionObjectSurface(QPushButton *btn, const QString &imagePath)
{
    if (imagePath.isEmpty()) {
        return;
    }
    QPixmap pic(imagePath);
    btn->setFixedSize(QSize(40, 40));
    pic = pic.scaled(btn->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    btn->setMask(pic.mask());
    btn->setIcon(QIcon(imagePath));
    btn->setIconSize(btn->size());
}
