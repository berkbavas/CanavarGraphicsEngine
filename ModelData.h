#ifndef MODELDATA_H
#define MODELDATA_H

#include "Model.h"

#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>

class ModelData : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit ModelData(Model::Type type, QObject *parent = nullptr);
    ~ModelData();

    bool load();
    bool create();
    void bind();
    void release();
    int count();

private:
    Model::Type mType;
    QOpenGLVertexArrayObject mVertexArray;
    QOpenGLBuffer mVertexBuffer;
    QOpenGLBuffer mNormalBuffer;
    QOpenGLBuffer mTextureCoordsBuffer;
    QVector<QVector3D> mVertices;
    QVector<QVector3D> mNormals;
    QVector<QVector2D> mTextureCoords;

    static const QMap<Model::Type, QString> MODEL_TO_PATH;
    static const QString ROOT_PATH;
};

#endif // MODELDATA_H
