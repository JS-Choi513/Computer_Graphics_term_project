/* This program illustrates the use of the glut library for
interfacing with a window system */
#define _CRT_SECURE_NO_WARNINGS
#include <glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>

//사용자 정의 상수
#define NULL 0
#define LINE 1
#define RECTANGLE 2
#define TRIANGLE  3
#define POINTS 4
#define TEXT 5
//추가 기능 상수
#define CIRCLE 6
#define ERASER 7
#define SPRAY 8
#define DRAW 9 // 브러쉬
#define REMOVE 10

#define _CRT_SECURE_NO_WARNINGS
void mouse2(int, int);
void mouse(int, int, int, int);
void key(unsigned char, int, int);
void display(void);
void display2(void); //메뉴창만 초기화
void drawSquare(int, int);
void myReshape(GLsizei, GLsizei);

void myinit(void);

void screen_box(int, int, int);
void right_menu(int);
void middle_menu(int);
void color_menu(int);
void pixel_menu(int);
void fill_menu(int);
int pick(int, int);
void captureScreen();
void loadImage();
void line_menu(int); //라인 모양 메뉴
void random_color(); //랜덤 색상
void bcolor_menu(int); //배경 색상
void line_dotted(int); //점선
void draw_menu(int); //지우개, 브러쉬, 스프레이 등 draw 기능 설정
/* globals */

GLsizei wh = 500, ww = 500; /* initial window size */
GLfloat size = 3.0;   /* half side length of square */
int draw_mode = 0; /* drawing mode */
int rx, ry; /*raster position*/
int cursorX, cursorY;
int line = 0; //라인 메뉴 값
int ran_check = 0; //랜덤 색상 적용 유무
int erase = 1;
GLfloat r = 0.0, g = 0.0, b = 0.0; /* drawing color */
GLfloat r2 = 1.0, g2 = 1.0, b2 = 1.0; /* 배경 색상 */
int fill = 0; /* fill flag */

//draw 기능  반지름 설정 변수
GLfloat circle_r = 10.0;

//사각형 모양의 점 출력
void drawSquare(int x, int y)
{
    y = wh - y;
    //랜덤 색
    glColor3ub((char)rand() % 256, (char)rand() % 256, (char)rand() % 256);
    glBegin(GL_POLYGON);
    glVertex2f(x + size, y + size);
    glVertex2f(x - size, y + size);
    glVertex2f(x - size, y - size);
    glVertex2f(x + size, y - size);
    glEnd();
}


/* rehaping routine called whenever window is resized
or moved */
//창조절 크기
void myReshape(GLsizei w, GLsizei h)
{

    /* adjust clipping box */

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (GLdouble)w, 0.0, (GLdouble)h, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* adjust viewport and  clear */

    glViewport(0, 0, w, h);
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    display();
    glFlush();

    /* set global size for use by drawing routine */

    ww = w;
    wh = h;
}

void myinit(void)
{

    glViewport(0, 0, ww, wh);


    /* Pick 2D clipping window to match size of X window
    This choice avoids having to scale object coordinates
    each time window is resized */

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (GLdouble)ww, 0.0, (GLdouble)wh, -1.0, 1.0);

    /* set clear color to black and clear window */

    glClearColor(0.8, 0.8, 0.8, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}


//마우스 동작 저장
void mouse(int btn, int state, int x, int y)
{
    static int count;
    int where;
    static int xp[2], yp[2];
    //왼쪽 마우스 버튼이 눌려졌을 때
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS); //?
        if (ran_check == 1)	// 1이면 랜덤 색상 적용
            random_color();
        //마우스 왼쪽 버튼이 눌린  좌표 저장 
        where = pick(x, y);
        cursorX = x;
        cursorY = y;
        line_dotted(line); //점선 속성 설정
        glColor3f(r, g, b);
        glLineWidth(size); //두께 설정
        //눌린 위치가 메뉴 창인가 아닌가 판단
        if (where != 0)
        {
            count = 0;
            draw_mode = where;
            cursorX = 0;
            cursorY = 0;
        }
        //실제 동작
        else switch (draw_mode)
        {
            //case 1. Line인 경우
        case(LINE):
            //처음 마우스가 눌린 경우(첫좌표 저장)
            if (count == 0)
            {
                count++; //그래서 눌렸으니 ++
                xp[0] = x;
                yp[0] = y;
            }
            //두번째 눌렷을 경우
            else
            {
                glBegin(GL_LINES);
                //받아온 좌표와
                glVertex2i(x, wh - y);
                //저장된 자표를 이음
                glVertex2i(xp[0], wh - yp[0]);
                glEnd();
                draw_mode = 0;
                count = 0;
            }
            break;
            //사각형
        case(RECTANGLE):
            //첫좌표 저장
            if (count == 0)
            {
                count++;
                xp[0] = x;
                yp[0] = y;
            }
            else
            {
                //채움 사각형 그리는 케이스
                if (fill) glBegin(GL_POLYGON);
                //아니라면 선 4개를 그려서 그냥 사각형 그리기
                else glBegin(GL_LINE_LOOP);
                glVertex2i(x, wh - y);
                glVertex2i(x, wh - yp[0]);
                glVertex2i(xp[0], wh - yp[0]);
                glVertex2i(xp[0], wh - y);
                glEnd();
                draw_mode = 0;
                count = 0;
            }
            break;
            //사각형과 같음 대신 점 세개가 필요하니 count 1 2 추가
        case (TRIANGLE):
            switch (count)
            {
            case(0):
                count++;
                xp[0] = x;
                yp[0] = y;
                break;
            case(1):
                count++;
                xp[1] = x;
                yp[1] = y;
                break;
            case(2):
                if (fill) glBegin(GL_POLYGON);
                else glBegin(GL_LINE_LOOP);
                glVertex2i(xp[0], wh - yp[0]);
                glVertex2i(xp[1], wh - yp[1]);
                glVertex2i(x, wh - y);
                glEnd();
                draw_mode = 0;
                count = 0;
            }
            break;
            //위의 네모점 찍기 추가
        case(POINTS):
        {
            drawSquare(x, y);
            count++;
        }
        break;
        case(TEXT):
        {
            rx = x;
            ry = wh - y;
            glRasterPos2i(rx, ry);
            count = 0;
        }
        //원 그리기 기능
        case(CIRCLE):
        {
            if (count == 0) {
                count++;
                xp[0] = x;
                yp[0] = y;
                break;
            }
            else {
                double cx, cy, angle, i;
                //반지름 구하기
                double r = fabs(sqrt((xp[0] - x) * (xp[0] - x) + (yp[0] - y) * (yp[0] - y)));
                if (fill) glBegin(GL_POLYGON);
                else glBegin(GL_POINTS);
                for (i = 0.0; i <= 3600; i += 0.18) {
                    angle = i * 3.141592 / 180.0;
                    glVertex2f((r * cos(angle)) + xp[0], (r * sin(angle)) + wh - yp[0]);
                }
                glEnd();
            }
            display2();
            draw_mode = 0;
            count = 0;
            break;
        }
        //지우기 기능
        case(ERASER):
        {
            glColor3f(r2, g2, b2);
            y = wh - y;
            double i, angle;
            if (x < ww) {
                glBegin(GL_POLYGON);
                for (i = 0.0; i <= 3600; i += 0.036) {
                    angle = i * 3.141592 / 180.0;
                    glVertex2f((circle_r * cos(angle)) + x, (circle_r * sin(angle)) + y);
                }
            }
            glEnd();
            break;
        }
        case(REMOVE):                   // 잘라내기
            if (erase == 1)				// 변수가 1이면 지우개, 0이면 잘라내기
                break;
            glColor3f(r2, g2, b2);
            if (count == 0)
            {
                count++;
                xp[0] = x;
                yp[0] = y;
            }
            else
            {								// 배경 색상으로 사각형 그래서 잘라내기 효과
                glBegin(GL_POLYGON);
                glVertex2i(x, wh - y);
                glVertex2i(x, wh - yp[0]);
                glVertex2i(xp[0], wh - yp[0]);
                glVertex2i(xp[0], wh - y);
                glEnd();
                draw_mode = 0;
                count = 0;
            }
            break;
        }
        display2();
        glPopAttrib();
        glFlush();
    }
}
void mouse2(int x, int y) {
    int n_y = wh - y;
    double i, angle;

    switch (draw_mode) {
        //지우개 모드
    case(ERASER):
        glColor3f(r2, g2, b2);

        glBegin(GL_POLYGON);
        for (i = 0.0; i <= 3600; i += 0.036) {
            angle = i * 3.141592 / 180.0;
            glVertex2f((circle_r * cos(angle)) + x, (circle_r * sin(angle)) + n_y);
        }
        glEnd();
        display2();
        // 마우스 좌표 갱신
        cursorX = x;
        cursorY = y;
        break;
    //스프레이
    case(SPRAY):
        glColor3f(r, g, b);
        glPointSize(1.0);
        glBegin(GL_POINTS); 
        for (i = 0.0; i <= 360; i += 36) {
            angle = i * 3.141592 / 180.0;
            //mod 4를 통해 지정된 범위에서 점 찍기
            glVertex2f(rand() % 4 * (circle_r / 4 * cos(angle)) + x, rand() % 4 * (circle_r / 4 *sin(angle)) + n_y);
        }
        glEnd();
        display2(); //도형 선택창 침범 x
        // 마우스 좌표 갱신
        cursorX = x;
        cursorY = y;
        break;
    //브러쉬
    case(DRAW):
        glColor3f(r, g, b);

        glBegin(GL_POLYGON);
        for (i = 0.0; i <= 3600; i += 0.036) {
            angle = i * 3.141592 / 180.0;
            glVertex2f((circle_r * cos(angle)) + x, (circle_r * sin(angle)) + n_y);
        }
        glEnd();
        display2();
        cursorX = x;
        cursorY = y;

        break;

    }
    display2();
    glPopAttrib();
    glFlush();
}


//마우스가 특정 지점에서 클릭되면 해당 지점이 지시하는 값 선택
int pick(int x, int y)
{
    y = wh - y;
    if (y < wh - ww / 10) return 0;
    else if (x < ww / 10) return LINE;
    else if (x < 2 * ww / 10) return RECTANGLE;
    else if (x < 3 * ww / 10) return TRIANGLE;
    else if (x < 4 * ww / 10) return POINTS;
    else if (x < 5 * ww / 10) return TEXT;
    //필요한 메뉴들 추가
    else if (x < 6 * ww / 10) return CIRCLE;
    else if (x < 7 * ww / 10) return ERASER;
    else if (x < 8 * ww / 10) return SPRAY;
    else if (x < 9 * ww / 10) return DRAW;
    else if (x < 10 * ww / 110) return REMOVE;

    else return 0;
}

//사각형을 그리는 함수
void screen_box(int x, int y, int s)
{
    glBegin(GL_QUADS);
    glVertex2i(x, y);
    glVertex2i(x + s, y);
    glVertex2i(x + s, y + s);
    glVertex2i(x, y + s);
    glEnd();
}

//우측 마우스 버튼이 눌렸졌을 때의 행동
void right_menu(int id)
{
    if (id == 1) exit(0);
    else if (id == 2) display();
    else if (id == 3) captureScreen();
    else if (id == 4) loadImage();
}

//창을 띄우는 메뉴이기 때문에 마땅한 동작이 없는 함수
void middle_menu(int id)
{
    if (id == 7) draw_mode = 7;
}

void erase_menu(int id)
{
    erase = id;
}

//색을 지정하는 메뉴
void color_menu(int id)
{
    if (id == 1) { r = 1.0; g = 0.0; b = 0.0; }
    else if (id == 2) { r = 0.0; g = 1.0; b = 0.0; }
    else if (id == 3) { r = 0.0; g = 0.0; b = 1.0; }
    else if (id == 4) { r = 0.0; g = 1.0; b = 1.0; }
    else if (id == 5) { r = 1.0; g = 0.0; b = 1.0; }
    else if (id == 6) { r = 1.0; g = 1.0; b = 0.0; }
    else if (id == 7) { r = 1.0; g = 1.0; b = 1.0; }
    else if (id == 8) { r = 0.0; g = 0.0; b = 0.0; }

}

//글자크기를 조절하는 동작
void pixel_menu(int id)
{
    if (id == 1) size = 2 * size;
    else if (size > 1) size = size / 2;
}

//도형을 그릴때 채울지 말지 결정하는 플래그 설정
void fill_menu(int id)
{
    if (id == 1) fill = 1;
    else fill = 0;
}

//페인터의 텍스트 추가 기능
void key(unsigned char k, int xx, int yy)
{
    if (draw_mode != TEXT) return;
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2i(rx, ry);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, k);
    /*glutStrokeCharacter(GLUT_STROKE_ROMAN,i); */
    rx += glutBitmapWidth(GLUT_BITMAP_9_BY_15, k);

}
void random_color() //랜덤 컬러 생성
{
    r = (float)(rand() % 256) / 255;
    g = (float)(rand() % 256) / 255;
    b = (float)(rand() % 256) / 255;
}

void bcolor_menu(int id) { //배경색 지정 및 랜덤 설정
    if (id == 1) { r2 = 1.0; g2 = 0.0; b2 = 0.0; }
    else if (id == 2) { r2 = 0.0; g2 = 1.0; b2 = 0.0; }
    else if (id == 3) { r2 = 0.0; g2 = 0.0; b2 = 1.0; }
    else if (id == 4) { r2 = 0.0; g2 = 1.0; b2 = 1.0; }
    else if (id == 5) { r2 = 1.0; g2 = 0.0; b2 = 1.0; }
    else if (id == 6) { r2 = 1.0; g2 = 1.0; b2 = 0.0; }
    else if (id == 7) { r2 = 1.0; g2 = 1.0; b2 = 1.0; }
    else if (id == 8) { r2 = 0.0; g2 = 0.0; b2 = 0.0; }
    else if (id == 9) { r2 = 0.8; g2 = 0.8; b2 = 0.8; }
    else {
        r2 = (float)(rand() % 256) / 255;
        g2 = (float)(rand() % 256) / 255;
        b2 = (float)(rand() % 256) / 255;
    }
    display();
}

void draw_menu(int id) {
    if (id == 1) {
        if (circle_r < 20.0)
            circle_r += 3;
    }
    else {
        if(circle_r > 4)
            circle_r -= 3;
    }
}

void line_dotted(int id) //점선
{
    if (id == 1) {
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1, 0xAAAA);
    }
    else if (id == 2) {
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(2, 0xAAAA);
    }
    else if (id == 3) {
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(3, 0xAAAA);
    }
    else if (id == 4) {
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(4, 0xAAAA);
    }
    else if (id == 5) {
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1, 0x00FF);
    }
    else if (id == 6) {
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(2, 0x00FF);
    }
    else
        glDisable(GL_LINE_STIPPLE);
}

void line_menu(int id)
{
    line = id;
}

//윈도우에 메뉴 창 그리기
void display(void)
{
    int shift = 0;
    //기존 ui 사각형 5개
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glClearColor(r2, g2, b2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    //선
    glColor3f(1.0, 1.0, 1.0);
    screen_box(0, wh - ww / 10, ww / 10);
    //사각형

    glColor3f(1.0, 0.0, 0.0);
    screen_box(ww / 10, wh - ww / 10, ww / 10);
    //삼각형
    glColor3f(0.0, 1.0, 0.0);
    screen_box(2 * ww / 10, wh - ww / 10, ww / 10);
    //점
    glColor3f(0.0, 0.0, 1.0);
    screen_box(3 * ww / 10, wh - ww / 10, ww / 10);
    //텍스트
    glColor3f(1.0, 1.0, 0.0);
    screen_box(4 * ww / 10, wh - ww / 10, ww / 10);
    /*-------------------------------------------------------*/
    //ui 구현 - 메뉴 사각형 추가
    //원
    glColor3f(1.0, 1.0, 1.0);
    screen_box(5 * ww / 10, wh - ww / 10, ww / 10);
    //지우개
    glColor3f(0.0, 1.0, 1.0);
    screen_box(6 * ww / 10, wh - ww / 10, ww / 10);
    //스프레이
    glColor3f(1.0, 0.0, 1.0);
    screen_box(7 * ww / 10, wh - ww / 10, ww / 10);
    //브러쉬 메뉴 박스
    glColor3f(0.5, 0.1, 0.1);
    screen_box(8 * ww / 10, wh - ww / 10, ww / 10);
    /*-------------------------------------------------------*/

    //ui 사각형
    glColor3f(0.0, 0.0, 0.0);
    screen_box(ww / 10 + ww / 40, wh - ww / 10 + ww / 40, ww / 20);

    //ui지우개
    glColor3f(1.0, 1.0, 1.0);
    screen_box(6 * ww / 10 + ww / 33, wh - ww / 10 + ww / 50, ww / 25);
    glColor3f(0.0, 0.0, 0.0);
    screen_box(6 * ww / 10 + ww / 33, wh - ww / 10 + ww / 30, ww / 25);
    

    //ui 선
    glBegin(GL_LINES);
    glVertex2i(wh / 40, wh - ww / 20);
    glVertex2i(wh / 40 + ww / 20, wh - ww / 20);
    glEnd();
   
    glBegin(GL_TRIANGLES);
    glVertex2i(ww / 5 + ww / 40, wh - ww / 10 + ww / 40);
    glVertex2i(ww / 5 + ww / 20, wh - ww / 40);
    glVertex2i(ww / 5 + 3 * ww / 40, wh - ww / 10 + ww / 40);
    glEnd();
    glPointSize(3.0);
    glBegin(GL_POINTS);
    glVertex2i(3 * ww / 10 + ww / 20, wh - ww / 20);
    glEnd();
    glRasterPos2i(2 * ww / 5, wh - ww / 20);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'A');
    shift = glutBitmapWidth(GLUT_BITMAP_9_BY_15, 'A');
    glRasterPos2i(2 * ww / 5 + shift, wh - ww / 20);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'B');
    shift += glutBitmapWidth(GLUT_BITMAP_9_BY_15, 'B');
    glRasterPos2i(2 * ww / 5 + shift, wh - ww / 20);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'C');
    glEnd();
    //ui 구현 - 메뉴 사각형 내 아이콘 추가
    //원
    glBegin(GL_POLYGON);
    double i, angle;
    for (i = 0.0; i <= 3600; i += 3.6) {
        angle = i * 3.141592 / 180.0;
        glVertex2f((15 * cos(angle)) + ww / 2 + ww / 20, (15 * sin(angle)) + wh - ww / 10 + ww / 20);
    }
    glEnd();
    

    //스프레이
    glPointSize(1.0);
    glBegin(GL_POINTS);
    for (i = 0.0; i <= 3600; i += 9) {
        angle = i * 3.141592 / 180.0;
        glVertex2f(rand() % 15 * cos(angle) + ww / 2 + ww / 5 + ww / 20, rand() % 15 * sin(angle) + wh - ww / 10 + ww / 20);
    }
    glEnd();
    glFlush();
    glPopAttrib();
}
//메뉴창만 초기화
void display2(void) {
    int shift = 0;
    //기존 ui 사각형 5개

    glColor3f(1.0, 1.0, 1.0);
    screen_box(0, wh - ww / 10, ww / 10);
    glColor3f(1.0, 0.0, 0.0);
    screen_box(ww / 10, wh - ww / 10, ww / 10);
    glColor3f(0.0, 1.0, 0.0);
    screen_box(ww / 5, wh - ww / 10, ww / 10);
    glColor3f(0.0, 0.0, 1.0);
    screen_box(3 * ww / 10, wh - ww / 10, ww / 10);
    glColor3f(1.0, 1.0, 0.0);
    screen_box(2 * ww / 5, wh - ww / 10, ww / 10);
    /*-------------------------------------------------------*/
    //ui 구현 - 메뉴 사각형 추가

    glColor3f(1.0, 1.0, 1.0);
    screen_box(5 * ww / 10, wh - ww / 10, ww / 10);
    glColor3f(0.0, 1.0, 1.0);
    screen_box(6 * ww / 10, wh - ww / 10, ww / 10);
    glColor3f(1.0, 0.0, 1.0);
    screen_box(7 * ww / 10, wh - ww / 10, ww / 10);

    glColor3f(0.5, 0.1, 0.1);
    screen_box(8 * ww / 10, wh - ww / 10, ww / 10);
    /*-------------------------------------------------------*/
    //ui 사각형
    glColor3f(0.0, 0.0, 0.0);
    screen_box(ww / 10 + ww / 40, wh - ww / 10 + ww / 40, ww / 20);

    glColor3f(1.0, 1.0, 1.0);
    screen_box(6 * ww / 10 + ww / 33, wh - ww / 10 + ww / 50, ww / 25);
    glColor3f(0.0, 0.0, 0.0);
    screen_box(6 * ww / 10 + ww / 33, wh - ww / 10 + ww / 30, ww / 25);
    //ui 선
    glBegin(GL_LINES);
    glVertex2i(wh / 40, wh - ww / 20);
    glVertex2i(wh / 40 + ww / 20, wh - ww / 20);
    glEnd();
    //ui 삼각형
    glBegin(GL_TRIANGLES);
    glVertex2i(ww / 5 + ww / 40, wh - ww / 10 + ww / 40);
    glVertex2i(ww / 5 + ww / 20, wh - ww / 40);
    glVertex2i(ww / 5 + 3 * ww / 40, wh - ww / 10 + ww / 40);
    glEnd();
    glPointSize(3.0);
    glBegin(GL_POINTS);
    glVertex2i(3 * ww / 10 + ww / 20, wh - ww / 20);
    glEnd();
    glRasterPos2i(2 * ww / 5, wh - ww / 20);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'A');
    shift = glutBitmapWidth(GLUT_BITMAP_9_BY_15, 'A');
    glRasterPos2i(2 * ww / 5 + shift, wh - ww / 20);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'B');
    shift += glutBitmapWidth(GLUT_BITMAP_9_BY_15, 'B');
    glRasterPos2i(2 * ww / 5 + shift, wh - ww / 20);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'C');
    /*-------------------------------------------------------*/
    //ui 구현 - 메뉴 사각형 내 아이콘 추가
    //원

    glBegin(GL_POLYGON);
    double i, angle;
    for (i = 0.0; i <= 3600; i += 3.6) {
        angle = i * 3.141592 / 180.0;
        glVertex2f((15 * cos(angle)) + ww / 2 + ww / 20, (15 * sin(angle)) + wh - ww / 10 + ww / 20);
    }
    glEnd();
    //지우개

    //스프레이
    glPointSize(1.0);
    glBegin(GL_POINTS);
    for (i = 0.0; i <= 3600; i += 9) {
        angle = i * 3.141592 / 180.0;
        glVertex2f(rand() % 15 * cos(angle) + ww / 2 + ww / 5 + ww / 20, rand() % 15 * sin(angle) + wh - ww / 10 + ww / 20);
    }
    glEnd();
    /*-------------------------------------------------------*/
    glFlush();
    glPopAttrib();
    /*-------------------------------------------------------*/
}
//그림 캡처(이미지 저장)
void captureScreen() {

    BITMAPFILEHEADER bmpfile;
    BITMAPINFOHEADER bmpinfo;
    GLubyte* pixels = new GLubyte[sizeof(GLubyte) * ww * (wh - ww / 11) * 3]; // 저장할 픽셀 메모리 변수, 객체 생성(C++)
    // 1바이트*윈도우 넓이*(윈도우 높이-윈도우 넓이/11)*3?

    FILE* file = fopen("capture.bmp", "wb");//저장할 파일이름 지정
    glReadPixels(0, 0, ww, (wh - ww / 11), GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);      // 픽셀 읽기

    memset(&bmpfile, 0, sizeof(bmpfile)); // bmpfile 메모리에 저장하기 위해 초기화
    memset(&bmpinfo, 0, sizeof(bmpinfo)); // bmpinfo 메모리에 저장하기 위해 초기화
    bmpfile.bfType = 'MB';
    bmpfile.bfSize = sizeof(bmpfile) + sizeof(bmpinfo) + ww * (wh - ww / 11) * 3;		// 버퍼 사이즈
    bmpfile.bfOffBits = sizeof(bmpfile) + sizeof(bmpinfo);		// 파일에서 비트맵 데이터가 있는 위치

    bmpinfo.biSize = sizeof(bmpinfo); // 구조체 크기
    bmpinfo.biWidth = ww; // 이미지 가로
    bmpinfo.biHeight = (wh - ww / 11); // 이미지 세로
    bmpinfo.biPlanes = 1; // 플레인수
    bmpinfo.biBitCount = 24; // 비트수 
    bmpinfo.biSizeImage = ww * (wh - ww / 11) * 3; // 이미지의크기

    fwrite(&bmpfile, sizeof(bmpfile), 1, file);
    fwrite(&bmpinfo, sizeof(bmpinfo), 1, file);
    fwrite(pixels, sizeof(unsigned char), (wh - ww / 11) * ww * 3, file);		// 저장

    fclose(file);
    free(pixels);
}
//그림 로드
void loadImage() {
    BITMAPFILEHEADER bmpfile;//비트맵 파일헤더,정보 변수 
    BITMAPINFOHEADER bmpinfo;
    FILE* fp = fopen("capture.bmp", "rb");//파일스트림 읽기모드, capture.bmp라는 파일이름이 소스코드 경로에 존재해야함
    if (fp == NULL)return;
    fread(&bmpfile, sizeof(BITMAPFILEHEADER), 1, fp);//파일 읽기 
    fread(&bmpinfo, sizeof(BITMAPFILEHEADER), 1, fp);
    DWORD bitsize = bmpinfo.biSizeImage;//이미지 비트크기 
    GLubyte* bits = new GLubyte[sizeof(GLubyte) * bitsize * 3];//메모리 할당 객체 생성 
    fread(bits, 1, bitsize, fp);//bits에 파일정보 저장
    glEnableClientState(GL_VERTEX_ARRAY);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);         glDrawPixels(ww, wh - ww / 11, GL_BGR_EXT, GL_UNSIGNED_BYTE, bits);
    glFlush();
    glPopAttrib();
    fclose(fp);
}

int main(int argc, char** argv)
{
    int c_menu, p_menu, f_menu, b_menu, l_menu, e_menu;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("square");
    glutDisplayFunc(display);

    //color_menu를 작동시키는 메뉴를 glutCreateMenu로 생성 후 c_menu에 반환
    c_menu = glutCreateMenu(color_menu);
    //이하 1~8번은 함수에 들어갈 인덱스 파라미터
    glutAddMenuEntry("Red", 1);
    glutAddMenuEntry("Green", 2);
    glutAddMenuEntry("Blue", 3);
    glutAddMenuEntry("Cyan", 4);
    glutAddMenuEntry("Magenta", 5);
    glutAddMenuEntry("Yellow", 6);
    glutAddMenuEntry("White", 7);
    glutAddMenuEntry("Black", 8);

    //color_menu의 형태와 같음222
    p_menu = glutCreateMenu(pixel_menu);
    glutAddMenuEntry("increase pixel size", 1);
    glutAddMenuEntry("decrease pixel size", 2);

    //333
    f_menu = glutCreateMenu(fill_menu);
    glutAddMenuEntry("fill on", 1);
    glutAddMenuEntry("fill off", 2);

    b_menu = glutCreateMenu(bcolor_menu);
    glutAddMenuEntry("Red", 1);
    glutAddMenuEntry("Green", 2);
    glutAddMenuEntry("Blue", 3);
    glutAddMenuEntry("Cyan", 4);
    glutAddMenuEntry("Magenta", 5);
    glutAddMenuEntry("Yellow", 6);
    glutAddMenuEntry("White", 7);
    glutAddMenuEntry("Black", 8);
    glutAddMenuEntry("Default", 9);
    glutAddMenuEntry("Random Color", 10);

    l_menu = glutCreateMenu(line_menu);
    glutAddMenuEntry("Dotted Line 1", 1);
    glutAddMenuEntry("Dotted Line 2", 2);
    glutAddMenuEntry("Dotted Line 3", 3);
    glutAddMenuEntry("Dotted Line 4", 4);
    glutAddMenuEntry("Dotted Line 5", 5);
    glutAddMenuEntry("Dotted Line 6", 6);
    glutAddMenuEntry("Full Line", 7);

    p_menu = glutCreateMenu(draw_menu);
    glutAddMenuEntry("Increase Size", 1);
    glutAddMenuEntry("Decrease Size", 2);
    //오른쪽 마우스 버튼 눌릴시 나타나는 메뉴 창
    glutCreateMenu(right_menu);
    //이하 1~2번은 함수에 들어갈 인덱스 파라미터
    glutAddMenuEntry("quit", 1);
    glutAddMenuEntry("clear", 2);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    glutAddMenuEntry("save", 3);//저장
    glutAddMenuEntry("load", 4);

    //마우스 휠 버튼을 클릭시 나타나는 메뉴 창
    glutCreateMenu(middle_menu); //middle_menu는 창만 띄우기 때문에 따로 함수의 동작은 없음
     //메뉴창 아래에 위에서 정의한 메뉴들 창을 띄우는 창
    glutAddSubMenu("Colors", c_menu);
    glutAddSubMenu("Pixel Size", p_menu);
    glutAddSubMenu("Fill", f_menu);
    glutAddSubMenu("Background Color", b_menu);
    glutAddSubMenu("Line", l_menu);
    glutAddSubMenu("Draw Size", p_menu);
    glutAttachMenu(GLUT_MIDDLE_BUTTON);
 
    myinit();
    glutReshapeFunc(myReshape);
    glutKeyboardFunc(key);
    glutMouseFunc(mouse);
    //마우스 모션 함수
    glutMotionFunc(mouse2);
    glutMainLoop();

}
