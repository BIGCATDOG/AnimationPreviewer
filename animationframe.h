#ifndef ANIMATIONFRAME_H
#define ANIMATIONFRAME_H

#include <QEasingCurve>
#include <QFrame>
#include <QString>
#include <QVector2D>

class QPushButton;

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
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    QEasingCurve::Type getEasingTypeByIndex(int index);
public slots:
    void playAnimation();
    void onComparisonModeChanged(bool comparsionMode);
    void onDurationChanged(double duration);
    void onEasingChanged(QEasingCurve::Type type);
    void onMotionObjectSelected(int index);
    void onMotionObjectSurfaceChange(int index, const QString& imgPath);
    void onObjectImageChanged(const QString& imagePath);
    void onPathTypeChanged(AnimationFrame::PathType pathType);
    void onResetPath();
    void onWidthChanged(int w);
    void onHeightChanged(int h);
protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void showEvent(QShowEvent *event);
private:
    void initialPath();
    int pickedPointIndex(const QPoint& mousePoint);
    void updateMotionObjectSurface(QPushButton* btn, const QString& imagePath);
private:
    bool                _comparisonMode = false;
    double              _duration = 1.0;
    PathType            _pathType = PathType::Line;
    QEasingCurve::Type  _easingTypeObj1 = QEasingCurve::InOutCirc;
    QEasingCurve::Type  _easingTypeObj2 = QEasingCurve::InOutCirc;
    QPushButton*        _motionObject;
    QString             _objectImage;
    QString             _backgroundImage;
    QSize               _frameSize;
    int                 _pickedPointIndex = -1;
    int                 _selectedObjectIndex = 0;
    QVector<QPoint>     _points;
};

#endif // ANIMATIONFRAME_H
