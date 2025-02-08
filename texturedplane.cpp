#include "texturedplane.h"

TexturedPlane::TexturedPlane()
{
    initializeOpenGLFunctions();
}

TexturedPlane::~TexturedPlane()
{
}

void TexturedPlane::init()
{
	mMesh.loadMesh(":/meshes/plane.obj");
	mMesh.setShader(QOpenGLShader::Vertex, ":/shaders/textured_plane_vs.glsl");
	mMesh.setShader(QOpenGLShader::Fragment, ":/shaders/textured_plane_fs.glsl");

	mMesh.init();
}

void TexturedPlane::render(const QSize& sceneSize, const QMatrix4x4& viewMatrix, const QMatrix4x4& projectionMatrix, const GLuint textureHandle)
{
    auto ctx = QOpenGLContext::currentContext();
    QOpenGLFunctions* f = ctx->functions();

    mMesh.shader().bind();
    mMesh.vbo().bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureHandle); // bind the texture to the first texture unit

    float aspect = sceneSize.width() / static_cast<float>(sceneSize.height());

    float aspectCorrectedWidth = 0.5 / aspect;

    QMatrix4x4 offsetMatrix; // used to transform from hdc (-1 to 1) into (0 to 1) texture space
    offsetMatrix.translate(0.5f, 0.5f, 0.0f); // Apply 0.5 offset
    
    offsetMatrix.scale(0.5, 0.5f, 0.0f);     // Apply 0.5 scaling

    mMesh.shader().setUniformValue("qt_offsetMatrix", offsetMatrix);
    mMesh.shader().setUniformValue("qt_aspectRatio", aspect);

    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glEnableVertexAttribArray(2);

    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), nullptr);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    f->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), reinterpret_cast<void*>(6 * sizeof(GLfloat)));
    glDrawArrays(GL_TRIANGLES, 0, mMesh.vbo().size() / 9 / sizeof(GLfloat));

    glBindTexture(GL_TEXTURE_2D, 0);
    mMesh.vbo().release();
    mMesh.shader().release();

    glDepthMask(GL_TRUE);  // Enable depth writes
}
