#include "CrossSectionAnalyzerWidget.h"

#include <QDateTime>
#include <QDebug>
#include <QKeyEvent>

Canavar::Engine::CrossSectionAnalyzerWidget::CrossSectionAnalyzerWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    connect(this, &QOpenGLWidget::frameSwapped, [this]() { update(); });
}

void Canavar::Engine::CrossSectionAnalyzerWidget::initializeGL()
{
    qDebug() << "CrossSectionAnalyzerWidget::initializeGL: BEGIN";

    emit Initialize();

    qDebug() << "CrossSectionAnalyzerWidget::initializeGL: END";
}

void Canavar::Engine::CrossSectionAnalyzerWidget::resizeGL(int width, int height)
{
    emit Resize(width, height);
}

void Canavar::Engine::CrossSectionAnalyzerWidget::paintGL()
{
    emit Render();
}

void Canavar::Engine::CrossSectionAnalyzerWidget::keyPressEvent(QKeyEvent *event)
{
    emit KeyPressed(event);
}

void Canavar::Engine::CrossSectionAnalyzerWidget::keyReleaseEvent(QKeyEvent *event)
{
    emit KeyReleased(event);
}

void Canavar::Engine::CrossSectionAnalyzerWidget::mousePressEvent(QMouseEvent *event)
{
    emit MousePressed(event);
}

void Canavar::Engine::CrossSectionAnalyzerWidget::mouseReleaseEvent(QMouseEvent *event)
{
    emit MouseReleased(event);
}

void Canavar::Engine::CrossSectionAnalyzerWidget::mouseMoveEvent(QMouseEvent *event)
{
    emit MouseMoved(event);
}

void Canavar::Engine::CrossSectionAnalyzerWidget::wheelEvent(QWheelEvent *event)
{
    emit WheelMoved(event);
}
