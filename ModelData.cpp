#include "ModelData.h"

#include <QFile>
#include <QVector2D>

ModelData::ModelData(Model::Type type, QObject *parent)
    : QObject(parent)
    , mType(type)
{}

ModelData::~ModelData()
{
    mVertexArray.destroy();
    mVertexBuffer.destroy();
    mNormalBuffer.destroy();
    mTextureCoordsBuffer.destroy();
}

bool ModelData::load()
{
    qInfo() << Q_FUNC_INFO << "Loading model" << (int) mType;

    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    QVector<QVector2D> textureCoords;

    QFile file(MODEL_TO_PATH[mType]);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream fileText(&file);
        while (!fileText.atEnd())
        {
            QString fileLine = fileText.readLine();
            if (fileLine.startsWith("vn "))
            {
                QStringList lineList = fileLine.split(" ");
                normals << QVector3D(lineList[1].toFloat(), lineList[2].toFloat(), lineList[3].toFloat());
            }
            else if (fileLine.startsWith("vt "))
            {
                QStringList lineList = fileLine.split(" ");
                textureCoords << QVector2D(lineList[1].toFloat(), lineList[2].toFloat());
            }
            else if (fileLine.startsWith("v "))
            {
                QStringList lineList = fileLine.split(" ");
                vertices << QVector3D(lineList[1].toFloat(), lineList[2].toFloat(), lineList[3].toFloat());
            }
            else if (fileLine.startsWith("f "))
            {
                QStringList lineList = fileLine.split(" ");
                for (int i = 1; i <= 3; i++)
                {
                    QStringList arg = lineList[i].split("/");
                    if (arg.size() == 2)
                    {
                        if (arg[0].toInt() - 1 < vertices.size())
                            mVertices << vertices[arg[0].toInt() - 1];

                        if (arg[1].toInt() - 1 < normals.size())
                            mNormals << normals[arg[1].toInt() - 1];
                    }
                    else if (arg.size() == 3)
                    {
                        if (arg[0].toInt() - 1 < vertices.size())
                            mVertices << vertices[arg[0].toInt() - 1];

                        if (arg[1].toInt() - 1 < textureCoords.size())
                            mTextureCoords << textureCoords[arg[1].toInt() - 1];

                        if (arg[2].toInt() - 1 < normals.size())
                            mNormals << normals[arg[2].toInt() - 1];
                    }
                }
            }
            else if (fileLine.startsWith("mtllib "))
            {}
        }
        file.close();
        qInfo() << Q_FUNC_INFO << "Model" << (int) mType << "is loaded.";
        return true;
    }
    else
    {
        qWarning() << Q_FUNC_INFO << QString("Could not open file '%1'.").arg(file.fileName());
        qWarning() << Q_FUNC_INFO << "Could not load model" << (int) mType;
        return false;
    }
}

bool ModelData::create()
{
    qInfo() << Q_FUNC_INFO << "Creating VAO for model" << (int) mType;

    initializeOpenGLFunctions();

    mVertexArray.create();
    mVertexArray.bind();

    // Vertices
    mVertexBuffer.create();
    mVertexBuffer.bind();
    mVertexBuffer.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    mVertexBuffer.allocate(mVertices.constData(), sizeof(QVector3D) * mVertices.size());
    glVertexAttribPointer(0,
                          3,                 // Size
                          GL_FLOAT,          // Type
                          GL_FALSE,          // Normalized
                          sizeof(QVector3D), // Stride
                          nullptr            // Offset
    );
    glEnableVertexAttribArray(0);
    mVertexBuffer.release();

    // Normals
    mNormalBuffer.create();
    mNormalBuffer.bind();
    mNormalBuffer.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    mNormalBuffer.allocate(mNormals.constData(), sizeof(QVector3D) * mNormals.size());

    glVertexAttribPointer(1,
                          3,                 // Size
                          GL_FLOAT,          // Type
                          GL_FALSE,          // Normalized
                          sizeof(QVector3D), // Stride
                          nullptr            // Offset
    );
    glEnableVertexAttribArray(1);
    mNormalBuffer.release();

    // TextureCoords
    mTextureCoordsBuffer.create();
    mTextureCoordsBuffer.bind();
    mTextureCoordsBuffer.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    mTextureCoordsBuffer.allocate(mTextureCoords.constData(), sizeof(QVector2D) * mTextureCoords.size());
    glVertexAttribPointer(2,
                          2,                 // Size
                          GL_FLOAT,          // Type
                          GL_FALSE,          // Normalized
                          sizeof(QVector2D), // Stride
                          nullptr            // Offset
    );
    glEnableVertexAttribArray(2);
    mTextureCoordsBuffer.release();

    mVertexArray.release();

    qInfo() << Q_FUNC_INFO << "VAO for model" << (int) mType << "is created.";

    return true;
}

void ModelData::bind()
{
    mVertexArray.bind();
}

void ModelData::release()
{
    mVertexArray.release();
}

int ModelData::count()
{
    return mVertices.count();
}

const QString ModelData::ROOT_PATH = ":/Resources/Models/";

const QMap<Model::Type, QString> ModelData::MODEL_TO_PATH = { //
    {Model::Camera, ROOT_PATH + "Camera.obj"},
    {Model::Capsule, ROOT_PATH + "Capsule.obj"},
    {Model::Cone, ROOT_PATH + "Cone.obj"},
    {Model::Cube, ROOT_PATH + "Cube.obj"},
    {Model::Cylinder, ROOT_PATH + "Cylinder.obj"},
    {Model::Dome, ROOT_PATH + "Dome.obj"},
    {Model::Plane, ROOT_PATH + "Plane.obj"},
    {Model::Pyramid, ROOT_PATH + "Pyramid.obj"},
    {Model::Sphere, ROOT_PATH + "Sphere.obj"},
    {Model::Suzanne, ROOT_PATH + "Suzanne.obj"},
    {Model::Tetrahedron, ROOT_PATH + "Tetrahedron.obj"},
    {Model::Torus, ROOT_PATH + "Torus.obj"},
    {Model::TorusKnot, ROOT_PATH + "TorusKnot.obj"}};
