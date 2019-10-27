package com.example.java_gles;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
import android.opengl.GLES30;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.os.Bundle;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;


public class MainActivity extends Activity {
    public GLSurfaceView glView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // 去掉activity的标题，全屏显示
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        glView = new GLSurfaceView(this);
        glView.setRenderer(new SimpleRenderer());
        setContentView(glView);
    }

    @Override
    public void onResume() {
        super.onPause();
        glView.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
        glView.onPause();
    }
/*
    class SimpleRenderer implements Renderer {

        FloatBuffer vertices;
        ShortBuffer indices;

        public SimpleRenderer() {

            ByteBuffer byteBuffer = ByteBuffer.allocateDirect(4 * 2 * 4);
            byteBuffer.order(ByteOrder.nativeOrder());
            vertices = byteBuffer.asFloatBuffer();
            //定义四个点
            vertices.put( new float[] {  -80f,-120f,
                    80f,-120f,
                    -80f,120f,
                    80f,120f});
            ByteBuffer indicesBuffer = ByteBuffer.allocateDirect(6 * 2);
            indicesBuffer.order(ByteOrder.nativeOrder());
            indices = indicesBuffer.asShortBuffer();
            indices.put(new short[] { 0, 1, 2, 1, 2, 3 });
            //indices.flip() == indices.position(0)
            indices.flip();
            vertices.flip();
        }

        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            Log.d("GLSurfaceViewTest", "surface created");
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            Log.d("GLSurfaceViewTest", "surface changed: " + width + "x"
                    + height);
        }

        @Override
        public void onDrawFrame(GL10 gl) {
            //定义显示在屏幕上的什么位置(opengl 自动转换)
            gl.glViewport(0, 0, glView.getWidth(), glView.getHeight());
            // gl.glViewport(50, 50,430, 550);
            gl.glClearColor(0.0f,1.0f,0.0f,0.0f);
            gl.glClear(GL10.GL_COLOR_BUFFER_BIT);
            gl.glMatrixMode(GL10.GL_PROJECTION);
            gl.glLoadIdentity();
            //设置视锥体的大小，一个很扁的长方体
            gl.glOrthof(-160, 160, -240, 240, 1, -1);
            //颜色设置为红色
            gl.glColor4f(1, 0, 0, 1);
            gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
            gl.glVertexPointer( 2, GL10.GL_FLOAT, 0, vertices);
            gl.glDrawElements(GL10.GL_TRIANGLE_STRIP, 6, GL10.GL_UNSIGNED_SHORT, indices);
        }

    }
    */


    public class RectangleRender implements GLSurfaceView.Renderer {
        //一个Float占用4Byte
        private static final int BYTES_PER_FLOAT = 4;
        //顶点个数
        private static final int POSITION_COMPONENT_COUNT = 4;
        //顶点位置缓存
        private final FloatBuffer vertexBuffer;
        //顶点颜色缓存
        private final FloatBuffer colorBuffer;
        //顶点索引缓存
        private final ShortBuffer indicesBuffer;
        //渲染程序
        private int mProgram;

        //相机矩阵
        private final float[] mViewMatrix = new float[16];
        //投影矩阵
        private final float[] mProjectMatrix = new float[16];
        //最终变换矩阵
        private final float[] mMVPMatrix = new float[16];

        //返回属性变量的位置
        //变换矩阵
        private int uMatrixLocation;
        //位置
        private int aPositionLocation;
        //颜色
        private int aColorLocation;

        //四个顶点的位置参数
        private float rectangleCoords[] = {
                -0.5f, 0.5f, 0.0f,//top left
                -0.5f, -0.5f, 0.0f, // bottom left
                0.5f, -0.5f, 0.0f, // bottom right
                0.5f, 0.5f, 0.0f // top right
        };

        /**
         * 顶点索引
         */
        private short[] indices = {
                0, 1, 2, 0, 2, 3
        };

        //四个顶点的颜色参数
        private float color[] = {
                0.0f, 0.0f, 1.0f, 1.0f,//top left
                0.0f, 1.0f, 0.0f, 1.0f,// bottom left
                0.0f, 0.0f, 1.0f, 1.0f,// bottom right
                1.0f, 0.0f, 0.0f, 1.0f// top right
        };

        public RectangleRender() {
            //顶点位置相关
            //分配本地内存空间,每个浮点型占4字节空间；将坐标数据转换为FloatBuffer，用以传入给OpenGL ES程序
            vertexBuffer = ByteBuffer.allocateDirect(rectangleCoords.length * BYTES_PER_FLOAT)
                    .order(ByteOrder.nativeOrder())
                    .asFloatBuffer();
            vertexBuffer.put(rectangleCoords);
            vertexBuffer.position(0);

            //顶点颜色相关
            colorBuffer = ByteBuffer.allocateDirect(color.length * BYTES_PER_FLOAT)
                    .order(ByteOrder.nativeOrder())
                    .asFloatBuffer();
            colorBuffer.put(color);
            colorBuffer.position(0);

            //顶点索引相关
            indicesBuffer = ByteBuffer.allocateDirect(indices.length * 4)
                    .order(ByteOrder.nativeOrder())
                    .asShortBuffer();
            indicesBuffer.put(indices);
            indicesBuffer.position(0);
        }

        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            //将背景设置为白色
            GLES30.glClearColor(1.0f,1.0f,1.0f,1.0f);

            //编译顶点着色程序
            String vertexShaderStr = ResReadUtils.readResource(R.raw.vertex_simple_shade);
            int vertexShaderId = ShaderUtils.compileVertexShader(vertexShaderStr);
            //编译片段着色程序
            String fragmentShaderStr = ResReadUtils.readResource(R.raw.fragment_simple_shade);
            int fragmentShaderId = ShaderUtils.compileFragmentShader(fragmentShaderStr);
            //连接程序
            mProgram = ShaderUtils.linkProgram(vertexShaderId, fragmentShaderId);
            //在OpenGLES环境中使用程序
            GLES30.glUseProgram(mProgram);


            uMatrixLocation = GLES30.glGetUniformLocation(mProgram, "u_Matrix");
            aPositionLocation = GLES30.glGetAttribLocation(mProgram, "vPosition");
            aColorLocation = GLES30.glGetAttribLocation(mProgram, "aColor");
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            //设置绘制窗口
            GLES30.glViewport(0, 0, width, height);


            //相机和透视投影方式
            //计算宽高比
            float ratio=(float)width/height;
            //设置透视投影
            Matrix.frustumM(mProjectMatrix, 0, -ratio, ratio, -1, 1, 3, 7);
            //设置相机位置
            Matrix.setLookAtM(mViewMatrix, 0, 0, 0, 7.0f, 0f, 0f, 0f, 0f, 1.0f, 0.0f);
            //计算变换矩阵
            Matrix.multiplyMM(mMVPMatrix,0,mProjectMatrix,0,mViewMatrix,0);


        /*//正交投影方式
        final float aspectRatio = width > height ?
                (float) width / (float) height :
                (float) height / (float) width;
        if (width > height) {
            //横屏
            Matrix.orthoM(mMVPMatrix, 0, -aspectRatio, aspectRatio, -1f, 1f, -1f, 1f);
        } else {
            //竖屏
            Matrix.orthoM(mMVPMatrix, 0, -1f, 1f, -aspectRatio, aspectRatio, -1f, 1f);
        }*/
        }

        @Override
        public void onDrawFrame(GL10 gl) {
            //把颜色缓冲区设置为我们预设的颜色
            GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT);

            //将变换矩阵传入顶点渲染器
            GLES30.glUniformMatrix4fv(uMatrixLocation,1,false,mMVPMatrix,0);
            //准备坐标数据
            GLES30.glVertexAttribPointer(aPositionLocation, 3, GLES30.GL_FLOAT, false, 0, vertexBuffer);
            //启用顶点位置句柄
            GLES30.glEnableVertexAttribArray(aPositionLocation);

            //准备颜色数据
            GLES30.glVertexAttribPointer(aColorLocation, 4, GLES30.GL_FLOAT, false, 0, colorBuffer);
            //启用顶点颜色句柄
            GLES30.glEnableVertexAttribArray(aColorLocation);

            //绘制三角形
            GLES30.glDrawElements(GL10.GL_TRIANGLES, indices.length, GL10.GL_UNSIGNED_SHORT, indicesBuffer);

            //禁止顶点数组的句柄
            GLES30.glDisableVertexAttribArray(aPositionLocation);
            GLES30.glDisableVertexAttribArray(aColorLocation);
        }
    }
}