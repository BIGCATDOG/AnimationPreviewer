#ifndef ANIMATIONFRAME_H
#define ANIMATIONFRAME_H

#include <QEasingCurve>
#include <QFrame>
#include <QVector2D>
class AnimationFrame : public QFrame
{
    Q_OBJECT
public:
    enum PathType {
        Line,
        Bezier
    };
    struct Line {
        QPoint st;
        QPoint end;
    };
    struct Bezier {
        QPoint st;
        QPoint c1;
        QPoint c2;
        QPoint end;
    };

    struct Path {
        PathType type;
        union{
            struct Line line;
            struct Bezier bezier;
        }data;
    };
public:
    AnimationFrame(QWidget* parent);
public slots:
    void playAnimation();
    void onDurationChanged(double duration);
    void onEasingChanged(QEasingCurve::Type type);
    void onPathTypeChanged(AnimationFrame::PathType pathType);
    void onResetPath();
protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void paintEvent(QPaintEvent *event);
private:
    int pickedPointIndex(const QPoint& mousePoint);
private:
    double _duration = 1.0;
    PathType _pathType = PathType::Line;
    QEasingCurve::Type  _easingType = QEasingCurve::InOutCirc;
    int _pickedPointIndex = -1;
    QVector<QPoint> _points;
};

#endif // ANIMATIONFRAME_H
