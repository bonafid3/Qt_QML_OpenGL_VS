#include "skybox.h"

Skybox::Skybox()
{
    initializeOpenGLFunctions();
}

Skybox::~Skybox()
{
}

void Skybox::init()
{
	mMesh.loadMesh(":/meshes/skybox.obj");
	mMesh.setShader(QOpenGLShader::Vertex, ":/shaders/skybox_vs.glsl");
	mMesh.setShader(QOpenGLShader::Fragment, ":/shaders/skybox_fs.glsl");
	mMesh.localTransform().scale(500, 500, 500);

	mMesh.init();

	QStringList list;
	list << "px.png" << "nx.png" << "py.png" << "ny.png" << "pz.png" << "nz.png";

    setupTextures(list);
}

void Skybox::setupTextures(const QStringList& images)
{
    if (images.size() != 6)
        return;

    glGenTextures(1, &mSkyBoxTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mSkyBoxTexture);

    for (int i = 0; i < 6; ++i) {
        QImage img;
        qDebug() << "trying to load: " << images.at(i);
        img.load(":/cubemap/" + images.at(i));
        if (img.isNull()) {
            // error
            return;
        }
        //img = img.convertToFormat(QImage::Format_ARGB32);
        img = img.convertToFormat(QImage::Format_RGBA8888);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::render(const QMatrix4x4& viewMatrix, const QMatrix4x4& projectionMatrix, const QVector3D eye)
{
    auto ctx = QOpenGLContext::currentContext();
    QOpenGLFunctions* f = ctx->functions();

    glDepthMask(GL_FALSE);  // Disable depth writes

    mMesh.vbo().bind();
    mMesh.shader().bind();

    //mSkyBox.texture().bind(0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mSkyBoxTexture);

    mMesh.localTransform().setToIdentity();
    mMesh.localTransform().translate(eye);
    //mMesh.localTransform().scale(31, 31, 31);

    mMesh.shader().setUniformValue("qt_modelViewMatrix", viewMatrix * mMesh.localTransform());
    mMesh.shader().setUniformValue("qt_projectionMatrix", projectionMatrix);

    f = ctx->functions();
    f->glEnableVertexAttribArray(0);

    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), nullptr);
    glDrawArrays(GL_TRIANGLES, 0, mMesh.vbo().size() / 9 / sizeof(GLfloat));

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    mMesh.shader().release();
    mMesh.vbo().release();

    glDepthMask(GL_TRUE);  // Enable depth writes
}
