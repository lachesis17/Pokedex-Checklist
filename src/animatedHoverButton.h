#ifndef ANIMATEDHOVERBUTTON_H
#define ANIMATEDHOVERBUTTON_H

#include <QPushButton>
#include <QVariantAnimation>
#include <QColor>
#include <QEnterEvent>
#include <QEvent>

class AnimatedHoverButton : public QPushButton
{
public:
    explicit AnimatedHoverButton(const QString &text = "", QWidget *parent = nullptr);
    void restoreAnimatedState();
    void setHoverColor(const QColor &color) { hoverColor = color; }

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QVariantAnimation *animation;
    QColor originalColor;
    QColor hoverColor;
    double colorValue;

    void updateStyleSheet(double value);
    QColor interpolateColor(const QColor &colorStart, const QColor &colorEnd, double fraction);
};

inline AnimatedHoverButton::AnimatedHoverButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent), colorValue(0.0)
{
    animation = new QVariantAnimation(this);
    animation->setDuration(300);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    originalColor = palette().button().color();
    hoverColor = QColor("#990000");

    connect(animation, &QVariantAnimation::valueChanged, [this](const QVariant &value)
            {
        colorValue = value.toDouble();
        updateStyleSheet(colorValue); });

    updateStyleSheet(0.0);
}

inline void AnimatedHoverButton::restoreAnimatedState()
{
    updateStyleSheet(colorValue);
}

inline void AnimatedHoverButton::enterEvent(QEnterEvent *event)
{
    animation->stop();
    animation->setStartValue(colorValue);
    animation->setEndValue(1.0);
    animation->start();
    QPushButton::enterEvent(event);
}

inline void AnimatedHoverButton::leaveEvent(QEvent *event)
{
    animation->stop();
    animation->setStartValue(colorValue);
    animation->setEndValue(0.0);
    animation->start();
    QPushButton::leaveEvent(event);
}

inline void AnimatedHoverButton::updateStyleSheet(double value)
{
    QColor color = interpolateColor(originalColor, hoverColor, value);
    setStyleSheet(QString(
                      "QPushButton {"
                      "    background-color: %1;"
                      "    padding: 8px 16px;"
                      "    margin: 2px;"
                      "    border: 1px solid #999;"
                      "    border-radius: 6px;"
                      "    font-size: 12px;"
                      "    font-weight: bold;"
                      "}")
                      .arg(color.name()));
}

inline QColor AnimatedHoverButton::interpolateColor(const QColor &colorStart, const QColor &colorEnd, double fraction)
{
    int r = static_cast<int>(colorStart.red() + (colorEnd.red() - colorStart.red()) * fraction);
    int g = static_cast<int>(colorStart.green() + (colorEnd.green() - colorStart.green()) * fraction);
    int b = static_cast<int>(colorStart.blue() + (colorEnd.blue() - colorStart.blue()) * fraction);
    return QColor(r, g, b);
}

#endif
