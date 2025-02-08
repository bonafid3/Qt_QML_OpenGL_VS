#include "ssao.h"
#include "utils.h"

#include <QOpenGLExtraFunctions>
#include <random>

SSAO::SSAO()
{
    initializeOpenGLFunctions();

    createSampleKernel();
    createNoiseData();
}

SSAO::~SSAO()
{
}

void SSAO::init(const QSize& dimensions)
{
    mMesh.loadMesh(":/meshes/plane.obj");
    mMesh.setShader(QOpenGLShader::Vertex, ":/shaders/ssao_vs.glsl");
    mMesh.setShader(QOpenGLShader::Fragment, ":/shaders/ssao_fs.glsl");
    mMesh.init();

    mPrePassShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/ssao_pre_pass_vs.glsl");
    mPrePassShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/ssao_pre_pass_fs.glsl");
    mPrePassShader.link();

    mSSAOShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/ssao_vs.glsl");
    mSSAOShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/ssao_fs.glsl");
    mSSAOShader.link();

    mSSAOBlurShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/ssao_vs.glsl");
    mSSAOBlurShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/ssao_blur_fs.glsl");
    mSSAOBlurShader.link();

    QOpenGLContext* ctx = QOpenGLContext::currentContext();
    QOpenGLExtraFunctions* ef = ctx->extraFunctions();

    // create FBOs
    //glGenFramebuffers(1, &mGeometryBufferFBO);
    //glGenFramebuffers(1, &mSSAOFBO);
	//glGenFramebuffers(1, &mSSAOBlurFBO);

    // create and initialize textures
    initTextures(dimensions);
}

void SSAO::createSampleKernel()
{
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    for (unsigned int i = 0; i < 64; ++i)
    {
        QVector3D sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample.normalize();
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;

        // scale samples s.t. they're more aligned to center of kernel
        scale = ssaoLerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        mSSAOKernel.push_back(sample);
    }
}

void SSAO::createNoiseData()
{
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    for (unsigned int i = 0; i < 16; i++)
    {
        QVector3D noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        mSSAONoise.push_back(noise);
    }
}

void SSAO::initTextures(const QSize& dimensions)
{
    qd << "initializing textures using dimensions: " << dimensions;
    QOpenGLContext* ctx = QOpenGLContext::currentContext();
    QOpenGLExtraFunctions* ef = ctx->extraFunctions();

    /*
    // Generate random position data for visual debugging
    QVector<float> randomData(dimensions.width() * dimensions.height() * 3);  // RGB format
    for (size_t i = 0; i < randomData.size(); ++i) {
        randomData[i] = static_cast<float>(rand()) / RAND_MAX;  // Random [0,1]
    }
    */

    // GBUFFER
    if (mGeometryBufferFBO != 0) {
        glDeleteFramebuffers(1, &mGeometryBufferFBO);
        mGeometryBufferFBO = 0;
    }
    glGenFramebuffers(1, &mGeometryBufferFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mGeometryBufferFBO);


    // delete old textures
    if (mGeometryBufferTexture != 0) {
        glDeleteTextures(1, &mGeometryBufferTexture);
        mGeometryBufferTexture = 0;
    }

    // G-Buffer using color attachment 0
    glGenTextures(1, &mGeometryBufferTexture);
    glBindTexture(GL_TEXTURE_2D, mGeometryBufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, dimensions.width(), dimensions.height(), 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mGeometryBufferTexture, 0);

    if (mNormalBufferTexture != 0) {
        glDeleteTextures(1, &mNormalBufferTexture);
        mNormalBufferTexture = 0;
    }

    // normal buffer using color attachment 1
    glGenTextures(1, &mNormalBufferTexture);
    glBindTexture(GL_TEXTURE_2D, mNormalBufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, dimensions.width(), dimensions.height(), 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mNormalBufferTexture, 0);

    if (mAlbedoTexture != 0) {
        glDeleteTextures(1, &mAlbedoTexture);
        mAlbedoTexture = 0;
    }

    // albedo buffer using color attachment 2
    glGenTextures(1, &mAlbedoTexture);
    glBindTexture(GL_TEXTURE_2D, mAlbedoTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimensions.width(), dimensions.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mAlbedoTexture, 0);

    if (mDepthBuffer != 0) {
        glDeleteRenderbuffers(1, &mDepthBuffer);
        mDepthBuffer = 0;
    }

    // depth buffer using depth attachment
    glGenRenderbuffers(1, &mDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, dimensions.width(), dimensions.height());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthBuffer);

    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    ef->glDrawBuffers(3, attachments);

    Q_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (mSSAOFBO != 0) {
        glDeleteFramebuffers(1, &mSSAOFBO);
        mSSAOFBO = 0;
    }
    glGenFramebuffers(1, &mSSAOFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mSSAOFBO);

    // SSAO RESULT
    if (mSSAOTexture != 0) {
        glDeleteTextures(1, &mSSAOTexture);
        mSSAOTexture = 0;
    }

    glGenTextures(1, &mSSAOTexture);
    glBindTexture(GL_TEXTURE_2D, mSSAOTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, dimensions.width(), dimensions.height(), 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mSSAOTexture, 0);
    Q_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // SSAO BLUR RESULT
    if (mSSAOBlurFBO != 0) {
        glDeleteFramebuffers(1, &mSSAOBlurFBO);
        mSSAOBlurFBO = 0;
    }
    glGenFramebuffers(1, &mSSAOBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mSSAOBlurFBO);

    if (mSSAOBlurTexture != 0) {
        glDeleteTextures(1, &mSSAOBlurTexture);
        mSSAOBlurTexture = 0;
    }

    glGenTextures(1, &mSSAOBlurTexture);
    glBindTexture(GL_TEXTURE_2D, mSSAOBlurTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, dimensions.width(), dimensions.height(), 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mSSAOBlurTexture, 0);
    Q_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // don't always need to regenerate the noise
    if (mNoiseTexture == 0) {
        glGenTextures(1, &mNoiseTexture);
        glBindTexture(GL_TEXTURE_2D, mNoiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &mSSAONoise[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }


}

// renders the entities into a textures
void SSAO::render(const QSize& dimensions, const QMatrix4x4& viewMatrix, const QMatrix4x4& projectionMatrix, const QVector<cMesh*>& entities, GLuint originalFBOHandle)
{
    QOpenGLContext* ctx = QOpenGLContext::currentContext();
    QOpenGLFunctions* f = ctx->functions();
    QOpenGLExtraFunctions* ef = ctx->extraFunctions();

    glViewport(0, 0, dimensions.width(), dimensions.height());
    glBindFramebuffer(GL_FRAMEBUFFER, mGeometryBufferFBO);
    glClearColor(0.33, 0.33, 0.33, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mPrePassShader.bind();

    for (auto&& entity : entities) {
        entity->vbo().bind();

        mPrePassShader.setUniformValue("qt_modelMatrix", entity->globalTransform());
        mPrePassShader.setUniformValue("qt_viewMatrix", viewMatrix);
        mPrePassShader.setUniformValue("qt_projectionMatrix", projectionMatrix);

        QMatrix4x4 modelViewMatrix = viewMatrix * entity->globalTransform();
        mPrePassShader.setUniformValue("qt_normalMatrix", modelViewMatrix.normalMatrix()); // inverse & transpose of world matrix

        f->glEnableVertexAttribArray(0);
        f->glEnableVertexAttribArray(1);
        f->glEnableVertexAttribArray(2);

        f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), nullptr);
        f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
        f->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), reinterpret_cast<void*>(6 * sizeof(GLfloat)));
        f->glDrawArrays(GL_TRIANGLES, 0, entity->vbo().size() / 9 / sizeof(GLfloat));

        entity->vbo().release();
    }
    
    mPrePassShader.release();

    glBindFramebuffer(GL_FRAMEBUFFER, originalFBOHandle); // important to bind the original FBO where the scene is drawn!
}

// renders the entities into a textures
void SSAO::renderSSAO(const QSize& dimensions, const QMatrix4x4& projectionMatrix, GLuint originalFBOHandle)
{
    QOpenGLContext* ctx = QOpenGLContext::currentContext();
    QOpenGLFunctions* f = ctx->functions();
    QOpenGLExtraFunctions* ef = ctx->extraFunctions();

    glViewport(0, 0, dimensions.width(), dimensions.height());
    glBindFramebuffer(GL_FRAMEBUFFER, mSSAOFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mSSAOShader.bind();

    mMesh.vbo().bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mGeometryBufferTexture); // bind the texture to the first texture unit

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mNormalBufferTexture); // bind the texture to the second texture unit

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mAlbedoTexture); // bind the texture to the third texture unit

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, mNoiseTexture); // bind the texture to the fourth texture unit

    mSSAOShader.setUniformValue("qt_projectionMatrix", projectionMatrix);

    for (int i = 0; i < 64; ++i) {
        QString uniformName = QString("samples[%1]").arg(i);
        mSSAOShader.setUniformValue(uniformName.toUtf8().constData(), mSSAOKernel[i]);
    }

    mSSAOShader.setUniformValue("qt_noiseScale", QVector2D(dimensions.width() / 4.0f, dimensions.height() / 4.0f));
    mSSAOShader.setUniformValue("qt_aspectRatio", static_cast<float>(dimensions.width()) / static_cast<float>(dimensions.height()));

    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glEnableVertexAttribArray(2);

    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), nullptr);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    f->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), reinterpret_cast<void*>(6 * sizeof(GLfloat)));
    f->glDrawArrays(GL_TRIANGLES, 0, mMesh.vbo().size() / 9 / sizeof(GLfloat));

    mMesh.vbo().release();

    mSSAOShader.release();

    glBindFramebuffer(GL_FRAMEBUFFER, originalFBOHandle); // important to bind the original FBO where the scene is drawn!
}

void SSAO::renderSSAOBlur(const QSize& dimensions, GLuint originalFBOHandle)
{
    QOpenGLContext* ctx = QOpenGLContext::currentContext();
    QOpenGLFunctions* f = ctx->functions();
    QOpenGLExtraFunctions* ef = ctx->extraFunctions();

    glViewport(0, 0, dimensions.width(), dimensions.height());
    glBindFramebuffer(GL_FRAMEBUFFER, mSSAOBlurFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mSSAOBlurShader.bind();

    mMesh.vbo().bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mSSAOTexture);

    //mSSAOBlurShader.setUniformValue("qt_aspectRatio", static_cast<float>(dimensions.width()) / static_cast<float>(dimensions.height()));
    mSSAOBlurShader.setUniformValue("qt_aspectRatio", 1.0f);

    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glEnableVertexAttribArray(2);

    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), nullptr);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    f->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), reinterpret_cast<void*>(6 * sizeof(GLfloat)));
    f->glDrawArrays(GL_TRIANGLES, 0, mMesh.vbo().size() / 9 / sizeof(GLfloat));

    mMesh.vbo().release();

    mSSAOBlurShader.release();

    glBindFramebuffer(GL_FRAMEBUFFER, originalFBOHandle); // important to bind the original FBO where the scene is drawn!
}

const GLuint& SSAO::geometryTexture() const
{
    return mGeometryBufferTexture;
}

const GLuint& SSAO::normalTexture() const
{
    return mNormalBufferTexture;
}

const GLuint& SSAO::albedoTexture() const
{
    return mAlbedoTexture;
}

const GLuint& SSAO::SSAOTexture() const
{
	return mSSAOTexture;
}

const GLuint& SSAO::SSAOBlurTexture() const
{
    return mSSAOBlurTexture;
}

const GLuint& SSAO::noiseTexture() const
{
    return mNoiseTexture;
}

