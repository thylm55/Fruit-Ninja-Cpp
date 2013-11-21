#ifndef proto_h
#define proto_h
#define PI 3.1415926535

double arrVelocity[2000];
double arrAlphaLeft[2000];
double arrAlphaRight[2000];
double arrStartTime[2000];
double arrpX[2000];
double arrSpeed[2000];
double rF[2000];
bool arrContinueFly[2000];
double arrEndX[2000];
double arrCentralY[2000];
GLdouble mouseArr[2000][2];

static GLdouble spin = 0.0;
static GLdouble timer = 0.0;
static double r = 0.0;
static double gravity = 9.81;
static int maxKnife = 20;

bool mouseUp, mouseDown;
GLint mouseX=0, mouseY=0;
GLint mouseXD=0, mouseYD=0;
int im=0, jm, level=0;
bool gameStop=false;
bool flew;

//char * format;
char str[256];

GLdouble WindowSizeX;
GLdouble WindowSizeY;

int MAXlife = 3;
int life;
int point = 0;
int objectFlew = 0;

//define struct of a flying object
struct dataObject{
    int points;
    GLdouble centralX;
    GLdouble centralY;
    GLdouble x[15];
    GLdouble y[15];
    GLdouble quad[2][4];
};

struct TheQuad{
    GLdouble x[4];
    GLdouble y[4];
};

//Define class flying object
class FlyingObject{
    public:
        void inputData(dataObject O, char* fName);
        void getRandom(long position);
        void draw();
        void getNewPosition(long position, double startTime1);
        void randomFly(long inPosition, double inStartTime1);
    private:        
        dataObject F;
        bool continueFly;
        double velocity, alpha, startTime2, pX, pY, speed;
        double startTime, alphaR, R, beta;
    protected:
        dataObject newF;
        GLuint myglu;
        char* fileName;    
        double startTime1;
        long position;
};

class Fruit: public FlyingObject{
    public:
        void randomFly(long position, double startTime1);
};

class Boom: public FlyingObject{
    public:
        void randomFly(long position, double startTime1);
};

class StaticObject: public FlyingObject{
    public:
        void inputData(char* fName, GLdouble Size);
        void draw(GLdouble pX, GLdouble pY);
    private:
        GLdouble size;
};

#endif
