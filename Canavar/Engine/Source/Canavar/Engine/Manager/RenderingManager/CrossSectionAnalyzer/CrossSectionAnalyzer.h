#pragma once

#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Manager/RenderingManager/CrossSectionAnalyzer/CrossSectionAnalyzerWidget.h"
#include "Canavar/Engine/Primitive/Plane.h"

#include <QMatrix4x4>
#include <QObject>
#include <QOpenGLExtraFunctions>

namespace Canavar::Engine
{
    class ManagerProvider;
    class NodeManager;
    class CameraManager;

    class CrossSectionAnalyzer : public QObject, protected QOpenGLExtraFunctions
    {
        Q_OBJECT
      public:
        CrossSectionAnalyzer() = default;

        void Initialize(ManagerProvider *pProvider);
        void RenderPlane();

        void ShowWidget();
        void CloseWidget();

      private:
        void UpdateCrossSectionParameters();
        void RenderCrossSection();

        NodeManager *mNodeManager;
        CameraManager *mCameraManager;
        CrossSectionAnalyzerWidget *mCrossSectionAnalyzerWidget;

        Plane *mPlane;
        Shader *mBasicShader;
        Shader *mCrossSectionShader;

        QMatrix4x4 mCrossSectionProjectionMatrix;
        QMatrix4x4 mCrossSectionViewMatrix;

        QVector3D mPlanePositionInner;
        QVector3D mPlaneScaleInner;

        DEFINE_MEMBER(QVector3D, PlanePosition, QVector3D(0, 0, 0));
        DEFINE_MEMBER(QVector3D, PlaneScale, QVector3D(1, 1, 1));
        DEFINE_MEMBER(QVector4D, PlaneColor, QVector4D(1, 0, 0, 0.25f));
        DEFINE_MEMBER(QVector4D, CrossSectionColor, QVector4D(1, 0, 0, 1));
    };

}