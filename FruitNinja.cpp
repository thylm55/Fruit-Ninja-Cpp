/* Module      : FruitNinja.cpp
 * Author      : Team 11 - Computer Graphics - INT3403
 * Email       : fruitninjateam@groups.facebook.com
 * Course      : Computer Graphics
 *
 * Description : This file contain main source code of Fruit Ninja game in C++
 *
 *
 * Date        : 9/27/2012
 *
 * History: Started in 9/27/2012
 * Revision      Date          Changed By
 * --------      ----------    ----------
 * 1.00 Alpha   9/27/2012     May Thy
 * 1.01 Alpha   10/24/2012    May Thy
 * 1.05 Alpha   12/1/2012     May Thy
 * First release.
 *
 */
 
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include "png.h"
#include "proto.h"

#define TEXTURE_LOAD_ERROR 0

using namespace std;

//data of polygons wrap around the flying objects
dataObject dataBanana     = {8,  37, 26,
                             4,  20, 43, 56, 59, 53, 47, 5,  0,   0,  0,  0,  0,  0,  0,
                            13,  4,  10, 24, 37, 58, 59, 18, 0,   0,  0,  0,  0,  0,  0,
                            0, 63, 63, 0, 63, 63, 0, 0};
                            
dataObject dataWatermelon = {9,  30, 28,
                             28, 33, 58, 48, 37, 23, 13, 5,  28,  0,  0,  0,  0,  0,  0,
                             58, 58, 15, 8,  6,  6,  10, 16, 58,  0,  0,  0,  0,  0,  0,
                             0, 63, 63, 0, 63, 63, 0, 0};

dataObject dataGrapes     = {9, 31, 33,
                            34, 38, 45, 52, 52, 44, 12, 10, 20, 0, 0, 0, 0, 0, 0,
                            1, 4, 16, 40, 48, 60, 61, 29, 9, 0, 0, 0, 0, 0, 0,
                            0, 63, 63, 0, 63, 63, 0, 0};
                            
dataObject dataPineapple  = { 9, 25, 24,
                             41, 59, 63, 36, 23, 13, 0,  0,  32,  0,  0,  0,  0,  0,  0,
                             63, 62, 40, 11, 0,  0,  12, 21, 56,  0,  0,  0,  0,  0,  0,
                             0, 63, 63, 0, 63, 63, 0, 0};
                            
dataObject dataStrawberry = {11, 35, 28,
                             31, 44, 52, 54, 52, 48, 39, 22, 12,  9, 16,  0,  0,  0,  0,
                             58, 53, 43, 20,  9,  5,  5, 15, 27, 41, 51,  0,  0,  0,  0,
                             0, 63, 63, 0, 63, 63, 0, 0};

dataObject dataBoom       = {11, 32, 32,
                            25, 39, 50, 53, 50, 42, 35, 27, 14, 11, 17, 0, 0, 0, 0,
                            10, 10, 18, 30, 42, 49, 53, 54, 41, 29, 16, 0, 0, 0, 0,
                            0, 63, 63, 0, 63, 63, 0, 0};

//Quad's coordinates wap around the flying objects
TheQuad textBlock = {250, 360, 360, 250, 145, 145, 165, 165};

Fruit Banana, Watermelon, Grapes, Pineapple, Strawberry;
Boom Nuclear;
StaticObject Alive, Dead, NinjaLost, Score;

//Build random function return a random number between min and max, step=step
double random(double min, double max, double step)
{
    long range = (long)((max - min)/step + 1);
    long rDMin = (long)(min/step);
    int randrD = rand() % range + rDMin;
    return (randrD*step);
}

//Build randomArray function to load random numbers into array[]
void randomArray(double array[], double min, double max){
    double number;
    for (int i=0; i<=1999; i++){
        number=random(min, max, 0.2);
        array[i]=number;
    }
}

//set random values for the game
void setRandom(){
    randomArray(arrVelocity, 90, 90);
    randomArray(arrAlphaLeft, 86, 89);
    randomArray(arrAlphaRight, 91, 94);
    randomArray(arrStartTime, 0, 100);
    randomArray(arrpX, 157, 357);
    randomArray(arrSpeed, 4, 5);
    for (int i=0; i<=1999; i++){
        double number=random(1, 5, 1);
        rF[i]=number;
    }
    for (int i=0; i<=1999; i++){
        arrContinueFly[i]=true;
    }
}

double roundIt(double number){
    const double sd = 10;
    return int(number*sd + (number<0? -0.5 : 0.5))/sd;
}

bool checkPoint(GLdouble ax, GLdouble ay, GLdouble bx, GLdouble by, GLdouble cx, GLdouble cy){ 
    double gt = (cx - ax)*(by - ay) - (cy - ay)*(bx - ax); 
   return gt<0?false:true; 
}

//pointInPolygon function returns true if mouse coordinates are in range of flying object polygon
bool pointInPolygon(GLdouble Pcx, GLdouble Pcy, dataObject D){
    int i, j; 
    bool s;
    for (i = 0; i <= D.points-1; i ++){ 
        j = (i+1)%D.points;
        s = checkPoint(D.x[i], D.y[i], D.x[j], D.y[j], Pcx, Pcy); 
        for (int k = 0; k <= D.points-1; k++) 
            if ((k != i) && (k != j)) 
                if (s != checkPoint(D.x[i], D.y[i], D.x[j], D.y[j], D.x[k], D.y[k]))
                    return false;
    }
   return true;
}

//This case designed for text block quad (instead flying object polygon)
bool pointInPolygon(GLdouble Pcx, GLdouble Pcy, TheQuad Q){
    bool s;
    int i, j;
    for (i=0; i<=3; i++){
        j = (i+1)%4;
        s = checkPoint(Q.x[i], Q.y[i], Q.x[j], Q.y[j], Pcx, Pcy);
        for (int k=0; k<=3; k++)
            if ((k!=i)&&(k!=j))
                if (s!=checkPoint(Q.x[i], Q.y[i], Q.x[j], Q.y[j], Q.x[k], Q.y[k]))
                    return false;
    }
    return true;
}

//startLevel(...) and setLevel(...):
//Design script for a level of game.
//Fruit number inscrease from 3 in level 1 and plus 2 in every next levels.
//Bomb increase from 0 and dependent on y = 0.5x;
int startLevel(int level){
    int fruitNumber = 2+level;
    int boomNumber = int(level*0.5);
    int totalObject = fruitNumber + boomNumber;
    if (level==1) return 0;
    else
        return startLevel(level-1)+totalObject;
}
 
void setLevel(int level){
    int fruitNumber = 2+level;
    int boomNumber = int(level*0.5);
    int totalObject = fruitNumber + boomNumber;
    if (boomNumber>0)
            for (int i=startLevel(level); i<=startLevel(level)+boomNumber-1; i++)
                Nuclear.randomFly(i, level*100);
    for (int i=startLevel(level)+boomNumber; i<=startLevel(level)+totalObject-1; i++){
        switch ((int)rF[i]){
            case 1: Banana.randomFly(i, level*100); break;
            case 2: Watermelon.randomFly(i, level*100); break;
            case 3: Grapes.randomFly(i, level*100); break;
            case 4: Pineapple.randomFly(i, level*100); break;
            case 5: Strawberry.randomFly(i, level*100); break;
        }
    }
}

//Loading PNG Texure function
GLuint loadTexture(const string filename, int &width, int &height){
   //header for testing if it is a png
   png_byte header[8];
   //open file as binary
   FILE *fp = fopen(filename.c_str(), "rb");
   if (!fp) {
     return TEXTURE_LOAD_ERROR;
   }
   //read the header
   fread(header, 1, 8, fp);
   //test if png
   int is_png = !png_sig_cmp(header, 0, 8);
   if (!is_png) {
     fclose(fp);
     return TEXTURE_LOAD_ERROR;
   }
   //create png struct
   png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
       NULL, NULL);
   if (!png_ptr) {
     fclose(fp);
     return (TEXTURE_LOAD_ERROR);
   }
   //create png info struct
   png_infop info_ptr = png_create_info_struct(png_ptr);
   if (!info_ptr) {
     png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
     fclose(fp);
     return (TEXTURE_LOAD_ERROR);
   }
   //create png info struct
   png_infop end_info = png_create_info_struct(png_ptr);
   if (!end_info) {
     png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
     fclose(fp);
     return (TEXTURE_LOAD_ERROR);
   }
   //png error stuff, not sure libpng man suggests this.
   if (setjmp(png_jmpbuf(png_ptr))) {
     png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
     fclose(fp);
     return (TEXTURE_LOAD_ERROR);
   }
   //init png reading
   png_init_io(png_ptr, fp);
   //let libpng know you already read the first 8 bytes
   png_set_sig_bytes(png_ptr, 8);
   // read all the info up to the image data
   png_read_info(png_ptr, info_ptr);
   //variables to pass to get info
   int bit_depth, color_type;
   png_uint_32 twidth, theight;
   // get info about png
   png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
       NULL, NULL, NULL);
   //update width and height based on png info
   width = twidth;
   height = theight;
   // Update the png info struct.
   png_read_update_info(png_ptr, info_ptr);
   // Row size in bytes.
   int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
   // Allocate the image_data as a big block, to be given to opengl
   png_byte *image_data = new png_byte[rowbytes * height];
   if (!image_data) {
     //clean up memory and close stuff
     png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
     fclose(fp);
     return TEXTURE_LOAD_ERROR;
   }
   //row_pointers is for pointing to image_data for reading the png with libpng
   png_bytep *row_pointers = new png_bytep[height];
   if (!row_pointers) {
     //clean up memory and close stuff
     png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
     delete[] image_data;
     fclose(fp);
     return TEXTURE_LOAD_ERROR;
   }
   // set the individual row_pointers to point at the correct offsets of image_data
   for (int i = 0; i < height; ++i)
     row_pointers[height - 1 - i] = image_data + i * rowbytes;
   //read the png into image_data through row_pointers
   png_read_image(png_ptr, row_pointers);
   //Now generate the OpenGL texture object
   GLuint texture;
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);
   glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, width, height, 0,
       GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) image_data);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   //clean up memory and close stuff
   png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
   delete[] image_data;
   delete[] row_pointers;
   fclose(fp);
   return texture;
}

//Draw texture in a quad wrap around flying object
void DrawTexture(GLdouble quad[2][4], GLuint textureid) {
        //int textw,texth;
        // tell opengl to use the generated texture name
        // Make some OpenGL properties better for 2D and enable alpha channel.
        glBindTexture(GL_TEXTURE_2D, textureid);
        glEnable(GL_TEXTURE_2D);
        glColor4ub(255, 255, 255, 255);
        // make a rectangle
        glBegin(GL_QUADS);
        // bottom left
        glTexCoord2i(0, 1);
        glVertex2f(quad[0][0], quad[1][0]);
        // bottom right
        glTexCoord2i(1, 1);
        glVertex2f(quad[0][1], quad[1][1]);
        // top right
        glTexCoord2i(1, 0);
        glVertex2f(quad[0][2], quad[1][2]);
        // top left
        glTexCoord2i(0, 0);
        glVertex2f(quad[0][3], quad[1][3]);
        glEnd();
        glDisable(GL_TEXTURE_2D );
}

//Draw life status in the upper right corner
void drawLifes(int life, int MAXlife){
    GLdouble pX = 480;
    GLdouble pY = 430;
    if (life==MAXlife)
        for (int i=1; i<=MAXlife; i++)
            Alive.draw(pX+35*i, pY);
    else{
        for (int i=1; i<=MAXlife-life; i++)
            Dead.draw(pX+35*i, pY);
        for (int j=MAXlife-life+1; j<=MAXlife; j++)
            Alive.draw(pX+35*j, pY);
    }
    Score.draw(40, 430);
}

//Print a text into the window with defined format
void printText(GLvoid *font_style, GLdouble pX, GLdouble pY, char* format, ...){
    va_list arg_list;
    char str[256];
	int i;
    
    va_start(arg_list, format);
    vsprintf(str, format, arg_list);
    va_end(arg_list);
    
    glRasterPos2f (pX, pY);

    for (i = 0; str[i] != '\0'; i++)
        glutBitmapCharacter(font_style, str[i]);
}

//Draw knife streak when player holding down the mouse
void drawKnifeStreak(){
    glColor3f(0.929, 0.118, 0.475);
    glBegin(GL_LINE_STRIP);
        if ((mouseX!=0)&&(mouseY!=0)){
            mouseArr[im][0]=mouseX;
            mouseArr[im][1]=WindowSizeY-mouseY;
            if (im<=maxKnife-1)
                for (jm=0; jm<=im; jm++)
                    glVertex2f(mouseArr[jm][0], mouseArr[jm][1]);
            if (im>maxKnife-1)
                for (jm=im-maxKnife+1; jm<=im-1; jm++)
                    glVertex2f(mouseArr[jm][0], mouseArr[jm][1]);
            im++;
            if (mouseUp){
                for (int jm=0; jm<=im-1; jm++){
                    mouseArr[jm][0]=mouseX;
                    mouseArr[jm][1]=WindowSizeY-mouseY;
                }
                im=0;
                mouseX=0;
                mouseY=0;
            }
        }
    glEnd();
}

//Draw Game Over Board after life = 0
void drawGameoverBoard(){
    glColor3f(1.0, 0.0, 0.0);
    gameStop=true;
    printText(GLUT_BITMAP_TIMES_ROMAN_24, 240, 260, "%s", "GAME OVER");
    printText(GLUT_BITMAP_TIMES_ROMAN_24, 240, 220, "%s: %i", "Your score", point);
    if ((mouseXD>=250)&&(mouseXD<=360)&&(480-mouseYD>=145)&&(480-mouseYD<=165))
        glColor3f(1.000, 0.498, 0.165);
    else
        glColor3f(0.44, 0.16, 0.62);
    printText(GLUT_BITMAP_9_BY_15, 260, 150, "%s", "PLAY AGAIN");
    //slide to start over
    if (pointInPolygon(mouseX, 480-mouseY, textBlock)){
        life=MAXlife;
        timer=0;
        point=0;
        gameStop=false;
        setRandom();
    }
    NinjaLost.draw(500, 10);
}

//Setting up background for the game
void setBackground(const string fileName){
    GLdouble edge[2][4] = {0, 640, 640, 0, 480, 480, 0, 0};
    GLuint myglu;
    int Width, Height;
    myglu = loadTexture(fileName, Width, Height);
    DrawTexture(edge, myglu);
}

void setWindow(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(left, right, bottom, top);
}

void setViewport(GLint left, GLint right, GLint bottom, GLint top){
    glViewport(left, bottom, right - left, top - bottom);
}

void EnableTransparency(){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}  

void Optimize2D(){
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_ALPHA_TEST);
}

void myInit(void){
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glColor3f(0.0, 0.0, 0.0);
    glPointSize(1.0);
    setWindow(0.0, 640.0, 0.0, 480.0);
    glShadeModel(GL_FLAT);
}

//Display function
void myDisplay(void){
    glClear(GL_COLOR_BUFFER_BIT);
    Optimize2D();
    //setBackground("bg2.png");
    WindowSizeX = glutGet(GLUT_WINDOW_WIDTH);
    WindowSizeY = glutGet(GLUT_WINDOW_HEIGHT);
    glColor3f(0.035, 0.369, 0.035);
    printText(GLUT_BITMAP_HELVETICA_18, 20, 440, "%i", point);
    drawLifes(life, MAXlife);
    //***This game for demo in 15 levels***
    for (int i=1; i<=15; i++)
        setLevel(i);
    if (life<0)
        life=0;
    if (life==0)
        drawGameoverBoard();
    drawKnifeStreak();
    glutSwapBuffers();
}

//Make the animation for the game
void animation(int){
    if (life>0){
        timer = timer + 0.1;
        spin = spin + 0.1;
    }
    glutPostRedisplay();
    glutTimerFunc(5, animation, 0);
}

void reshape(int w, int h){
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    setWindow(0.0, glutGet(GLUT_WINDOW_WIDTH), 0.0, glutGet(GLUT_WINDOW_HEIGHT));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
    
void processMouseActiveMotion(int x, int y){
    mouseUp=false;
    mouseX=x;
    mouseY=y;
    glutPostRedisplay();
}

void processMousePassiveMotion(int xD, int yD){
    mouseDown=false;
    mouseXD=xD;
    mouseYD=yD;
    glutPostRedisplay();
}

void processMouse(int button, int state, int x, int y){
    if(state == GLUT_UP)
        if(button == GLUT_LEFT_BUTTON)
            mouseUp=true;
    if(state == GLUT_DOWN)
        if(button == GLUT_LEFT_BUTTON)
            mouseUp=false;
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Fruit Ninja - Fruit mine is opening!!!");
    myInit();
    EnableTransparency();
    life = MAXlife;
    srand(time(NULL));
    setRandom();
    
    Banana.inputData(dataBanana, "banana.png");
    Watermelon.inputData(dataWatermelon, "watermelon.png");
    Grapes.inputData(dataGrapes, "grapes.png");
    Pineapple.inputData(dataPineapple, "pineapple.png");
    Strawberry.inputData(dataStrawberry, "strawberry.png");
    Nuclear.inputData(dataBoom, "bomb.png");
    Alive.inputData("x_unfilled.png", 32);
    Dead.inputData("x_filled.png", 32);
    NinjaLost.inputData("NinjaLost2.png", 100);
    Score.inputData("fruit_cut.png", 32);
    
    glutDisplayFunc(myDisplay);
    glutMouseFunc(processMouse);
    glutMotionFunc(processMouseActiveMotion);
    glutPassiveMotionFunc(processMousePassiveMotion);
    glutTimerFunc(5, animation, 0);
    glutReshapeFunc(reshape);
    glutMainLoop();
}

//set input for the data of FlyingObject
void FlyingObject::inputData(dataObject O, char* fName){
    F = O;
    fileName = fName;
    int Width, Height;
    myglu = loadTexture(fileName, Width, Height);
}

//set input for the data of StaticObject
void StaticObject::inputData(char* fName, GLdouble Size){
    size = Size;
    fileName = fName;
    int Width, Height;
    myglu = loadTexture(fileName, Width, Height);
}

//get needed random values to make independent flying object
void FlyingObject::getRandom(long position){
    velocity=arrVelocity[position];
    startTime2=arrStartTime[position];
    pX=arrpX[position];
    pY=-63;
    speed=arrSpeed[position];
    if (arrpX[position]<257)
        alpha=arrAlphaLeft[position];
    if (arrpX[position]>257)
        alpha=arrAlphaRight[position];
}

//Flying object drawing function
void FlyingObject::draw(){
    DrawTexture(newF.quad, myglu);
}

//Static object drawing function
void StaticObject::draw(GLdouble pX, GLdouble pY){
    GLdouble quads[2][4];
    quads[0][0] = pX;        quads[1][0] = pY+size-1;
    quads[0][1] = pX+size-1; quads[1][1] = pY+size-1;
    quads[0][2] = pX+size-1; quads[1][2] = pY;
    quads[0][3] = pX;        quads[1][3] = pY;
    DrawTexture(quads, myglu);
}

//Calculating new position while object flying
void FlyingObject::getNewPosition(long position, double startTime1){
    getRandom(position);
    double startTime=startTime1+startTime2;
    double alphaR = alpha*PI/180;
    GLdouble R;
    double beta;
    newF.points=F.points;
    for (int i=0; i<=14; i++){
        newF.x[i]=0;
        newF.y[i]=0;
    }
    newF.centralX = 0;
    newF.centralY = 0;
    for (int i=0; i<=1; i++)
        for (int j=0; j<=3; j++)
            newF.quad[i][j]=0;
    if (timer>=startTime){
        if ((alpha>0)&&(alpha<90)){
            newF.centralX=F.centralX+pX+timer-startTime;
            for (int i=0; i<=F.points-1; i++)
                newF.x[i]=F.x[i]+pX+timer-startTime;
            for (int j=0; j<=3; j++)
                newF.quad[0][j]=F.quad[0][j]+pX+timer-startTime;
            newF.centralY=F.centralY-63+(tan(alphaR)*(timer-startTime)-(gravity*(timer-startTime)
            *(timer-startTime))/(2*velocity*velocity*cos(alphaR)*cos(alphaR)));
            arrCentralY[position]=newF.centralY;
            for (int i=0; i<=F.points-1; i++){
                newF.y[i]=F.y[i]-63+(tan(alphaR)*(timer-startTime)-(gravity*(timer-startTime)
                *(timer-startTime))/(2*velocity*velocity*cos(alphaR)*cos(alphaR)));
            }
            for (int j=0; j<=3; j++)
                newF.quad[1][j]=F.quad[1][j]-63+(tan(alphaR)*(timer-startTime)-(gravity*(timer-startTime)
                *(timer-startTime))/(2*velocity*velocity*cos(alphaR)*cos(alphaR)));
            arrEndX[position]=F.centralX+pX+tan(alphaR)*2*velocity*velocity*cos(alphaR)*cos(alphaR)/gravity;
        }
        if ((alpha>90)&&(alpha<180)){
            newF.centralX=F.centralX+pX-timer+startTime;
            for (int i=0; i<=F.points-1; i++)
                newF.x[i]=F.x[i]+pX-timer+startTime;
            for (int j=0; j<=3; j++)
                newF.quad[0][j]=F.quad[0][j]+pX-timer+startTime;
            newF.centralY=F.centralY-63+(-tan(alphaR)*(timer-startTime)-(gravity*(timer-startTime)
            *(timer-startTime))/(2*velocity*velocity*cos(alphaR)*cos(alphaR)));
            arrCentralY[position]=newF.centralY;
            for (int i=0; i<=F.points-1; i++){
                newF.y[i]=F.y[i]-63+(-tan(alphaR)*(timer-startTime)-(gravity*(timer-startTime)
                *(timer-startTime))/(2*velocity*velocity*cos(alphaR)*cos(alphaR)));
            }
            for (int j=0; j<=3; j++)
                newF.quad[1][j]=F.quad[1][j]-63+(-tan(alphaR)*(timer-startTime)-(gravity*(timer-startTime)
                *(timer-startTime))/(2*velocity*velocity*cos(alphaR)*cos(alphaR)));
            arrEndX[position]=F.centralX+pX+tan(alphaR)*2*velocity*velocity*cos(alphaR)*cos(alphaR)/gravity;
        }
        for (int i=0; i<=F.points-1; i++){
            beta = atan2(newF.centralY-newF.y[i], newF.centralX-newF.x[i]);
            R=sqrt((newF.centralX-newF.x[i])*(newF.centralX-newF.x[i])
            +(newF.centralY-newF.y[i])*(newF.centralY-newF.y[i]));
            newF.x[i]=newF.centralX+R*cos(speed*spin*PI/180+beta+PI);
            newF.y[i]=newF.centralY+R*sin(speed*spin*PI/180+beta+PI);
        }
        for (int j=0; j<=3; j++){
            beta = atan2(newF.centralY-newF.quad[1][j], newF.centralX-newF.quad[0][j]);
            R=sqrt((newF.centralX-newF.quad[0][j])*(newF.centralX-newF.quad[0][j])
            +(newF.centralY-newF.quad[1][j])*(newF.centralY-newF.quad[1][j]));
            newF.quad[0][j]=newF.centralX+R*cos(speed*spin*PI/180+beta+PI);
            newF.quad[1][j]=newF.centralY+R*sin(speed*spin*PI/180+beta+PI);
        }
    }
    else
      newF.centralY=0.0;
}

//Function that make object fly
void FlyingObject::randomFly(long position, double startTime1){
    if ((arrContinueFly[position]==true)&&(timer>=startTime1+arrStartTime[position])&&(arrCentralY[position]>=-63)){
        getNewPosition(position, startTime1);
        if ((newF.centralY>=1e-5)&&(newF.centralY<=1e+5))
            if (pointInPolygon(mouseX, WindowSizeY-mouseY, newF)){
                arrContinueFly[position]=false;
            }
        draw();
    }
}

//Function that make fruit fly
void Fruit::randomFly(long position, double startTime1){
    if ((arrContinueFly[position]==true)&&(timer>=startTime1+arrStartTime[position])&&(arrCentralY[position]>=-63)){
        getNewPosition(position, startTime1);
        if ((newF.centralY>=1e-5)&&(newF.centralY<=1e+5))
            if (pointInPolygon(mouseX, WindowSizeY-mouseY, newF)){
                arrContinueFly[position]=false;
                if (gameStop==false)
                    point++;
                else arrContinueFly[position]=true;
            }
        draw();
        if ((roundIt(arrEndX[position])!=0)
        &&((roundIt(arrEndX[position])-newF.centralX)<=1e-4)&&((roundIt(arrEndX[position])-newF.centralX)>=-1e-4)){
            life--;
        }
    }
}

//Function that make bomb fly
void Boom::randomFly(long position, double startTime1){
    if ((arrContinueFly[position]==true)&&(timer>=startTime1+arrStartTime[position])&&(arrCentralY[position]>=-63)){
        getNewPosition(position, startTime1);
        if ((newF.centralY>=1e-5)&&(newF.centralY<=1e+5))
            if (pointInPolygon(mouseX, WindowSizeY-mouseY, newF)){
                arrContinueFly[position]=false;
                if (gameStop==false)
                    life--;
                else arrContinueFly[position]=true;
            }
        draw();
    }
}
