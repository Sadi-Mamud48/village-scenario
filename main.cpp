#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>


float skyR = 0.53f, skyG = 0.81f, skyB = 0.92f;
float targetR = 0.53f, targetG = 0.81f, targetB = 0.92f;
float transitionSpeed = 0.02f;
bool isTransitioning = false;


float targetSunY = 200.0f, targetMoonY = -200.0f;



float cloudR = 1.0f, cloudG = 1.0f, cloudB = 1.0f;
float targetCloudR = 1.0f, targetCloudG = 1.0f, targetCloudB = 1.0f;

float hillR = 0.2f, hillG = 0.4f, hillB = 0.1f;
float targetHillR = 0.2f, targetHillG = 0.4f, targetHillB = 0.1f;
float groundR = 0.0f, groundG = 0.8f, groundB = 0.0f;
float targetGroundR = 0.0f, targetGroundG = 0.8f, targetGroundB = 0.0f;
float treeTrunkR = 0.55f, treeTrunkG = 0.27f, treeTrunkB = 0.07f;
float targetTreeTrunkR = 0.55f, targetTreeTrunkG = 0.27f, targetTreeTrunkB = 0.07f;
float treeLeavesR = 0.13f, treeLeavesG = 0.55f, treeLeavesB = 0.13f;
float targetTreeLeavesR = 0.13f, targetTreeLeavesG = 0.55f, targetTreeLeavesB = 0.13f;

float riverR = 0.0f, riverG = 0.4f, riverB = 0.7f;
float targetRiverR = 0.0f, targetRiverG = 0.4f, targetRiverB = 0.7f;

float boatx = 500.0f;
float boatSpeed = 0.4f;
bool showpal = false;
bool windowGlow = false;

bool starsVisible = false;
float starPositions[50][2];
bool starsInitialized = false;
bool rainbowVisible = false;

// Rain variables
bool rainActive = false;
float raindrops[200][3]; // x, y, speed
bool raindropInitialized = false;

// Thunderstorm
bool thunderActive = false;
int thunderTimer = 0;
float thunderBrightness = 0.0f;

//FRIEND'S CODE Variables (UFO, COW, WINDMILL)
static int slices = 15;
static int stacks = 10;

// UFO Variables
float ufoX = -7.0f;
float ufoY = 0.7f;
float ufoZ = -3.0f;
float ufoscale = 0.4f;

//UFO animation states
float ufospeed = 0.003f; // UFO movement speed
float targetufox = 0.9f; // Stop position on cow
int beamon = 0;

// Cow variables in 2D
float cowx = 200.0f;
float cowy = -100.0f;

//Cow animation states
float cowliftspeed = 0.2f;
float cowscale = 1.0f; //shrink factor
int cowabductionbyaliens = 0;

// Human variables
float humanx = 280.0f; // Standing beside the cow
float humany = -100.0f;
float humanRunSpeed = 1.0f; // Reduced from 2.0f (half speed)
bool humanRunning = false;

// Windmill variables
float windmillx = -450.0f; // Moved further left
float windmilly = -120.0f;
float bladeangle = 0.0f;

//windmill animation states
int windmillrotate = 0;

// UFO animation
int ufouptothespace = 0;
int animationbegins = 0;


void plotCirclePoints(int xc, int yc, int x, int y, float R, float G, float B)
{
    glColor3f(R, G, B);
    glBegin(GL_POINTS);
    glVertex2f(xc + x, yc + y);
     glVertex2f(xc - x, yc + y);
    glVertex2f(xc + x, yc - y);
     glVertex2f(xc - x, yc - y);
    glVertex2f(xc + y, yc + x);
     glVertex2f(xc - y, yc + x);
    glVertex2f(xc + y, yc - x);
     glVertex2f(xc - y, yc - x);
    glEnd();
}

//Functions
void drawCircleMidpoint(float cx, float cy, float radius, float R, float G, float B)
{
    int r = (int)radius;
    int x = 0;
    int y = r;
    int p = 1 - r;

    glPointSize(2.0f);
    plotCirclePoints(cx, cy, x, y, R, G, B);

    while (x < y) {
        x++;
        if (p < 0) {
            p = p + 2 * x + 1;
        } else {
            y--;
            p = p + 2 * (x - y) + 1;
        }
        plotCirclePoints(cx, cy, x, y, R, G, B);
    }

    // Fill the circle
    glColor3f(R, G, B);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 100; i++) {
        float angle = 2.0f * 3.14159f * i / 100;
        glVertex2f(cx + radius * cos(angle), cy + radius * sin(angle));
    }
    glEnd();
    glPointSize(1.0f);
}

//2D projection for all the 2D objects
void set2D()
{
    glMatrixMode(GL_PROJECTION);
 glLoadIdentity();

    gluOrtho2D(-600, 600, -350, 350);
     glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

//3D projections for the UFO
void set3D(int w, int h)
{
    float ar = (float)w / (float)h;
    glMatrixMode(GL_PROJECTION);
     glLoadIdentity();

    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);
     glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void init()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (!starsInitialized) {
        for (int i = 0; i < 50; i++) {
            starPositions[i][0] = -600 + (rand() % 1200);
            starPositions[i][1] = 100 + (rand() % 250);
        }
        starsInitialized = true;
    }

    if (!raindropInitialized) {
        for (int i = 0; i < 200; i++) {
            raindrops[i][0] = -600 + (rand() % 1200);
            raindrops[i][1] = 350 + (rand() % 400);
            raindrops[i][2] = 3.0f + (rand() % 3);
        }
        raindropInitialized = true;
    }
}


void drawBezierCurve(float p0x, float p0y, float p1x, float p1y,float p2x, float p2y, float p3x, float p3y,float r, float g, float b)
{
    glColor3f(r, g, b);
    glLineWidth(8.0f);
    glBegin(GL_LINE_STRIP);
    for (float t = 0; t <= 1.0f; t += 0.01f) {
         float mt = 1 - t;
        float x = mt*mt*mt*p0x + 3*mt*mt*t*p1x + 3*mt*t*t*p2x + t*t*t*p3x;
         float y = mt*mt*mt*p0y + 3*mt*mt*t*p1y + 3*mt*t*t*p2y + t*t*t*p3y;
        glVertex2f(x, y);
    }
    glEnd();
    glLineWidth(1.0f);
}

void drawRainbow()
{
    if (!rainbowVisible || starsVisible) return;

    float rainbowX = 110.0f;
     float rainbowY = 0.0f;
    float rainbowScaleX = 0.6f;
     float rainbowScaleY = 0.9f;

    glPushMatrix();
     glTranslatef(rainbowX, rainbowY, 0);
    glScalef(rainbowScaleX, rainbowScaleY, 1.0f);

    float colors[7][3] = { {0.9f, 0.0f, 0.0f}, {1.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 0.0f},{0.0f, 0.8f, 0.0f}, {0.0f, 0.5f, 1.0f}, {0.3f, 0.0f, 0.8f}, {0.6f, 0.0f, 0.8f}
    };
    for (int i = 0; i < 7; i++) {
        float offset = i * 15.0f;
        drawBezierCurve(-500, 50 + offset, -250, 300 + offset,250, 300 + offset, 500, 50 + offset,colors[i][0], colors[i][1], colors[i][2]);
    }

    glPopMatrix();
}

//Cow
void drawCow()
{

    glPushMatrix();
    glTranslatef(cowx, cowy, 0.0f);  // Translation
    glScalef(cowscale, cowscale, 1.0f);  // Scaling

//body
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
     glVertex2f(-30, 0);
      glVertex2f(30, 0);
    glVertex2f(30, 25);
    glVertex2f(-30, 25);

    glEnd();

//head
glBegin(GL_QUADS);
 glVertex2f(25, 20);
  glVertex2f(45, 20);
  glVertex2f(45, 35);
glVertex2f(25, 35);
    glEnd();
//spots
    drawCircleMidpoint(-15, 15, 6, 0, 0, 0);
drawCircleMidpoint(10, 18, 7, 0, 0, 0);

//legs
    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
glVertex2f(-20, -15);
 glVertex2f(-15, -15);
 glVertex2f(-15, 0);
  glVertex2f(-20, 0);
    glVertex2f(-5, -15);
     glVertex2f(0, -15);
     glVertex2f(0, 0);
      glVertex2f(-5, 0);
     glVertex2f(5, -15);
      glVertex2f(10, -15);
      glVertex2f(10, 0);
       glVertex2f(5, 0);
glVertex2f(20, -15);
 glVertex2f(25, -15);
 glVertex2f(25, 0);
 glVertex2f(20, 0);
    glEnd();

       //tail
    glBegin(GL_LINE_STRIP);
    glVertex2f(-30, 20);
    glVertex2f(-40, 25);
     glVertex2f(-35, 15);
    glEnd();

    glPopMatrix();
}


void drawHuman()
{
    if (humanx > 650) return;


    glPushMatrix();
    glTranslatef(humanx, humany, 0.0f);  // Translation

    // Head
    glColor3f(0.96f, 0.8f, 0.69f);
     glBegin(GL_QUADS);
    glVertex2f(-8, 35);
     glVertex2f(8, 35);
      glVertex2f(8, 50);
       glVertex2f(-8, 50);
    glEnd();

    // Body
    glColor3f(0.2f, 0.4f, 0.8f);
     glBegin(GL_QUADS);
    glVertex2f(-12, 10);
     glVertex2f(12, 10);
     glVertex2f(12, 35);
      glVertex2f(-12, 35);
    glEnd();

    // Arms and Hands
    if (humanRunning) {
        glColor3f(0.2f, 0.4f, 0.8f);
         glBegin(GL_QUADS);
        glVertex2f(-15, 25);
         glVertex2f(-12, 26);
          glVertex2f(-10, 12);
          glVertex2f(-13, 11);
        glEnd();
         glColor3f(0.96f, 0.8f, 0.69f);
        glBegin(GL_QUADS);
glVertex2f(-13, 11);
        glVertex2f(-10, 12);
         glVertex2f(-10, 7);
         glVertex2f(-13, 6);
    glEnd();

        glColor3f(0.2f, 0.4f, 0.8f);
        glBegin(GL_QUADS);
         glVertex2f(12, 26);
    glVertex2f(15, 25);
          glVertex2f(22, 18);
           glVertex2f(19, 17);
        glEnd();
        glColor3f(0.96f, 0.8f, 0.69f);
         glBegin(GL_QUADS);
    glVertex2f(19, 17); glVertex2f(22, 18);
         glVertex2f(23, 13); glVertex2f(20, 12);
        glEnd();
    } else {
         glColor3f(0.2f, 0.4f, 0.8f);
        glBegin(GL_QUADS);
         glVertex2f(-16, 28); glVertex2f(-12, 28); glVertex2f(-12, 10); glVertex2f(-16, 10);
        glEnd();
    glColor3f(0.96f, 0.8f, 0.69f);
         glBegin(GL_QUADS);
        glVertex2f(-16, 10); glVertex2f(-12, 10); glVertex2f(-12, 5); glVertex2f(-16, 5);
        glEnd();

        glColor3f(0.2f, 0.4f, 0.8f);
         glBegin(GL_QUADS);
        glVertex2f(12, 28);
    glVertex2f(16, 28);
        glVertex2f(16, 10);
         glVertex2f(12, 10);
         glEnd();
    glColor3f(0.96f, 0.8f, 0.69f);
         glBegin(GL_QUADS);
        glVertex2f(12, 10);
         glVertex2f(16, 10);
         glVertex2f(16, 5);
          glVertex2f(12, 5);
        glEnd();

    }

    glColor3f(0.3f, 0.2f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(-12, 0); glVertex2f(12, 0);
     glVertex2f(12, 10); glVertex2f(-12, 10);
    glEnd();

    if (humanRunning) {
        glBegin(GL_QUADS);
        glVertex2f(-10, 0);
        glVertex2f(-5, 0);
         glVertex2f(-3, -20);
          glVertex2f(-8, -20);
        glEnd();
        glBegin(GL_QUADS);
        glVertex2f(5, 0);
        glVertex2f(10, 0);
         glVertex2f(12, -18);
         glVertex2f(7, -18);
        glEnd();
    } else {
        glBegin(GL_QUADS);
        glVertex2f(-10, 0);
         glVertex2f(-5, 0);
         glVertex2f(-5, -20);
          glVertex2f(-10, -20);
        glEnd();
        glBegin(GL_QUADS);
        glVertex2f(5, 0);
        glVertex2f(10, 0);
        glVertex2f(10, -20);
         glVertex2f(5, -20);
        glEnd();

    }

    glColor3f(0.0f, 0.0f, 0.0f);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    glVertex2f(-4, 43);
    glVertex2f(4, 43);
    glEnd();
    glPointSize(1.0f);

    glPopMatrix();
}



void drawLineDDA(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float steps;

    // Determine number of steps needed
    if (fabs(dx) > fabs(dy))
        steps = fabs(dx);
    else
        steps = fabs(dy);

    // Calculate increment in x and y for each step
    float xIncrement = dx / steps;
    float yIncrement = dy / steps;

    float x = x1;
    float y = y1;

    glBegin(GL_POINTS);
    for (int i = 0; i <= steps; i++) {
        glVertex2f(round(x), round(y));
        x += xIncrement;
        y += yIncrement;
    }
    glEnd();
}

//windmill starts from here

//windmils blades
void drawBlade()
{
    glColor3f(0.50f, 0.0f, 0.0f);
    glPointSize(2.0f);

    // Draw blade using DDA
    drawLineDDA(-10, 0, -10, 120);
    drawLineDDA(10, 0, 10, 120);
    drawLineDDA(-10, 120, 10, 120);
    drawLineDDA(-10, 0, 10, 0);

    // Fill the blade with horizontal DDA lines
    for (int y = 0; y <= 120; y += 2) {
        drawLineDDA(-10, y, 10, y);
    }

    glPointSize(1.0f);
}

void drawWindmill()
{

    glPushMatrix();
    glTranslatef(windmillx, windmilly, 0);

        //towers body
    glColor3f(1.0f, 0.5f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-70, 0); glVertex2f(70, 0); glVertex2f(30, 220); glVertex2f(-30, 220);
    glEnd();

    glLineWidth(7.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(-60, 30);
    glVertex2f(60, 30);
    glVertex2f(-55, 50);
     glVertex2f(55, 50);
    glVertex2f(-50, 70);
    glVertex2f(50, 70);
    glVertex2f(-45, 90);
     glVertex2f(45, 90);
    glVertex2f(-40, 110);
    glVertex2f(40, 110);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);

    glVertex2f(-20, 130); glVertex2f(20, 130); glVertex2f(20, 160); glVertex2f(-20, 160);
    glEnd();

    glPushMatrix();
    glTranslatef(0, 220, 0);
drawCircleMidpoint(0, 0, 15, 0.9f, 0.2f, 0.2f);

// blades rotations
    glRotatef(bladeangle, 0, 0, 1);
    for (int i = 0; i < 4; i++) {
        glRotatef(90, 0, 0, 1);
        drawBlade();
    }
    glPopMatrix();
    glPopMatrix();
}

//UFO starts from here

void drawUFO(void)
{

    GLfloat mat_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat mat_diffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat mat_specular[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat mat_shininess[] = { 50.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

//UFo main body
    glColor3f(0.6f, 0.6f, 0.6f);
    glPushMatrix();
    glScaled(1.6, 0.3, 1.6);
    glutSolidSphere(0.8, slices, stacks);
    glPopMatrix();

//UFO top part
    GLfloat mat_diffuse_red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_red);
    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslated(0.0, 0.35, 0.0);
    glutSolidSphere(0.5, slices, stacks);
    glPopMatrix();

//UFo below part
    GLfloat mat_diffuse_dark[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_dark);
    glColor3f(0.5f, 0.5f, 0.5f);
    glPushMatrix();
    glTranslated(0.0, -0.25, 0.0);
    glScaled(1.0, 0.4, 1.0);
    glutSolidSphere(0.6, slices, stacks);
    glPopMatrix();
}

//triangle for the beam light
void drawTriangle()
{
    glBegin(GL_TRIANGLES);
    glColor4f(1.0f, 1.0f, 0.8f, 0.6f);
    glVertex3f(-0.6f, -1.4f, 0.0f);
    glVertex3f(0.6f, -1.4f, 0.0f);
    glVertex3f(0.0f, 2.0f, 0.0f);
    glEnd();
}


void drawBoat()
{
    float boatY = -250.0f;      // Y position (উপর-নিচে)
    float boatScaleX = 1.0f;    // X-axis scale
    float boatScaleY = 1.0f;    // Y-axis scale

    glPushMatrix();
    glTranslatef(boatx, boatY, 0.0f);
    glScalef(boatScaleX, boatScaleY, 1.0f);

    glBegin(GL_POLYGON);
    glColor3f(0.1f, 0.3f, 0.2f);
    glVertex2f(-60, 19);
    glVertex2f(-60, -45);
    glVertex2f(60, 19);
    glVertex2f(60, -45);
    glEnd();

    float cx = -60;
    float cy = -6;
    float r = 25;
    int n = 100;
    glColor3f(0.8f, 0.6f, 0.4f);
    glBegin(GL_POLYGON);
    for(int i = 0; i <= n; i++) {
        float t = 2.0f * 3.1416f * float(i) / float(n);
        float x = cx + r * cosf(t);
        float y = cy + r * sinf(t);
        glVertex2f(x, y);
    }
    glEnd();

    float ci = 60;
    float cf = -6;
    float a = 25;
    int z = 100;
    glColor3f(0.1f, 0.3f, 0.2f);
    glBegin(GL_POLYGON);
    for(int i = 0; i <= z; i++) {
        float t = 2.0f * 3.1416f * float(i) / float(z);
        float x = ci + a * cosf(t);
        float y = cf + a * sinf(t);
        glVertex2f(x, y);
    }
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(-152, -8);
    glVertex2f(-104, -45);
    glVertex2f(104, -45);
    glVertex2f(152, -8);
    glVertex2f(104, -56);
    glVertex2f(-104, -56);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(-105, -8);
    glVertex2f(-100, -8);
    glVertex2f(-100, 90);
    glVertex2f(-105, 90);
    glEnd();

    glPopMatrix();
}

void drawpal()
{
    float boatY = -250.0f;      // Same as boat
    float boatScaleX = 1.0f;    // Same as boat
    float boatScaleY = 1.0f;    // Same as boat

    glPushMatrix();
    glTranslatef(boatx, boatY, 0.0f);
    glScalef(boatScaleX, boatScaleY, 1.0f);

    glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.95f);
    glVertex2f(-105, 80);
    glVertex2f(-150, 80);
    glVertex2f(-120, 50);
    glVertex2f(-105, 55);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.95f);
    glVertex2f(-105, 40);
    glVertex2f(-150, 40);
    glVertex2f(-120, 10);
    glVertex2f(-105, 15);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.95f);
    glVertex2f(-100, 80);
    glVertex2f(-50, 80);
    glVertex2f(-80, 50);
    glVertex2f(-100, 55);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.95f);
    glVertex2f(-100, 40);
    glVertex2f(-50, 40);
    glVertex2f(-80, 10);
    glVertex2f(-100, 15);
    glEnd();

    glPopMatrix();
}


// Add these variables at the top with other variables
float boat2x = -600.0f;  // Starting from left
float boat2Speed = 0.6f;
bool boat2Light = false;

// Digital Boat function
void drawBoat2()
{
    float boat2Y = -180.0f;      // Different Y position than first boat
    float boat2ScaleX = 2.0f;    // Smaller than first boat
    float boat2ScaleY = 0.8f;

    glPushMatrix();
    glTranslatef(boat2x, boat2Y, 0.0f);
    glScalef(boat2ScaleX, boat2ScaleY, 1.0f);

    // Main body - modern angular design
    glBegin(GL_POLYGON);
    glColor3f(0.2f, 0.2f, 0.3f);  // Dark blue-grey
    glVertex2f(-70, 10);
    glVertex2f(-50, -30);
    glVertex2f(50, -30);
    glVertex2f(70, 10);
    glVertex2f(50, 10);
    glVertex2f(-50, 10);
    glEnd();

    // Cabin
    glBegin(GL_QUADS);
    glColor3f(0.3f, 0.3f, 0.4f);
    glVertex2f(-40, 10);
    glVertex2f(40, 10);
    glVertex2f(35, 50);
    glVertex2f(-35, 50);
    glEnd();

    // Roof
    glBegin(GL_QUADS);
    glColor3f(0.25f, 0.25f, 0.35f);
    glVertex2f(-38, 50);
    glVertex2f(38, 50);
    glVertex2f(33, 60);
    glVertex2f(-33, 60);
    glEnd();

    // Windows (2 windows)
    if (boat2Light) {
        glColor3f(1.0f, 1.0f, 0.3f);  // Bright yellow when light is on
    } else {
        glColor3f(0.4f, 0.6f, 0.8f);  // Light blue when off
    }

    // Left window
    glBegin(GL_QUADS);
    glVertex2f(-30, 25);
    glVertex2f(-15, 25);
    glVertex2f(-15, 40);
    glVertex2f(-30, 40);
    glEnd();

    // Right window
    glBegin(GL_QUADS);
    glVertex2f(15, 25);
    glVertex2f(30, 25);
    glVertex2f(30, 40);
    glVertex2f(15, 40);
    glEnd();

    // Front light (headlight)
    if (boat2Light) {
        glColor3f(1.0f, 1.0f, 0.0f);  // Bright yellow
    } else {
        glColor3f(0.5f, 0.5f, 0.5f);  // Grey when off
    }
    drawCircleMidpoint(65, 5, 5, boat2Light ? 1.0f : 0.5f,
                                 boat2Light ? 1.0f : 0.5f,
                                 boat2Light ? 0.0f : 0.5f);

    // Light beam (only visible when light is on)
    if (boat2Light) {
        glColor4f(1.0f, 1.0f, 0.8f, 0.3f);  // Semi-transparent yellow
        glBegin(GL_TRIANGLES);
        glVertex2f(70, 5);
        glVertex2f(120, 20);
        glVertex2f(120, -10);
        glEnd();
    }

    // Antenna
    glColor3f(0.7f, 0.1f, 0.1f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(0, 60);
    glVertex2f(0, 80);
    glEnd();
    glLineWidth(1.0f);

    // Antenna light
    drawCircleMidpoint(0, 82, 3, 1.0f, 0.0f, 0.0f);

    glPopMatrix();
}

// Update function for boat2
void updateBoat2()
{
    boat2x += boat2Speed;
    if (boat2x > 750) {
        boat2x = -750;  // Reset to left when it goes off right side
    }
}

//YOUR ORIGINAL 2D OBJECTS
void drawCloud(float x, float y)
{
    drawCircleMidpoint(x, y, 30, cloudR, cloudG, cloudB);
    drawCircleMidpoint(x + 25, y + 10, 35, cloudR, cloudG, cloudB);
    drawCircleMidpoint(x + 50, y, 30, cloudR, cloudG, cloudB);
    drawCircleMidpoint(x + 25, y - 10, 28, cloudR, cloudG, cloudB);
}

float cloudX1 = 600.0f, cloudX2 = 900.0f, cloudX3 = 300.0f;

void drawClouds()
{
    float scaleX = 2.0f;
    float scaleY = 1.2f;

    glPushMatrix();
    glScalef(scaleX, scaleY, 1.0f);

    drawCloud(cloudX1/scaleX, 250/scaleY);
    drawCloud(cloudX2/scaleX, 180/scaleY);
    drawCloud(cloudX3/scaleX, 280/scaleY);

    glPopMatrix();
}

float cloudX11 = 1000.0f, cloudX22 = 1200.0f, cloudX33 = 500.0f,cloudX44 = 800.0f;
void drawClouds2()
{
    float scaleX = 0.5f;
    float scaleY = 0.5f;

    glPushMatrix();
    glScalef(scaleX, scaleY, 1.0f);

    drawCloud(cloudX11/scaleX, 250/scaleY);
    drawCloud(cloudX22/scaleX, 180/scaleY);
    drawCloud(cloudX33/scaleX, 280/scaleY);

    glPopMatrix();
}

void drawStars()
{
    if (starsVisible) {
        glColor3f(1.0f, 1.0f, 1.0f);
        glPointSize(2.0f);
        glBegin(GL_POINTS);
        for (int i = 0; i < 50; i++)
            glVertex2f(starPositions[i][0], starPositions[i][1]);
        glEnd();
        glPointSize(1.0f);
    }
}







//  sun

float sunY = 200.0f;

void drawSun()
{
    drawCircleMidpoint(-300, sunY, 50, 1.0f, 0.9f, 0.0f);
}






//moon
float moonY = -200.0f;
float moonX = 300.0f;

void drawMoon()
{
     drawCircleMidpoint(moonX, moonY, 50, 0.9f, 0.9f, 0.95f);

    drawCircleMidpoint(moonX - 15, moonY + 15, 12, 0.7f, 0.7f, 0.75f);
     drawCircleMidpoint(moonX + 10, moonY - 10, 10, 0.7f, 0.7f, 0.75f);
    drawCircleMidpoint(moonX - 5,  moonY - 20, 8,  0.7f, 0.7f, 0.75f);
}








void drawSky()
{
    glBegin(GL_QUADS);
    if (thunderActive) {
        glColor3f(skyR + thunderBrightness, skyG + thunderBrightness, skyB + thunderBrightness);
    } else {
        glColor3f(skyR, skyG, skyB);
    }
    glVertex2f(-600, 70); glVertex2f(600, 70);
    glVertex2f(600, 350); glVertex2f(-600, 350);
    glEnd();
}

void drawGround()
{
    glBegin(GL_QUADS);
    glColor3f(groundR, groundG, groundB);
    glVertex2f(-600, -140); glVertex2f(600, -140);
    glVertex2f(600, 70); glVertex2f(-600, 70);
    glEnd();
}

void drawHills()
{
    glColor3f(hillR, hillG, hillB);
    glBegin(GL_TRIANGLES);
    glVertex2f(-400, 250);
     glVertex2f(-550, 70);
    glVertex2f(-250, 70);
    glEnd();

glBegin(GL_TRIANGLES);
    glVertex2f(50, 220);
    glVertex2f(-120, 70);
     glVertex2f(220, 70);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(420, 190);

glVertex2f(300, 70); glVertex2f(540, 70);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(-150, 180);
    glVertex2f(-250, 70);

    glVertex2f(-50, 70);
    glEnd();

 glBegin(GL_TRIANGLES);
    glVertex2f(-150, 180);
    glVertex2f(-250, 70);

    glVertex2f(-50, 70);
    glEnd();



}

void drawTree(float x, float y, float scale)
{
    glColor3f(treeTrunkR, treeTrunkG, treeTrunkB);
    glBegin(GL_QUADS);
    glVertex2f(x - 8 * scale, y); glVertex2f(x + 8 * scale, y);
    glVertex2f(x + 8 * scale, y + 40 * scale); glVertex2f(x - 8 * scale, y + 40 * scale);
    glEnd();

    drawCircleMidpoint(x, y + 50 * scale, 25 * scale, treeLeavesR, treeLeavesG, treeLeavesB);
    drawCircleMidpoint(x - 15 * scale, y + 40 * scale, 22 * scale, treeLeavesR, treeLeavesG, treeLeavesB);
    drawCircleMidpoint(x + 15 * scale, y + 40 * scale, 22 * scale, treeLeavesR, treeLeavesG, treeLeavesB);
}

void drawTrees()
{
    // Tree 1
    float tree1X = -600.0f;     // X position
    float tree1Y = 0.0f;        // Y position
    float tree1Scale = 2.2f;    // Size
    drawTree(tree1X, tree1Y, tree1Scale);

    // Tree 2
    float tree2X = -250.0f;
    float tree2Y = 0.0f;
    float tree2Scale = 1.3f;
    drawTree(tree2X, tree2Y, tree2Scale);

    // Tree 3
    float tree3X = 250.0f;
    float tree3Y = 0.0f;
    float tree3Scale = 2.3f;
    drawTree(tree3X, tree3Y, tree3Scale);

    // Tree 4
    float tree4X = 450.0f;
    float tree4Y = 0.0f;
    float tree4Scale = 1.9f;
    drawTree(tree4X, tree4Y, tree4Scale);

    // Tree 5
    float tree5X = 100.0f;
    float tree5Y = 0.0f;
    float tree5Scale = 1.7f;
    drawTree(tree5X, tree5Y, tree5Scale);
}

void drawHouse()
{
    float houseX = -100.0f;
    float houseY = -50.0f;
    float scaleX = 1.7f;
    float scaleY = 2.5f;

    glPushMatrix();
    glTranslatef(houseX, houseY, 0.0f);
    glScalef(scaleX, scaleY, 1.0f);


    glColor3f(0.82f, 0.71f, 0.55f);
    glBegin(GL_QUADS);
    glVertex2f(-50, 0); glVertex2f(50, 0);
    glVertex2f(50, 60); glVertex2f(-50, 60);
    glEnd();

    glColor3f(0.6f, 0.2f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0, 90);
    glVertex2f(-60, 60);
    glVertex2f(60, 60);
    glEnd();

    glColor3f(0.4f, 0.2f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(-15, 0); glVertex2f(15, 0);
    glVertex2f(15, 35); glVertex2f(-15, 35);
    glEnd();

    drawCircleMidpoint(10, 18, 2, 0.8f, 0.7f, 0.0f);

    if (windowGlow)
        glColor3f(1.0f, 0.9f, 0.4f);
    else
        glColor3f(0.6f, 0.8f, 0.9f);

    glBegin(GL_QUADS);
    glVertex2f(-40, 35); glVertex2f(-20, 35);
    glVertex2f(-20, 50); glVertex2f(-40, 50);
    glVertex2f(20, 35); glVertex2f(40, 35);
    glVertex2f(40, 50); glVertex2f(20, 50);
    glEnd();

    glPopMatrix();
}

// NEW HOUSE 2 - Different design
void drawHouse2()
{
    float house2X = 400.0f;
    float house2Y = -50.0f;
    float scale2X = 2.0f;
    float scale2Y = 2.0f;

    glPushMatrix();
    glTranslatef(house2X, house2Y, 0.0f);
    glScalef(scale2X, scale2Y, 1.0f);

    // Main body (আয়তাকার ঘর)
    glColor3f(0.9f, 0.85f, 0.7f);  // হালকা বেইজ রঙ
    glBegin(GL_QUADS);
    glVertex2f(-60, 0); glVertex2f(60, 0);
    glVertex2f(60, 70); glVertex2f(-60, 70);
    glEnd();

    // Roof (সমতল ছাদ)
    glColor3f(0.5f, 0.3f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(-70, 70); glVertex2f(70, 70);
    glVertex2f(60, 85); glVertex2f(-60, 85);
    glEnd();

    // Chimney (চিমনি)
    glColor3f(0.6f, 0.3f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(30, 85); glVertex2f(45, 85);
    glVertex2f(45, 110); glVertex2f(30, 110);
    glEnd();

    // Door (দরজা - বড়)
    glColor3f(0.3f, 0.15f, 0.05f);
    glBegin(GL_QUADS);
    glVertex2f(-20, 0); glVertex2f(20, 0);
    glVertex2f(20, 45); glVertex2f(-20, 45);
    glEnd();

    // Door knob
    drawCircleMidpoint(15, 22, 2, 0.9f, 0.8f, 0.1f);

    // Windows (2টা জানালা - পাশাপাশি)
    if (windowGlow)
        glColor3f(1.0f, 0.95f, 0.5f);
    else
        glColor3f(0.7f, 0.85f, 0.95f);

    // Left window
    glBegin(GL_QUADS);
    glVertex2f(-50, 40); glVertex2f(-30, 40);
    glVertex2f(-30, 60); glVertex2f(-50, 60);
    glEnd();

    // Right window
    glBegin(GL_QUADS);
    glVertex2f(30, 40); glVertex2f(50, 40);
    glVertex2f(50, 60); glVertex2f(30, 60);
    glEnd();

    // Window frames (জানালার ফ্রেম)
    glColor3f(0.3f, 0.2f, 0.1f);
    glLineWidth(2.0f);
    // Left window frame
    glBegin(GL_LINE_LOOP);
    glVertex2f(-50, 40); glVertex2f(-30, 40);
    glVertex2f(-30, 60); glVertex2f(-50, 60);
    glEnd();
    // Left window cross
    glBegin(GL_LINES);
    glVertex2f(-40, 40); glVertex2f(-40, 60);
    glVertex2f(-50, 50); glVertex2f(-30, 50);
    glEnd();

    // Right window frame
    glBegin(GL_LINE_LOOP);
     glVertex2f(30, 40); glVertex2f(50, 40);
    glVertex2f(50, 60); glVertex2f(30, 60);
    glEnd();
    // Right window cross
    glBegin(GL_LINES);
    glVertex2f(40, 40); glVertex2f(40, 60);
    glVertex2f(30, 50); glVertex2f(50, 50);
    glEnd();
    glLineWidth(1.0f);

    glPopMatrix();
}

void drawRiver()
{
    glBegin(GL_QUADS);
    glColor3f(riverR, riverG, riverB);
    glVertex2f(-600, -350); glVertex2f(600, -350);
    glVertex2f(600, -140); glVertex2f(-600, -140);
    glEnd();
}

//RAIN
void drawRain()
{
    if (!rainActive) return;

    glColor4f(0.7f, 0.7f, 0.9f, 0.6f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    for (int i = 0; i < 200; i++) {
        glVertex2f(raindrops[i][0], raindrops[i][1]);
        glVertex2f(raindrops[i][0] - 2, raindrops[i][1] - 15);
    }
    glEnd();
    glLineWidth(1.0f);
}

void updateRain()
{
    if (!rainActive) return;

    for (int i = 0; i < 200; i++) {
        raindrops[i][1] -= raindrops[i][2];

        if (raindrops[i][1] < -350) {
            raindrops[i][1] = 350 + (rand() % 100);
            raindrops[i][0] = -600 + (rand() % 1200);
        }
    }
}

void updateThunder()
{
    if (thunderActive) {
        thunderTimer++;
        if (thunderTimer < 50) {
            thunderBrightness = 0.5f;
        } else {
            thunderActive = false;
            thunderTimer = 0;
            thunderBrightness = 0.0f;
        }
    }
}


float cloudSpeed = 1.0f;
void updateClouds()
{
    cloudX1 -= cloudSpeed;
    cloudX2 -= cloudSpeed;
    cloudX3 -= cloudSpeed;
    if (cloudX1 < -800) cloudX1 = 800;
    if (cloudX2 < -800) cloudX2 = 800;
    if (cloudX3 < -800) cloudX3 = 800;
}


void updateClouds2()
{
    cloudX11 -= cloudSpeed;
    cloudX22 -= cloudSpeed;
    cloudX33 -= cloudSpeed;
    if (cloudX11 < -700) cloudX11 = 700;
    if (cloudX22 < -700) cloudX22 = 700;
    if (cloudX33 < -700) cloudX33 = 700;
    if (cloudX44 < -700) cloudX44 = 700;
}


void updateBoat()
{
    if (showpal) {
        boatx -= boatSpeed;
        if (boatx < -750) boatx = 750;
    }
}

void ndTransition()
{
    auto updateColor = [](float &current, float target) {
        float diff = target - current;
        if (diff*diff > 0.0001f) {
            current += diff * 0.001f;// night and day transition kore felbo
        } else {
            current = target;
        }
    };

    updateColor(skyR, targetR);
     updateColor(skyG, targetG);
      updateColor(skyB, targetB);

    updateColor(cloudR, targetCloudR);
     updateColor(cloudG, targetCloudG);
      updateColor(cloudB, targetCloudB);
    updateColor(hillR, targetHillR);
     updateColor(hillG, targetHillG);
     updateColor(hillB, targetHillB);
    updateColor(groundR, targetGroundR);
    updateColor(groundG, targetGroundG);
     updateColor(groundB, targetGroundB);

    updateColor(treeTrunkR, targetTreeTrunkR);
     updateColor(treeTrunkG, targetTreeTrunkG);
     updateColor(treeTrunkB, targetTreeTrunkB);
    updateColor(treeLeavesR, targetTreeLeavesR);
    updateColor(treeLeavesG, targetTreeLeavesG);
     updateColor(treeLeavesB, targetTreeLeavesB);
    updateColor(riverR, targetRiverR);
     updateColor(riverG, targetRiverG);
      updateColor(riverB, targetRiverB);
    updateColor(sunY, targetSunY);
    updateColor(moonY, targetMoonY);
}

// DISPLAY
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    double a = t * 60.0;

//2D background
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    set2D();

    drawSky();
    drawStars();
    drawSun();
    drawMoon();
    drawRainbow();
    drawClouds();
     drawClouds2();

    drawGround();
    drawHills();
    drawTrees();
    drawWindmill();
    drawHouse();
    drawHouse2();
    drawRiver();
     drawBoat2();
    drawBoat();
    if (showpal)
        drawpal();
    drawRain();
    drawHuman();

    if (cowscale > 0.01f)
        drawCow();

    if (windmillrotate) {
        bladeangle += 1.0f; //Windmills blade rotation speed
    }

// UFO movement
    if (animationbegins) {
        if (!ufouptothespace) {
            if (ufoX + ufospeed<targetufox) {
                ufoX += ufospeed;
            } else {
                ufoX = targetufox;
                beamon = 1;
                cowabductionbyaliens = 1;
                humanRunning = true;
            }
        } else {
            ufoY += 0.004f;// ufo space a chole jabe
        }
    }

    // Human running away
    if (humanRunning && humanx < 650) {
        humanx += humanRunSpeed;
    }

//cow churi korar animation
    if (animationbegins) {
        if (cowabductionbyaliens && cowscale > 0.01f) {
            cowy += cowliftspeed;
            cowscale -= 0.0006f;
        } else if (cowabductionbyaliens) {
            cowabductionbyaliens = 0;
            beamon = 0;
            ufouptothespace = 1;
        }
    }

 //ufo 3d part
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    set3D(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

//ufo and beam together
    glPushMatrix();
    glTranslated(ufoX, ufoY, ufoZ);
    glScaled(ufoscale, ufoscale, ufoscale);

// UFO rotation
    glPushMatrix();
    glRotated(3, 1, 1, 0);
    glRotated(a, 0, 1, 0);
    drawUFO();
    glPopMatrix();

//goru churi korar light(beam)
    if (beamon) {
        glDisable(GL_LIGHTING);
        glPushMatrix();
        glTranslatef(0.0f, -1.6f, 0.0f);
        drawTriangle();
        glPopMatrix();
        glEnable(GL_LIGHTING);
    }

    glPopMatrix();

    glutSwapBuffers();
}

void idle(void)
{
   ndTransition();
    updateClouds();
    updateClouds2();
    updateBoat2();
    updateBoat();
    updateRain();
    updateThunder();
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 'n': case 'N':
            targetR = 0.05f;
             targetG = 0.05f;
              targetB = 0.15f;
            targetSunY = -200.0f;
             targetMoonY = 200.0f;
            targetCloudR = 0.2f;
             targetCloudG = 0.2f;
              targetCloudB = 0.25f;
            targetHillR = 0.0f;
             targetHillG = 0.2f;
            targetHillB = 0.0f;
            targetGroundR = 0.0f;
             targetGroundG = 0.3f;
             targetGroundB = 0.0f;
             targetTreeTrunkR = 0.2f;
            targetTreeTrunkG = 0.1f;
             targetTreeTrunkB = 0.0f;
            targetTreeLeavesR = 0.05f;
             targetTreeLeavesG = 0.2f;
              targetTreeLeavesB = 0.05f;
             targetRiverR = 0.0f;
             targetRiverG = 0.1f;
              targetRiverB = 0.3f;
            windowGlow = true;
             starsVisible = true;
            isTransitioning = true;
            break;
        case 'd': case 'D':
            targetR = 0.53f; targetG = 0.81f; targetB = 0.92f;
            targetSunY = 200.0f; targetMoonY = -200.0f;
            targetCloudR = 1.0f; targetCloudG = 1.0f; targetCloudB = 1.0f;
            targetHillR = 0.0f; targetHillG = 0.6f; targetHillB = 0.0f;
            targetGroundR = 0.0f; targetGroundG = 0.8f; targetGroundB = 0.0f;
            targetTreeTrunkR = 0.55f; targetTreeTrunkG = 0.27f; targetTreeTrunkB = 0.07f;
            targetTreeLeavesR = 0.13f; targetTreeLeavesG = 0.55f; targetTreeLeavesB = 0.13f;
            targetRiverR = 0.0f; targetRiverG = 0.4f; targetRiverB = 0.7f;
            windowGlow = false;
            starsVisible = false;
            isTransitioning = true;
            break;
        case 's': case 'S':
            showpal = !showpal;
            break;
        case 'u': case 'U':
            animationbegins = 1;
            break;
        case 'w': case 'W':
            windmillrotate = !windmillrotate;
            break;
        case 'r': case 'R':
            rainbowVisible = !rainbowVisible;
            break;
        case 'l': case 'L':
            windowGlow = !windowGlow;
            boat2Light = !boat2Light;  // Changed from windowGlow
    break;
            break;
        case 'b': case 'B':
            rainActive = !rainActive;
            break;
        case 't': case 'T':
            thunderActive = true;
            thunderTimer = 0;
            break;
            case 27: // ESC KEY — FULL RESET
// sky color reset
    skyR = targetR = 0.53f;
    skyG = targetG = 0.81f;
    skyB = targetB = 0.92f;
// sun moon reset
    sunY  = targetSunY  = 200.0f;
    moonY = targetMoonY = -200.0f;

// color reset for rest
    cloudR = targetCloudR = 1.0f;
    cloudG = targetCloudG = 1.0f;
    cloudB = targetCloudB = 1.0f;

    hillR = targetHillR = 0.2f;
    hillG = targetHillG = 0.4f;
    hillB = targetHillB = 0.1f;

    groundR = targetGroundR = 0.0f;
    groundG = targetGroundG = 0.8f;
    groundB = targetGroundB = 0.0f;
    treeTrunkR = targetTreeTrunkR = 0.55f;
    treeTrunkG = targetTreeTrunkG = 0.27f;
    treeTrunkB = targetTreeTrunkB = 0.07f;
    treeLeavesR = targetTreeLeavesR = 0.13f;
    treeLeavesG = targetTreeLeavesG = 0.55f;
    treeLeavesB = targetTreeLeavesB = 0.13f;
    riverR = targetRiverR = 0.0f;
    riverG = targetRiverG = 0.4f;
    riverB = targetRiverB = 0.7f;
// effect turn off
    starsVisible = false;
    rainbowVisible = false;
    rainActive = false;
    thunderActive = false;
    windowGlow = false;
    boatx = 500.0f;
    showpal = false;

    boat2x = -600.0f;
    boat2Light = false;
    cloudX1 = 600.0f;
    cloudX2 = 900.0f;
    cloudX3 = 300.0f;
    cloudX11 = 1000.0f; cloudX22 = 1200.0f; cloudX33 = 500.0f; cloudX44 = 800.0f;

    windmillrotate = 0;
    bladeangle = 0.0f;

    ufoX = -7.0f; ufoY = 0.7f; ufoscale = 0.4f;
    beamon = 0;
    ufouptothespace = 0;

    cowx = 200.0f;
    cowy = -100.0f;
    cowscale = 1.0f;
    cowabductionbyaliens = 0;

     humanx = 280.0f;
   humany = -100.0f;
    humanRunning = false;


      animationbegins = 0;
      isTransitioning = false;

    break;



    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1200, 700);
    glutInitWindowPosition(100, 50);
    glutCreateWindow("OpenGL Scene - All 7 Concepts Demonstrated");

    init();

   // sahding korechi bonus dibe sir
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

    // Light properties for better shading
    GLfloat light_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

     glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
     glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
