//
// Created by asus on 2019/10/11.
//

#ifndef GLESTEST_NATIVE_LIB_H
#define GLESTEST_NATIVE_LIB_H

#include <GLES2/gl2.h>
#include "Matrix.h"
class Native1Lesson {

public:

    Native1Lesson();

    ~Native1Lesson();

    void create();

    void change(int width, int height);

    void draw();

    void drawTriangle(GLfloat *verticesData);

private:

    Matrix *mViewMatrix;
    Matrix *mModelMatrix;
    Matrix *mProjectionMatrix;
    Matrix *mMVPMatrix;

    GLuint mProgram;

    GLuint mMVPMatrixHandle;
    GLuint mPositionHandle;
    GLuint mColorHandle;


};

#endif //GLESTEST_NATIVE_LIB_H
