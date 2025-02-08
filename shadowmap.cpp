#include "shadowmap.h"

ShadowMap::ShadowMap()
{
    initializeOpenGLFunctions();
}

ShadowMap::~ShadowMap()
{
}

void ShadowMap::init()
{
    mMesh.loadMesh(":/meshes/plane.obj");
	mMesh.setShader(QOpenGLShader::Vertex, ":/shaders/shadowmap_vs.glsl");
	mMesh.setShader(QOpenGLShader::Fragment, ":/shaders/shadowmap_fs.glsl");

	mMesh.init();

    QOpenGLContext* ctx = QOpenGLContext::currentContext();

    glGenFramebuffers(1, &mShadowMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mShadowMapFBO);

    glGenTextures(1, &mShadowMapTexture);
    glBindTexture(GL_TEXTURE_2D, mShadowMapTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Set to maximum depth
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // attach texture to the frame buffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mShadowMapTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    Q_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, ctx->defaultFramebufferObject());
}

// renders the entities into a shadow map fbo's / depth attachment texture
void ShadowMap::render(const QSize& sceneSize, const QVector3D& lightObjPosition, const QVector<cMesh*>& entities, GLuint originalFBOHandle)
{
    // for the lightViewMatrix, we just automatically calculate the up vector
    QVector3D lightTarget(0, 0, 0);
    QVector3D forward = (lightTarget - lightObjPosition).normalized();
    QVector3D right = QVector3D::crossProduct(QVector3D(0, 0, -1), forward).normalized();
    QVector3D up = QVector3D::crossProduct(forward, right).normalized();
    QMatrix4x4 lightViewMatrix;
    lightViewMatrix.lookAt(lightObjPosition, lightTarget, up);

    QMatrix4x4 lightProjectionMatrix;
    int dim = 24;
    lightProjectionMatrix.ortho(-dim, dim, -dim, dim, ZNEAR, ZFAR);

    // here we calculate the shadowMapMatrix
    QMatrix4x4 offsetMatrix; // used to transform from hdc (-1 to 1) into (0 to 1) texture space
    offsetMatrix.translate(0.5f, 0.5f, 0.5f); // Apply 0.5 offset
    offsetMatrix.scale(0.5f, 0.5f, 0.5f);     // Apply 0.5 scaling
    mShadowMapMatrix = offsetMatrix * lightProjectionMatrix * lightViewMatrix;
    
    auto ctx = QOpenGLContext::currentContext();
    QOpenGLFunctions* f = ctx->functions();

    f->glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
    mMesh.shader().bind();

    // render the objects to the shadowmap fbo
    f->glBindFramebuffer(GL_FRAMEBUFFER, mShadowMapFBO);
    f->glClear(GL_DEPTH_BUFFER_BIT);

    //f->glCullFace(GL_FRONT);

    for (auto&& entity : entities) {
        entity->vbo().bind();
        mMesh.shader().setUniformValue("qt_modelViewProjectionMatrix", lightProjectionMatrix * lightViewMatrix * entity->globalTransform());

        f->glEnableVertexAttribArray(0);

        f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), nullptr);
        f->glDrawArrays(GL_TRIANGLES, 0, entity->vbo().size() / 9 / sizeof(GLfloat));

        entity->vbo().release();
    }

    //f->glCullFace(GL_BACK);

    f->glBindFramebuffer(GL_FRAMEBUFFER, originalFBOHandle); // important to bind the original FBO where the scene is drawn!

    mMesh.shader().release();
}

const GLuint& ShadowMap::shadowMapTexture() const
{
    return mShadowMapTexture;
}

const QMatrix4x4& ShadowMap::shadowMapMatrix() const
{
    return mShadowMapMatrix;
}
