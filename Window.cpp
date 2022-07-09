#include "Window.h"

#include <imgui.h>
#include <QKeyEvent>
#include <QtImGui.h>

Window::Window(QWindow *parent)
    : QOpenGLWindow(QOpenGLWindow::UpdateBehavior::NoPartialUpdate, parent)
{
    QSurfaceFormat format;
    format.setMajorVersion(3);
    format.setMinorVersion(3);
    format.setSamples(16);
    format.setSwapInterval(1);
    setFormat(format);

    mCamera = new Camera;
    mCamera->setPosition(QVector3D(0, 20, 0));
    mCamera->setProjection(mProjection);

    mLight = new Light;
    mLight->setScale(QVector3D(0.005f, 0.005f, 0.005f));
    mLight->setPosition(QVector3D(5, 20, 35));

    mRendererManager = new RendererManager;
    mRendererManager->setCamera(mCamera);
    mRendererManager->setLight(mLight);

    resize(800, 600);

    connect(this, &QOpenGLWindow::frameSwapped, this, [=]() { update(); });
}

Window::~Window() {}

void Window::initializeGL()
{
    initializeOpenGLFunctions();
    QtImGui::initialize(this);

    mRendererManager->init();

    // Plane
    {
        mPlane = new Model;
        mPlane->setObjectName("Plane");
        mPlane->setType(Model::Plane);
        mPlane->setPosition(QVector3D(0, 0, 0));
        mModels << mPlane;
        mRendererManager->addModel(mPlane);
    }

    // Cube
    {
        mCube = new Model;
        mCube->setObjectName("Cube");
        mCube->setType(Model::Cube);
        mCube->material().setColor(QVector4D(1, 0, 0, 1));
        mCube->setScale(QVector3D(0.01f, 0.01f, 0.01f));
        mCube->setPosition(QVector3D(0, 2, 0));
        mModels << mCube;
        mRendererManager->addModel(mCube);
    }

    // Suzanne
    {
        mSuzanne = new Model;
        mSuzanne->setObjectName("Suzanne");
        mSuzanne->setType(Model::Suzanne);
        mSuzanne->material().setColor(QVector4D(1, 0, 1, 1));
        mSuzanne->setPosition(QVector3D(10, 2, 10));
        mModels << mSuzanne;
        mRendererManager->addModel(mSuzanne);
    }
}

void Window::resizeGL(int w, int h)
{
    mProjection.setToIdentity();
    mProjection.perspective(60, (float) (w) / h, 0.1, 10000);
    mCamera->setProjection(mProjection);
}

void Window::paintGL()
{
    mRendererManager->render();

    QtImGui::newFrame();
    ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);
    mImGuiWantsCapture = ImGui::GetIO().WantCaptureMouse;

    ImGui::Begin("Controls");
    int id = 0;

    // Light
    if (!ImGui::CollapsingHeader("Light"))
    {
        ImGui::PushID(id++);
        mLightParameters.position[0] = mLight->position().x();
        mLightParameters.position[1] = mLight->position().y();
        mLightParameters.position[2] = mLight->position().z();

        ImGui::SliderFloat("x", &mLightParameters.position[0], -100, 100);
        ImGui::SliderFloat("y", &mLightParameters.position[1], -100, 100);
        ImGui::SliderFloat("z", &mLightParameters.position[2], -100, 100);
        mLight->setPosition(QVector3D(mLightParameters.position[0], mLightParameters.position[1], mLightParameters.position[2]));

        mLightParameters.properties[0] = mLight->ambient();
        mLightParameters.properties[1] = mLight->diffuse();
        mLightParameters.properties[2] = mLight->specular();

        ImGui::SliderFloat("Ambient", &mLightParameters.properties[0], 0, 1);
        ImGui::SliderFloat("Diffuse", &mLightParameters.properties[1], 0, 1);
        ImGui::SliderFloat("Specular", &mLightParameters.properties[2], 0, 1);
        mLight->setAmbient(mLightParameters.properties[0]);
        mLight->setDiffuse(mLightParameters.properties[1]);
        mLight->setSpecular(mLightParameters.properties[2]);

        ImGui::PopID();
    }

    // Models
    for (int i = 0; i < mModels.size(); ++i)
    {
        if (ImGui::CollapsingHeader(mModels[i]->objectName().toStdString().c_str()))
        {
            ImGui::PushID(id++);

            // Position
            mModelParameters[i].position[0] = mModels[i]->position().x();
            mModelParameters[i].position[1] = mModels[i]->position().y();
            mModelParameters[i].position[2] = mModels[i]->position().z();
            ImGui::SliderFloat("x", &mModelParameters[i].position[0], -20, 20);
            ImGui::SliderFloat("y", &mModelParameters[i].position[1], -20, 20);
            ImGui::SliderFloat("z", &mModelParameters[i].position[2], -20, 20);
            mModels[i]->setPosition(QVector3D(mModelParameters[i].position[0], mModelParameters[i].position[1], mModelParameters[i].position[2]));

            // Material
            mModelParameters[i].properties[0] = mModels[i]->material().ambient();
            mModelParameters[i].properties[1] = mModels[i]->material().diffuse();
            mModelParameters[i].properties[2] = mModels[i]->material().specular();
            mModelParameters[i].properties[3] = mModels[i]->material().shininess();
            mModelParameters[i].color[0] = mModels[i]->material().color().x();
            mModelParameters[i].color[1] = mModels[i]->material().color().y();
            mModelParameters[i].color[2] = mModels[i]->material().color().z();
            mModelParameters[i].color[3] = mModels[i]->material().color().w();
            ImGui::SliderFloat("Ambient", &mModelParameters[i].properties[0], 0, 1);
            ImGui::SliderFloat("Diffuse", &mModelParameters[i].properties[1], 0, 1);
            ImGui::SliderFloat("Specular", &mModelParameters[i].properties[2], 0, 1);
            ImGui::SliderFloat("Shininess", &mModelParameters[i].properties[3], 1, 256);
            mModels[i]->material().setAmbient(mModelParameters[i].properties[0]);
            mModels[i]->material().setDiffuse(mModelParameters[i].properties[1]);
            mModels[i]->material().setSpecular(mModelParameters[i].properties[2]);
            mModels[i]->material().setShininess(mModelParameters[i].properties[3]);

            ImGui::ColorEdit3("Color", mModelParameters[i].color);
            mModels[i]->material().setColor(
                QVector4D(mModelParameters[i].color[0], mModelParameters[i].color[1], mModelParameters[i].color[2], mModelParameters[i].color[3]));

            ImGui::PopID();
        }
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0 / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    glViewport(0, 0, width(), height());
    ImGui::Render();
    QtImGui::render();
}

void Window::keyPressEvent(QKeyEvent *event)
{
    mCamera->onKeyPressed(event);
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
    mCamera->onKeyReleased(event);
}

void Window::mousePressEvent(QMouseEvent *event)
{
    mCamera->onMousePressed(event);
}

void Window::mouseReleaseEvent(QMouseEvent *event)
{
    mCamera->onMouseReleased(event);
}

void Window::mouseMoveEvent(QMouseEvent *event)
{
    if (mImGuiWantsCapture)
        return;

    mCamera->onMouseMoved(event);
}

void Window::wheelEvent(QWheelEvent *) {}
