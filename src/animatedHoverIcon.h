#ifndef ANIMATEDHOVERICON_H
#define ANIMATEDHOVERICON_H

#include <QLabel>
#include <QVariantAnimation>
#include <QEnterEvent>
#include <QEvent>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QUrl>
#include <QGraphicsOpacityEffect>
#include <QPainter>

class AnimatedHoverIcon : public QLabel
{
public:
    explicit AnimatedHoverIcon(const QString &url = "", QWidget *parent = nullptr);
    void setUrl(const QString &url) { targetUrl = url; }

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QVariantAnimation *scaleAnimation;
    QVariantAnimation *opacityAnimation;
    QGraphicsOpacityEffect *opacityEffect;
    QString targetUrl;
    double currentScale;
    double currentOpacity;
    bool isPressed;
    QPixmap originalPixmap;
};

inline AnimatedHoverIcon::AnimatedHoverIcon(const QString &url, QWidget *parent)
    : QLabel(parent), targetUrl(url), currentScale(0.85), currentOpacity(0.95), isPressed(false)
{
    setCursor(Qt::PointingHandCursor);

    opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(currentOpacity);
    setGraphicsEffect(opacityEffect);

    scaleAnimation = new QVariantAnimation(this);
    scaleAnimation->setDuration(200);
    scaleAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    opacityAnimation = new QVariantAnimation(this);
    opacityAnimation->setDuration(200);
    opacityAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    connect(scaleAnimation, &QVariantAnimation::valueChanged, [this](const QVariant &value)
            {
                currentScale = value.toDouble();
                update(); });

    connect(opacityAnimation, &QVariantAnimation::valueChanged, [this](const QVariant &value)
            {
        currentOpacity = value.toDouble();
        opacityEffect->setOpacity(currentOpacity); });
}

inline void AnimatedHoverIcon::enterEvent(QEnterEvent *event)
{
    if (!isPressed)
    {
        scaleAnimation->stop();
        scaleAnimation->setStartValue(currentScale);
        scaleAnimation->setEndValue(1.0);
        scaleAnimation->start();

        opacityAnimation->stop();
        opacityAnimation->setStartValue(currentOpacity);
        opacityAnimation->setEndValue(1.0);
        opacityAnimation->start();
    }
    QLabel::enterEvent(event);
}

inline void AnimatedHoverIcon::leaveEvent(QEvent *event)
{
    if (!isPressed)
    {
        scaleAnimation->stop();
        scaleAnimation->setStartValue(currentScale);
        scaleAnimation->setEndValue(0.85);
        scaleAnimation->start();

        opacityAnimation->stop();
        opacityAnimation->setStartValue(currentOpacity);
        opacityAnimation->setEndValue(0.95);
        opacityAnimation->start();
    }
    QLabel::leaveEvent(event);
}

inline void AnimatedHoverIcon::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        isPressed = true;
        scaleAnimation->stop();
        scaleAnimation->setDuration(100);
        scaleAnimation->setStartValue(currentScale);
        scaleAnimation->setEndValue(0.9);
        scaleAnimation->start();
    }
    QLabel::mousePressEvent(event);
}

inline void AnimatedHoverIcon::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isPressed)
    {
        isPressed = false;
        scaleAnimation->setDuration(200);

        if (rect().contains(event->pos()))
        {
            scaleAnimation->stop();
            scaleAnimation->setStartValue(currentScale);
            scaleAnimation->setEndValue(1.0);
            scaleAnimation->start();

            if (!targetUrl.isEmpty())
            {
                QDesktopServices::openUrl(QUrl(targetUrl));
            }
        }
        else
        {
            scaleAnimation->stop();
            scaleAnimation->setStartValue(currentScale);
            scaleAnimation->setEndValue(0.85);
            scaleAnimation->start();

            opacityAnimation->stop();
            opacityAnimation->setStartValue(currentOpacity);
            opacityAnimation->setEndValue(0.95);
            opacityAnimation->start();
        }
    }
    QLabel::mouseReleaseEvent(event);
}

inline void AnimatedHoverIcon::paintEvent(QPaintEvent *event)
{
    if (pixmap().isNull())
    {
        QLabel::paintEvent(event);
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QPointF center = rect().center();

    painter.translate(center);
    painter.scale(currentScale, currentScale);
    painter.translate(-center);

    QRect pixmapRect = rect();
    painter.drawPixmap(pixmapRect, pixmap());
}

#endif
