#ifndef WINDOW_H
#define WINDOW_H

#include "RendererManager.h"

#include <QOpenGLFunctions>
#include <QOpenGLWindow>

class Window : public QOpenGLWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    Window(QWindow *parent = nullptr);
    ~Window();

protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;
    virtual void keyPressEvent(QKeyEvent *) override;
    virtual void keyReleaseEvent(QKeyEvent *) override;
    virtual void mousePressEvent(QMouseEvent *) override;
    virtual void mouseReleaseEvent(QMouseEvent *) override;
    virtual void mouseMoveEvent(QMouseEvent *) override;
    virtual void wheelEvent(QWheelEvent *) override;

private:
    RendererManager *mRendererManager;
    QMatrix4x4 mProjection;
    Camera *mCamera;
    Light *mLight;

    QList<Model *> mModels;
    Model *mPlane;
    Model *mCube;
    Model *mSuzanne;

    struct ImGuiParameters
    {
        float position[3];
        float properties[4];
        float color[4];
    };
    bool mImGuiWantsCapture;
    ImGuiParameters mModelParameters[4];
    ImGuiParameters mLightParameters;
};
#endif // WINDOW_H
