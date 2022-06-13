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
}
AnimationFrame::AnimationFrame(QWidget *parent)
    :QFrame(parent)
{
    _motionObject = new QPushButton(this);
    _motionObject->setFixedSize(QSize(40, 40));
    connect(_motionObject,&QPushButton::clicked,[=](){
        auto imagePath = QFileDialog::getOpenFileName(Q_NULLPTR, "Pick a Image", QDir::homePath(), tr("Images (*.png *.xpm *.jpg *.webp)"));
        if (!imagePath.isEmpty()) {
            _objectImage = imagePath;
            QPixmap pic(_objectImage);
            pic = pic.scaled(_motionObject->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            _motionObject->setMask(pic.mask());
            _motionObject->setIcon(QIcon(_objectImage));
            _motionObject->setIconSize(_motionObject->size());
        }
    });
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
   _frameSize = kDefaultSize;
   setMaximumSize(_frameSize);
   setMinimumSize(_frameSize);
    setAcceptDrops(true);
    _motionObject->show();
}

QSize AnimationFrame::sizeHint() const
{
    return _frameSize;
}

QSize AnimationFrame::minimumSizeHint() const
{
    return QSize(600, 800);
}

void AnimationFrame::playAnimation()
{
    if (_pathType == Line && _points.size() > 1) {

        QPropertyAnimation *animation = new QPropertyAnimation(_motionObject, "pos");
        animation->setDuration(_duration * 1000);
        animation->setStartValue(_points[0]);
        animation->setEndValue(_points[1]);
        animation->setEasingCurve(_easingType);
        animation->start();
    } else if(_pathType == Bezier && _points.size() > 3) {
        QTimeLine* timeLine = new QTimeLine(_duration * 1000, this);
        auto btnSize = _motionObject->size();
        timeLine->setEasingCurve(QEasingCurve(_easingType));
        connect(timeLine, &QTimeLine::valueChanged, [=](qreal value) {
            qDebug()<< value;
            float currentTime = value;
            QPoint p0 = _points[0];
            QPoint p1 = _points[1];
            QPoint p2 = _points[2];
            QPoint p3 = _points[3];

            QPoint position = p0 * pow(1 - currentTime, 3) +
                    3 * p1 * currentTime * pow(1 - currentTime, 2) +
                    3 * p2 * pow(currentTime, 2) * (1 - currentTime) +
                    p3 * pow(currentTime, 3);
            auto s = QPoint(btnSize.width() / 2, btnSize.height() / 2);
            _motionObject->move(position);
        });
        connect(timeLine, &QTimeLine::finished, [=]() {
            timeLine->deleteLater();
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
    setMaximumSize(_frameSize);
    setMinimumSize(_frameSize);
    updateGeometry();
}

void AnimationFrame::onHeightChanged(int h)
{
    _frameSize.setHeight(h);
    setMaximumSize(_frameSize);
    setMinimumSize(_frameSize);
    updateGeometry();
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
