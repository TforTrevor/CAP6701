#include "post_processing.hpp"

PostProcessing::PostProcessing(const unsigned int width, const unsigned int height)
{
    hdrFBOs.push(HDRFBO{ width, height });
    hdrFBOs.push(HDRFBO{ width, height });

    createQuad();
}

void PostProcessing::begin(const HDRFBO& inFBO)
{
    HDRFBO& frontFBO = hdrFBOs.front();

    glBindFramebuffer(GL_READ_FRAMEBUFFER, inFBO.frameBufferObject);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frontFBO.frameBufferObject);
    glBlitFramebuffer(0, 0, inFBO.WIDTH, inFBO.HEIGHT, 0, 0, frontFBO.WIDTH, frontFBO.HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void PostProcessing::render(std::shared_ptr<Shader> effect)
{
    HDRFBO& read = hdrFBOs.front();
    hdrFBOs.pop();
    HDRFBO& write = hdrFBOs.front();

    glBindFramebuffer(GL_FRAMEBUFFER, write.frameBufferObject);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    effect->bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, read.colorBuffer);
    effect->setUniform1f("exposure", 1.0f);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    hdrFBOs.push(read);
}

void PostProcessing::end()
{
    HDRFBO& fbo = hdrFBOs.front();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    copyShader.bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fbo.colorBuffer);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    copyShader.unbind();
}

void PostProcessing::createQuad()
{
    float quadVertices[] = {
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    //Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    //UV
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}