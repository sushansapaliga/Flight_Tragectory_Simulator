#include<windows.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>
#include<math.h>
#include<iostream>
#include<stdio.h>
//#include<graphics.h>

float airport[][2]={
    /***************    astralia          ************************/
    {147.143,34.375},{152.00,39.00},{157.571,42.00},{155.57,38.125},{158.143,36.250},{160.714,32.125},{164.143,35.5},
    /*********************          asia    ****************/
    {124.857,53.50},{125.714,57.750},{127.571,55.750},{136.143,57.375},{134.571,53.375},{139.857,59.00},{140.571,62.0},{137.143,64.5},{143.857,65.0},
    {141.85,70},{143.857,74.250},{147.143,76.750},{148.571,73.0},{153.0,76.625},{112.85,62.250},{113.714,57.250},{116.857,68.5},
    {117.714,76.125},{128.429,76},{126.429,72},{131.571,71.375},{124.429,66.25},{123.714,63.125},{120.143,61.625},{133.429,61.750},{131.857,66.250},
    {136.0,71.0},{138.429,74.625},{144.0,76},
    /*** North America  ***/
    {51.286,63.875},{55.286,67.250},{40.714,60.750},{38.714,65.00},{42.143,61.375},{52.571,73.625},{46.714,64.250},{42,73},{42,68},
    /** South America **/
    {65,40},{62.286,30},{62.286,40.550},{66.143,44.750},{58.857,40.500},{63,48},{63,37},
    /*** Africa ***/
    {99.143,30.625},{99.857,30.780},{99.571,30.750},{104.143,49.875},{105.00,50.250},{105.143,44.750},{100,43},{100.7,58.75},{100,60.75},{55.7,49.5},
    {102,40},{100.5,41.6},{100.4,37},{92,58},{90,54},{93,56},
    /** Europe **/
    {105,73.750},{105,70.75},{102,70.75},{108,70.75},{102,65},{97,70},{99,70},{99,78}
};
int pointSize=3,displayMenu=0;
int recal=1; /************** glut post redisplay *************/
int rot=0;
float aeroplane[2]={1,60};  /******************** selected plane , range of the plane **********/

float airport_selected[2]={0,0};
float aeroplane_at[2]={127.571,55.750};


float body_color[3]={0.1,0.1,1};
float wing_color[3]={1,1,1};

int error_message=0;

int speed=0;
int i1=0;
void *font;
void *currentfont;
int fontType,flag=0;
float color_controller=0;

GLint x0=50,y00=50,yend=90,xend=150,dy,dx;
GLfloat m;
GLfloat x11=0,y11=0;
GLfloat mov[2]={0,0};
int co=0;
float path_follow[500][2];
int dnt_repeat=-1;
int swap1=0;

struct Node{
    float x;
    float y;
    struct Node* next;
};

struct RemNode{
    float x;
    float y;
    struct RemNode* next;
};


struct RemNode* Remhead = NULL;

struct Node* head = NULL;

struct Node* PathHead = NULL;

void drawstring(float x,float y,float z,char *string1){
	char *c;
	glRasterPos3f(x,y,z);

	for(c=string1;*c!='\0';c++)
	{	glColor3f(0.0,1.0,1.0);
		glutBitmapCharacter(currentfont,*c);
	}
}

void setFont(void *font){
	currentfont=font;
}

void PathInsert(float x1,float y1) {
   struct Node* new_node = (struct Node*) malloc(sizeof(struct Node));
   new_node->x = x1;
   new_node->y = y1;
   new_node->next = PathHead;
   PathHead = new_node;
}

void PathDeleteList(){
   struct Node* current = PathHead;
   struct Node* next;

   while (current != NULL)
   {
       next = current->next;
       free(current);
       current = next;
   }
   PathHead = NULL;
}

void PathPrint() {
   struct Node* ptr;
   co=0;
   ptr = PathHead;
   //float x1,y1;
   while (ptr != NULL) {

   path_follow[co][0]=ptr->x;
   path_follow[co][1]=ptr->y;
        co++;
      ptr = ptr->next;
   }

PathDeleteList();
}

void setPixel(GLint x,GLint y){
    glColor3f(1,0,0);
    glBegin(GL_POINTS);
    glVertex2i(x,y);
    glEnd();
    x11=(GLfloat)x;
    y11=(GLfloat)y;

    PathInsert((float)x11,(float)y11);
    mov={x,y};
    glutPostRedisplay();
}

void LinePosSlope(){
    GLint p=2*dy-dx;
    GLint twoDy=2*dy;
    GLint twoDyminusDx=2*(dy-dx);
    GLint x=x0,y=y00;
    GLint z=xend;
    if(x0>xend){
        x=xend;
        y=yend;
        xend=x0;
        swap1=1;
    }
    while(x<xend){
        x++;
        if(p<0){
            p+=twoDy;
        }
        else{
            if((x0>z) && (y00<yend)){
                y--;
                p+=twoDyminusDx;
            }
            else{
                y++;
                p+=twoDyminusDx;
            }

        }
        printf("%d,%d   %d \n",x,y,p);
        setPixel(x,y);
    }
}

void LineNegSlope(){
    GLint p=2*dx-dy;
    GLint twoDy=2*dx;
    GLint twoDyminusDx=2*(dx-dy);
    GLint x=x0,y=y00;
    GLint z=yend;
    if(y00>yend){
        x=xend;
        y=yend;
        swap1=1;
        yend=y00;
    }
    while(y<yend){
        y++;
        if(p<0){
            p+=twoDy;
        }
        else{
            if(y00<z && x0>xend){
                x--;
                p+=twoDyminusDx;
            }
            else{
                x++;
                p+=twoDyminusDx;
            }

        }
        printf("%d,%d   %d \n",x,y,p);
        setPixel(x,y);
    }
}

void quad(GLfloat v[4][3]){
    glBegin(GL_QUADS);
    glVertex3fv(v[0]);
    glVertex3fv(v[1]);
    glVertex3fv(v[2]);
    glVertex3fv(v[3]);
    glEnd();
}

void drawAeroplane(){

    if(rot==0){
    GLfloat v[][3]={{2,2,0},{6,2,0},{6,3,0},{2,3,0}};///body
    glColor3f(body_color[0],body_color[1],body_color[2]);
    quad(v);
    v={{2,2,0},{1.5,2.25,0},{1.5,2.75,0},{2,3,0}};///body - point
    glColor3f(body_color[0],body_color[1],body_color[2]);
    quad(v);
    v={{4,2,0},{4.5,0,0},{3.5,0,0},{2.5,2,0}};///wing
    glColor3f(wing_color[0],wing_color[1],wing_color[2]);
    quad(v);
    v={{4,3,0},{4.5,5,0},{3.5,5,0},{2.5,3,0}};///wing
    glColor3f(wing_color[0],wing_color[1],wing_color[2]);
    quad(v);
    v={{6,3,0},{6,4,0},{5.5,4,0},{5,3,0}};///tail wing
    glColor3f(wing_color[0],wing_color[1],wing_color[2]);
    quad(v);
    v={{6,2,0},{6,1,0},{5.5,1,0},{5,2,0}};///tail wing
    glColor3f(wing_color[0],wing_color[1],wing_color[2]);
    quad(v);
    }
    else {

        GLfloat v[][3]={{2,2,0},{6,2,0},{6,3,0},{2,3,0}};///body
    glColor3f(body_color[0],body_color[1],body_color[2]);
    quad(v);
    v={{6,2,0},{6.5,2.25,0},{6.5,2.75,0},{6,3,0}};///body - point
    glColor3f(body_color[0],body_color[1],body_color[2]);
    quad(v);
    v={{4,2,0},{4.5,0,0},{3.5,0,0},{2.5,2,0}};///wing
    glColor3f(wing_color[0],wing_color[1],wing_color[2]);
    quad(v);
    v={{4,3,0},{4.5,5,0},{3.5,5,0},{2.5,3,0}};///wing
    glColor3f(wing_color[0],wing_color[1],wing_color[2]);
    quad(v);
    v={{6,3,0},{6,4,0},{5.5,4,0},{5,3,0}};///tail wing
    glColor3f(wing_color[0],wing_color[1],wing_color[2]);
    quad(v);
    v={{6,2,0},{6,1,0},{5.5,1,0},{5,2,0}};///tail wing
    glColor3f(wing_color[0],wing_color[1],wing_color[2]);
    quad(v);
    }
}

void insert(float x1,float y1) {
   struct Node* new_node = (struct Node*) malloc(sizeof(struct Node));
   new_node->x = x1;
   new_node->y = y1;
   new_node->next = head;
   head = new_node;
}

void RemInsert(float x1,float y1) {
   struct RemNode* new_node = (struct RemNode*) malloc(sizeof(struct RemNode));
   new_node->x = x1;
   new_node->y = y1;
   new_node->next = Remhead;
   Remhead = new_node;
}

void deleteList(){
   struct Node* current = head;
   struct Node* next;

   while (current != NULL)
   {
       next = current->next;
       free(current);
       current = next;
   }
   head = NULL;
}

void RemdeleteList(){
   struct RemNode* current = Remhead;
   struct RemNode* next;

   while (current != NULL)
   {
       next = current->next;
       free(current);
       current = next;
   }
   Remhead = NULL;
}

void print() {
   struct Node* ptr;
   ptr = head;
   float x1,y1;
   glLineWidth(4);
   //glBegin(GL_POLYGON);
   glBegin(GL_LINE_LOOP);
   while (ptr != NULL) {
      x1=ptr->x;
      y1=ptr->y;
      //printf(" %f , %f \n",x1,y1);
      glVertex3f(x1,y1,0);
      ptr = ptr->next;
   }
   glEnd();
   deleteList();
}

void RemPrint() {
   struct RemNode* ptr;
   ptr = Remhead;
   float x1,y1;
   glLineWidth(4);
   //glBegin(GL_POLYGON);
   glBegin(GL_POINTS);
   while (ptr != NULL) {
      x1=ptr->x;
      y1=ptr->y;
      //printf(" %f , %f \n",x1,y1);
      glVertex3f(x1,y1,0);
      ptr = ptr->next;
   }
   glEnd();
}

void circle(float x,float y,float z,float r,float a=0,float b=360){
    float theta;
    glBegin(GL_POLYGON);
    for(int i=a;i<b;i++){

        theta=i*3.142/180;
        glVertex3f(x+r*cos(theta),y+r*sin(theta),z);
    }

    glEnd();
}

void point(float x,float y,float z=0){
    glBegin(GL_POINTS);
    glVertex3f(x, y, z);
    glEnd();
}

void lines(float x1,float y1,float z1,float x2,float y2,float z2){
    glBegin(GL_LINES);
    glVertex3f(x1,y1,z1);
    glVertex3f(x2,y2,z2);
    glEnd();
}

void asia(){
    insert(121,59);
    insert(122.5,58);
    insert(123,57);
    insert(124,57);
    insert(124,56);
    insert(124.3,54);
    insert(125,52);
    insert(126,51);
    insert(126.4,51);
    insert(127,51.3);
    insert(128,54.9);
    insert(131,58);
    insert(132.8,56);
    insert(133,54);
    insert(135,52);
    insert(136,52);
    insert(137,53);
    insert(138.5,51.5);
    insert(140,54);
    insert(138,56);
    insert(140,58);
    insert(143,58.5);
    insert(141,66);

    insert(145,61);
    insert(145,62);
    insert(146,64);

    insert(145,66.3);
    insert(144.6,67);
    insert(144.5,68);
    insert(143,73.4);
    insert(145,73);
    insert(151,71);
    insert(152,70.9);
    insert(150,75);
    insert(153,74);
    insert(155,76);
    insert(155.2,77.9);
    insert(149,77.4);
    insert(145,77.9);
    insert(141,78);
    insert(133,80);
    insert(131,81);
    insert(127,80);
    insert(128,81);
    insert(123,82);
    insert(121,81);
    insert(117,80);
    insert(114,81);
    insert(114.7,78);
    insert(113,76);
    insert(112,72);
    insert(113,72);
    insert(109,70);
    insert(110,68);
    insert(112,66);
    insert(110,66);
    insert(108.8,66);
    insert(107.2,65);
    insert(107,64);
    insert(109,64);
    insert(111,64);
    insert(111,62);
    insert(112,60);
    insert(112,55);
    insert(113,55);
    insert(115,56);
    insert(116,57);
    insert(117,61);




    print();
}

void northamerica(){
    insert(29.47,70.86);
    insert(31.71,69.13);
    insert(33.95,69.99);
    insert(38.43,69.99);
    insert(40.67,69.13);
    insert(39.55,66.52);
    insert(37.31,63.91);
    insert(36.19,60.43);
    insert(38.43,56.95);
    insert(40.67,55.21);
    insert(42.91,53.47);
    insert(45.14,51.73);
    insert(47.38,51.73);
    insert(49.62,50);
    insert(51.86,50);
    insert(51.86,53.47);
    insert(49.62,53.47);
    insert(51.86,55.21);
    insert((47.38+49.62)/2,55.21);
    insert((45.17+47.38)/2,(53.47+55.21)/2);
    insert(47.38,58.69);
    insert(51.86,58.69);
    insert(54.10,56.95);
    insert(56.34,60.43);
    insert(60.82,63.91);
    insert(63.05,63.91);
    insert(65.29,65.65);
    insert(69.77,67.39);
    insert(67.53,69.13);
    insert(67.53,70.86);
    insert(65.29,(69.13+70.86)/2);
    insert(65.29,70.86);
    insert(63.05,70.86);
    insert(58.58,67.39);
    insert(56.34,69.13);
    insert(56.34,70.86);
    insert(58.58,72.60);
    insert(63.05,72.60);
    insert(60.82,74.34);
    insert(60.82,76.08);
    insert(58.58,74.34);
    insert(56.34,76.08);
    insert(54.10,74.34);
    insert(45.14,74.34);
    insert(40.67,76.08);
    insert(36.19,72.60);
    insert(31.71,74.34);
    insert(31.71,72.60);

    print();
}

void Europe(){
    insert(114.7,78);
    insert(113,76);
    insert(112,72);
    insert(113,72);
    insert(109,70);
    insert(110,68);
    insert(107,67);
    insert(106,66);
    insert(105,69);
    insert(104,65);
    insert(103,65);
    insert(103.5,69);
    insert(102,69);
    insert(101,68);
    insert(101.5,66);
    insert(99,68);
    insert(99.4,67.7);
    insert(101.7,66);
    insert(100,65);
    insert(97,65.3);
    insert(95,65);
    insert(93,64.4);
    insert(91,65);
    insert(89,66);
    insert(89.3,67);
    insert(93,68);
    insert(92,70);
    insert(95,71);
    insert(94,72);
    insert(103,74);
    insert(101,75);
    insert(102,77);
    insert(100,76);
    insert(101,74);
    insert(99,72.4);
    insert(98,73);
    insert(96,73);
    insert(96,75);
    insert(99,78);
    insert(102,80);
    insert(106,78);
    insert(105,77.8);
    insert(104,75);
    insert(109,78);
    insert(111,78.5);
    print();

}

void Africa(){
    insert(111,62);
    insert(112,60);
    insert(110,55);
    insert(112.3,53);
    insert(117,52);
    insert(117,50);
    insert(113,48);
    insert(110,45);
    insert(110.5,41);
    insert(110,40);
    insert(108,38);
    insert(107,34);
    insert(105,32);
    insert(103,31);
    insert(101,31.3);
    insert(99,36);
    insert(97,40);
    insert(99,42);
    insert(96.6,46);
    insert(97,49);
    insert(95,50);
    insert(92,49.5);
    insert(89,50);
    insert(84,52);
    insert(83.7,55);
    insert(85,56);
    insert(91,61);
    insert(93,61);
    insert(97,64);
    insert(98,64);
    insert(98,61);
    insert(103,62);

    print();
}

void GreenLand(){
    insert(85,83);
    insert(89,82.5);
    insert(89,81);
    insert(87,78);
    insert(83,77.5);
    insert(77,74);
    insert(76,76);
    insert(77,80);
    insert(75,80.5);
    insert(73,81);
    insert(71,81.5);
    insert(69,81);
    insert(67,82);
    insert(65,83);
    insert(75,84);
    insert(78,83);



    print();
}

void southamerica(){
    insert(54.10,(50+51.73)/2);
    insert(51.86,51.73);
    insert(51.86,50);
    insert((51.86+54.10)/2,(48.26+50)/2);
    insert(54.10,48.26);
    insert(51.86,44.78);
    insert(54.10,41.30);
    insert((56.34+58.58)/2,(37.82+39.56)/2);
    insert(58.58,32.6);
    insert(60.82,25.65);
    insert(63.05,23.91);
    insert(67.53,23.91);
    insert(65.29,25.65);
    insert(63.05,25.65);
    insert(63.05,30.86);
    insert(65.29,30.86);
    insert((67.53+69.77)/2,34.34);
    insert((67.53+69.77)/2,36.08);
    insert(69.77,(36.08+37.82)/2);
    insert(72.01,41.3);
    insert(74.25,43.04);
    insert(74.25,44.78);
    insert(67.53,46.52);
    insert(65.29,50);
    insert(60.82,51.73);

    print();
}

void australia(){
    insert(145.89,32.6);
    insert((148.13+150.37)/2,32.6);
    insert(150.37,(32.6+34.34)/2);
    insert(154.85,(32.6+34.34)/2);
    insert(157.08,30.86);
    insert((159.32+161.56)/2,30.86);
    insert(163.80,32.6);
    insert((163.80+166.04)/2,34.34);
    insert((163.80+166.04)/2,37.82);
    insert((163.80+161.56)/2,39.56);
    insert((163.80+161.56)/2,43.04);
    insert((159.32+161.56)/2,43.04);
    insert((159.32+161.56)/2,39.56);
    insert((159.32+157.08)/2,43.04);
    insert((154.85+157.08)/2,43.04);
    insert(154.85,41.3);
    insert(152.61,41.3);
    insert(150.37,37.82);
    insert(145.89,36.08);
    print();
}

void check(float x,float y){
    float radius;
    int i;
    for(i=0;i<sizeof(airport);i++){
            if(aeroplane_at[0]!=airport[i][0] && aeroplane_at[1] != airport[i][1]){
        radius=(airport[i][0]-x)*(airport[i][0]-x)+(airport[i][1]-y)*(airport[i][1]-y)-5*5;
        if(radius <=0)
            RemInsert(airport[i][0],airport[i][1]);
            }
    }
}

void check_airport(float x,float y){
    float radius;
    int i,j=0;
    for(i=0;i<sizeof(airport);i++){
            if(aeroplane_at[0]!=airport[i][0] && aeroplane_at[1] != airport[i][1]){
        radius=(airport[i][0]-x)*(airport[i][0]-x)+(airport[i][1]-y)*(airport[i][1]-y)-1*1;
        if(radius <=0){
            airport_selected={airport[i][0],airport[i][1]};
            RemdeleteList();
            RemInsert(airport[i][0],airport[i][1]);
            j++;
            }
            }
    }
    if(j==0)
        airport_selected={0,0};
}

void point_the_airport(float height){
    glColor3f(0.75, 0.16, 0.11);
    //height=0;
    circle(airport_selected[0],airport_selected[1]+height+2.5,0,3,0,180);
    GLfloat v[][3]={{airport_selected[0],airport_selected[1]+height},{airport_selected[0]-3,airport_selected[1]+height+2.5},
                    {airport_selected[0]+3,airport_selected[1]+height+2.5},{airport_selected[0]+3,airport_selected[1]+height+2.5}};
    quad(v);
}

void display_the_map(){

    glClearColor(0.00, 0.79, 0.89,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /**********************
        global frame
        ***********************/
        glColor3f(0.16, 0.16, 0.16);
        circle(50,50,0,40);
        circle(150,50,0,40);
        GLfloat v[][3]={{50,90,0},{50,10,0},{150,10,0},{150,90,0}};
        quad(v);

        /**********************
        print the world map
        ***********************/
        glColor3f(0.58, 0.84, 0.26);
        asia();

        glColor3f(0.15, 0.73, 0.96);
        Europe();

        glColor3f(0.99, 0.11, 0.15);
        northamerica();

        glColor3f(0.99, 0.62, 0.13);
        southamerica();

        glColor3f(1.00, 0.87, 0.34);
        Africa();

        glColor3f(0.00, 0.62, 0.29);
        australia();

        glColor3f(1,1,1);
        GreenLand();

        /******************
        back button
        *********************/
        glColor3f(1,0,0);
        v={{180,95,0},{195,95,0},{195,90,0},{180,90,0}};
        quad(v);
        glColor3f(0,0,0);
        setFont(GLUT_BITMAP_TIMES_ROMAN_24);
        drawstring(182,92,0,"BACK");


        /********************
        LETS TRAVEL BUTTON
        ******************/
        glColor3f(0.86, 0.29, 0.25);
        v={{176,15,0},{195,15,0},{195,10,0},{176,10,0}};
        quad(v);
        glColor3f(0,0,0);
        setFont(GLUT_BITMAP_TIMES_ROMAN_24);
        drawstring(182,12,0,"FLY");
}

void error_message_display(){
    display_the_map();

    glColor3f(0.78, 0.78, 0.78);
    glBegin(GL_POLYGON);
        glVertex3f(75,55,0);
        glVertex3f(125,55,0);
        glVertex3f(125,60,0);
        glVertex3f(75,60,0);
    glEnd();

    glColor3f(0, 0, 0);
    setFont(GLUT_BITMAP_TIMES_ROMAN_24);
    drawstring(76.571 , 56.250,0,"Error");

    glColor3f(0.94, 0.94, 0.94);
    glBegin(GL_POLYGON);
        glVertex3f(75,55,0);
        glVertex3f(125,55,0);
        glVertex3f(125,45,0);
        glVertex3f(75,45,0);
    glEnd();

    glColor3f(0, 0, 0);
    setFont(GLUT_BITMAP_TIMES_ROMAN_24);
    drawstring(77.000 , 51,0,"Select an airport!!");

    glColor3f(1,0,0);
    glBegin(GL_POLYGON);
        glVertex3f(116,46.5,0);
        glVertex3f(123,46.5,0);
        glVertex3f(123,50,0);
        glVertex3f(116,50,0);
    glEnd();

    glColor3f(0, 0, 0);
    setFont(GLUT_BITMAP_HELVETICA_18);
    drawstring(118.000 , 47.375,0,"Ok");
}

void F(){
        insert(0,0);
        insert(0,5);
        insert(3,5);
        insert(3,4);
        insert(1,4);
        insert(1,3);
        insert(2.5,3);
        insert(2.5,2);
        insert(1,2);
        insert(1,0);
        print();
    //glEnd();

}

void L(){
    insert(0,0);
    insert(0,5);
    insert(1,5);
    insert(1,1);
    insert(3,1);
    insert(3,0);
    print();
}

void I(){
    insert(0,0);
    insert(0,5);
    insert(1,5);
    insert(1,0);
    print();
}

void G(){
    insert(0,0);
    insert(0,5);
    insert(3,5);
    insert(3,4);
    insert(1,4);
    insert(1,1);
    insert(2,1);
    insert(2,2);
    insert(3,2);
    insert(3,0);
    print();
}

void H(){
    insert(0,0);
    insert(0,5);
    insert(1,5);
    insert(1,3);
    insert(2,3);
    insert(2,5);
    insert(3,5);
    insert(3,0);
    insert(2,0);
    insert(2,2);
    insert(1,2);
    insert(1,0);
    print();
}

void T(){
    insert(0,4);
    insert(0,5);
    insert(3,5);
    insert(3,4);
    insert(2,4);
    insert(2,0);
    insert(1,0);
    insert(1,4);
    print();
}

void FLIGHT(){
                glPushMatrix();
                //glTranslatef(-20,-10,0);
                glTranslatef(25,76.5,0);
                glScalef(2.5,2.5,0);
                F();
                glPushMatrix();
                glTranslatef(-1.5,-2.5,0);
                glTranslatef(5.2,2.5,0);
                L();
                glPopMatrix();
                glPushMatrix();
                glTranslatef(-0.5,-2.5,0);
                glTranslatef(8,2.5,0);
                I();
                glPopMatrix();
                glPushMatrix();
                glTranslatef(-1.5,-2.5,0);
                glTranslatef(11,2.5,0);
                G();
                glPopMatrix();
                glPushMatrix();
                glTranslatef(-1.5,-2.5,0);
                glTranslatef(15,2.5,0);
                H();
                glPopMatrix();
                glPushMatrix();
                glTranslatef(-1.5,-2.5,0);
                glTranslatef(18.5,2.5,0);
                T();
                glPopMatrix();
                glPopMatrix();
}

void R(){
    insert(0,0);
    insert(0,5);
    insert(3,5);
    insert(3,3);
    insert(1,3);
    insert(1,3);
    insert(3,0);
    insert(2,0);
    insert(1,2);
    insert(1,0);
    print();
}

void A(){
    insert(0,0);
    insert(0,5);
    insert(3,5);
    insert(3,0);
    insert(2,0);
    insert(2,2);
    insert(1,2);
    insert(1,0);
    print();

    insert(1,4);
    insert(2,4);
    insert(2,3);
    insert(1,3);
    print();

}

void J(){
    insert(0,4);
    insert(0,5);
    insert(3,5);
    insert(3,4);
    insert(2,4);
    insert(2,0);
    insert(0,0);
    insert(0,1);
    insert(1,1);
    insert(1,4);
    print();
}

void E(){
        insert(0,0);
        insert(0,5);
        insert(3,5);
        insert(3,4);
        insert(1,4);
        insert(1,3);
        insert(2.5,3);
        insert(2.5,2);
        insert(1,2);
        insert(1,1);
        insert(3,1);
        insert(3,0);
        print();
}

void C(){
    insert(0,0);
        insert(0,5);
        insert(3,5);
        insert(3,4);
        insert(1,4);
        insert(1,3);
        insert(1,2);
        insert(1,1);
        insert(3,1);
        insert(3,0);
        print();
}

void O(){
    insert(0,0);
    insert(0,5);
    insert(3,5);
    insert(3,0);
    print();

    insert(1,1);
    insert(1,4);
    insert(2,4);
    insert(2,1);
    print();
}

void Y(){
    insert(0,5);
    insert(1,5);
    insert(1.5,3);
    insert(2,5);
    insert(3,5);
    insert(2,2.5);
    insert(2,0);
    insert(1,0);
    insert(1,2.5);
    print();
}

void TRAJECTORY(){
    glPushMatrix();
    glTranslatef(85,76.5,0);

    glScalef(2.5,2.5,0);

    T();
    glPushMatrix();
    glTranslatef(-1.5,-2.5,0);
    glTranslatef(5.2,2.5,0);
    R();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-1.5,-2.5,0);
    glTranslatef(9.2,2.5,0);
    A();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-1.5,-2.5,0);
    glTranslatef(13.2,2.5,0);
    J();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-1.5,-2.5,0);
    glTranslatef(17,2.5,0);
    E();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-1.5,-2.5,0);
    glTranslatef(20.9,2.5,0);
    C();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-1.5,-2.5,0);
    glTranslatef(24.8,2.5,0);
    T();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-1.5,-2.5,0);
    glTranslatef(28.7,2.5,0);
    O();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-1.5,-2.5,0);
    glTranslatef(32.6,2.5,0);
    R();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-1.5,-2.5,0);
    glTranslatef(36.5,2.5,0);
    Y();
    glPopMatrix();

    glPopMatrix();
}

void S(){
    insert(0,0);
    insert(0,1);
    insert(2,1);
    insert(2,2);
    insert(0,2);
    insert(0,5);
    insert(3,5);
    insert(3,4);
    insert(1,4);
    insert(1,3);
    insert(3,3);
    insert(3,0);
    print();
}

void M(){
glPushMatrix();
    I();

    insert(1,4);
    insert(1,5);
    insert(2.5,4);
    insert(3.5,5);
    insert(3.5,4);
    insert(2.5,3);
    print();
    glPushMatrix();
    glTranslatef(-0.5,-2.5,0);
    glTranslatef(4,2.5,0);
    I();
    glPopMatrix();
glPopMatrix();
}

void U(){
    insert(0,0);
    insert(0,5);
    insert(1,5);
    insert(1,1);
    insert(2,1);
    insert(2,5);
    insert(3,5);
    insert(3,0);
    print();
}

void SIMULATOR(){
    glPushMatrix();

    glTranslatef(58,59,0);
    glScalef(2.5,2.5,0);

    S();
    glPushMatrix();
                glTranslatef(-1.5,-2.5,0);
                glTranslatef(5.5,2.5,0);
                I();
                glPopMatrix();
    glPushMatrix();
                glTranslatef(-1.5,-2.5,0);
                glTranslatef(7.5,2.5,0);
                M();
                glPopMatrix();
    glPushMatrix();
                glTranslatef(-1.5,-2.5,0);
                glTranslatef(13,2.5,0);
                U();
                glPopMatrix();
    glPushMatrix();
                glTranslatef(-1.5,-2.5,0);
                glTranslatef(17,2.5,0);
                L();
                glPopMatrix();
    glPushMatrix();
                glTranslatef(-1.5,-2.5,0);
                glTranslatef(21,2.5,0);
                A();
                glPopMatrix();
    glPushMatrix();
                glTranslatef(-1.5,-2.5,0);
                glTranslatef(25,2.5,0);
                T();
                glPopMatrix();
    glPushMatrix();
                glTranslatef(-1.5,-2.5,0);
                glTranslatef(29,2.5,0);
                O();
                glPopMatrix();
    glPushMatrix();
                glTranslatef(-1.5,-2.5,0);
                glTranslatef(33,2.5,0);
                R();
                glPopMatrix();

    glPopMatrix();
}

void scene1(){
            //glClearColor(1,1,1,1);
            glClearColor(0.00, 0.79, 0.89,1);
            //glClearColor(1,0,0,1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
               /* glBegin(GL_POLYGON);

                    glColor3f(0,0.9,0);
                    glVertex3f(0,0,0);

                    glColor3f(0,0.9,0);
                    glVertex3f(200,0,0);

                    glColor3f(0.00, 0.79, 0.89);
                    glVertex3f(200,100,0);

                    glColor3f(0.00, 0.79, 0.89);
                    glVertex3f(0,100,0);
                glEnd();*/


            /****************
            header
            ************************/
                if((int)(color_controller)%2==0)
                    glColor3f(1,1,1);
                else
                    glColor3f(0.5,0.5,0.5);
                FLIGHT();
                TRAJECTORY();
                SIMULATOR();
                color_controller=color_controller+0.25;
                if(color_controller>100)
                    color_controller=0;



            /***********************
            button LETS FLY
            ************************/
            //glColor3f(0.49, 0.70, 0.27);
            glColor3f(0, 0, 0.5);
            //glColor3f(0.5,0.5,0.5);
            GLfloat v[][3]={{60,10,0},{60,20,0},{140,20,0},{140,10,0}};
            quad(v);
            //glColor3f(0.5,0.5,0.5);
            glColor3f(1,1,1);
            glPushMatrix();
            //glScalef(2,2,0);
            //glColor3f(1,0,0);
            setFont(GLUT_BITMAP_TIMES_ROMAN_24);
            drawstring(93,14,0,"Let's FLY");
            glPopMatrix();




            /*******************
            introduction
            *****************/
            //glColor3f(0.49, 0.70, 0.27);
            glColor3f(1,1,1);
            v={{75,50,0},{75,40,0},{125,40,0},{125,50,0}};
            quad(v);
            //glColor3f(0.5, 0.5, 0.5);
            glColor3f(1,0,0);
            setFont(GLUT_BITMAP_TIMES_ROMAN_24);
            drawstring(91,44,0,"Introduction");

            /*****************
            setting
            *****************/
            //glColor3f(0.49, 0.70, 0.27);
            glColor3f(1,1,1);
            v={{75,35,0},{75,25,0},{125,25,0},{125,35,0}};
            quad(v);
            //glColor3f(0.5, 0.5, 0.5);
            glColor3f(1,0,0);
            setFont(GLUT_BITMAP_TIMES_ROMAN_24);
            drawstring(85,29.5,0,"Customize your Plane");



}

void scene2(){

        display_the_map();

        /**********************
        to print the airports inside the radius that has been selected
        to point the selected airport
        ***********************/
        if(!(airport_selected[0]==0 && airport_selected[1] ==0))
            point_the_airport((pointSize/3));
        //glColor3f(0.17, 0.69, 1.00);
        glColor3f(1,1,1);
        pointSize=(pointSize+1)%7;
        glPointSize(pointSize);             /****************    0-6 point size *********************/
        RemPrint();

        glColor3f(0.49, 0.70, 0.27);
        glBegin(GL_POINTS);
        glVertex3f(aeroplane_at[0],aeroplane_at[1],0);
        glEnd();

        glPushMatrix();
        glTranslatef(-4,-2.5,0);
        glTranslatef(aeroplane_at[0],aeroplane_at[1],0);
        drawAeroplane();
        glPopMatrix();

}

void scene3(){
        display_the_map();

        if(dnt_repeat==-1)
    {
        glPointSize(6);
    dx=abs(xend-x0);
    dy=abs(yend-y00);
    m=(float)(dy)/(float)(dx);
    if(m>=1)
        LineNegSlope();
    else
        LinePosSlope();
    PathPrint();
    printf("%d",co);
    dnt_repeat++;

    }
    else {


            glPointSize(6);
            glBegin(GL_POINTS);
                glColor3f(0,1,0);
                glVertex3f(aeroplane_at[0],aeroplane_at[1],0);
                glColor3f(1,1,0);
                glVertex3f(airport_selected[0],airport_selected[1],0);
            glEnd();



    glPushMatrix();
    glTranslatef(-4,-2.5,0);

    /*if(rot==1)
            glRotatef(180,0,0,1);*/
    if(swap1==0)
        glTranslatef((GLfloat)path_follow[dnt_repeat][0],(GLfloat)path_follow[dnt_repeat][1],0);
    else
        glTranslatef((GLfloat)path_follow[co-dnt_repeat-1][0],(GLfloat)path_follow[co-dnt_repeat-1][1],0);

        glPushMatrix();

        /*rot=0;
        if(rot==1){
            glTranslatef(-4,-2.5,0);
            glRotatef(180,0,0,1);
            if(swap1==0)
                glTranslatef((GLfloat)path_follow[dnt_repeat][0],(GLfloat)path_follow[dnt_repeat][1],0);
            else
                glTranslatef((GLfloat)path_follow[co-dnt_repeat-1][0],(GLfloat)path_follow[co-dnt_repeat-1][1],0);
        }*/

        drawAeroplane();

        glPopMatrix();
    glPopMatrix();
    if(dnt_repeat<co)
        dnt_repeat++;
    else{
        displayMenu=1;

        aeroplane_at[0]=airport_selected[0];
        aeroplane_at[1]=airport_selected[1];
        airport_selected[0]=0;
        airport_selected[1]=0;
        rot=0;
    }
    }


}

void scene4(){


    //glClearColor(0.00, 0.79, 0.89,1);
    glClearColor(0.00, 0.79, 0.89,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    //glColor3f(0.5, 0.5, 0.5);
    glColor3f(1.00, 0.33, 0.33);
    setFont(GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(0,0,0.5);
    drawstring(50,85,0,"DEPARTMENT OF COMPUTER SCIENCE AND ENGINEERING");
    glColor3f(1.00, 0.33, 0.33);
    glColor3f(0,0,0.5);
    drawstring(50,80,0,"ST JOSEPH ENGINEERING COLLEGE, MANGALORE-575028");

    glColor3f(1, 1, 1);
    drawstring(65,60,0,"COMPUTER GRAPHICS PROJECT ON:");
    glColor3f(0, 0, 0);
    glColor3f(1,0,0);
    drawstring(68,55,0,"FLIGHT TRAJECTORY SIMULATOR");


    glColor3f(1, 1, 1);
    drawstring(130,35,0,"SUBMITTED BY:");
    glColor3f(1, 1, 1);
    glColor3f(0,0,0.5);
    drawstring(130,30,0,"SUSHAN SAPALIGA: 4S016CS113");
    glColor3f(1, 1, 1);
    glColor3f(0,0,0.5);
    drawstring(130,25,0,"WINSTON D'SOUZA: 4SO16CS119");
    glColor3f(1, 1, 1);
    drawstring(130,15,0,"UNDER THE GUIDANCE OF:");
    glColor3f(1, 1, 1);
    glColor3f(0,0,0.5);
    drawstring(130,10,0,"NISHA ROCHE");

    glColor3f(1,0,0);
    glBegin(GL_POLYGON);
        glVertex3f(195,95,0);
        glVertex3f(195,90,0);
        glVertex3f(175,90,0);
        glVertex3f(175,95,0);
    glEnd();
    glColor3f(0, 0, 0);
    drawstring(180,92,0,"BACK");
}

void feild(double hallo){

    glColor3f(0.94, 0.94, 0.94);
    glBegin(GL_POLYGON);   ///-
        glVertex3f(20,77,0);
        glVertex3f(25,77,0);
        glVertex3f(25,73,0);
        glVertex3f(20,73,0);
    glEnd();
    glColor3f(0, 0, 0);
    drawstring(21,74,0,"-");

    glColor3f(0.94, 0.94, 0.94);
    glBegin(GL_POLYGON);   ///feild
        glVertex3f(27,77,0);
        glVertex3f(35.7,77,0);
        glVertex3f(35.7,73,0);
        glVertex3f(27,73,0);
    glEnd();
    char hallo1[10];
    //itoa(hallo, hallo1, 10);
    gcvt(hallo,10,hallo1);
    glColor3f(0, 0, 0);
    drawstring(28,74,0,hallo1);

    glColor3f(0.00, 0.79, 0.89);
    glBegin(GL_POLYGON);
    glVertex3f(35.7,77,0);
    glVertex3f(35.7,73,0);
    glVertex3f(50,73,0);
    glVertex3f(50,77,0);
    glEnd();

    glColor3f(0.94, 0.94, 0.94);
    glBegin(GL_POLYGON);   ///+
        glVertex3f(37,77,0);
        glVertex3f(42,77,0);
        glVertex3f(42,73,0);
        glVertex3f(37,73,0);
    glEnd();
    glColor3f(0, 0, 0);
    drawstring(38,74,0,"+");
}

void scene5(){

    glClearColor(0.00, 0.79, 0.89,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



    setFont(GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(1,0,0);
    glBegin(GL_POLYGON);
        glVertex3f(195,95,0);
        glVertex3f(195,90,0);
        glVertex3f(175,90,0);
        glVertex3f(175,95,0);
    glEnd();
    glColor3f(0, 0, 0);
    drawstring(180,92,0,"BACK");

    glColor3f(01, 01, 01);
    //drawstring(10,90,0,"SETTING:");

    glColor3f(01, 01, 01);
    drawstring(10,80,0,"SPEED:");
    glColor3f(0.94, 0.94, 0.94);
    glBegin(GL_POLYGON);   ///-
        glVertex3f(20,77,0);
        glVertex3f(25,77,0);
        glVertex3f(25,73,0);
        glVertex3f(20,73,0);
    glEnd();
    glColor3f(0, 0, 0);
    drawstring(21,74,0,"-");

    glColor3f(0.94, 0.94, 0.94);
    glBegin(GL_POLYGON);   ///feild
        glVertex3f(27,77,0);
        glVertex3f(35,77,0);
        glVertex3f(35,73,0);
        glVertex3f(27,73,0);
    glEnd();
    char speed1[10];
    itoa(speed, speed1, 10);
    glColor3f(0, 0, 0);
    drawstring(28,74,0,speed1);

    glColor3f(0.94, 0.94, 0.94);
    glBegin(GL_POLYGON);   ///+
        glVertex3f(37,77,0);
        glVertex3f(42,77,0);
        glVertex3f(42,73,0);
        glVertex3f(37,73,0);
    glEnd();
    glColor3f(0, 0, 0);
    drawstring(38,74,0,"+");

    /*********************************************/
    glColor3f(01, 01, 01);
    drawstring(10,65,0,"COLOR YOUR PLANE:");

    glColor3f(01, 01, 01);
    drawstring(15,60,0,"BODY:");

    glColor3f(01, 0, 0);
    drawstring(20,55,0,"RED:");
    glColor3f(0, 0.5, 0);
    drawstring(20,50,0,"GREEN:");
    glColor3f(0, 0, 01);
    drawstring(20,45,0,"BLUE:");

    glPushMatrix();///R
    glTranslatef(-20,-73,0);
    glTranslatef(34,55,0);
    feild(body_color[0]);
    glPopMatrix();

    glPushMatrix();///G
    glTranslatef(-20,-73,0);
    glTranslatef(34,50,0);
    feild(body_color[1]);
    glPopMatrix();

    glPushMatrix();///B
    glTranslatef(-20,-73,0);
    glTranslatef(34,45,0);
    feild(body_color[2]);
    glPopMatrix();



    glColor3f(01, 01, 01);
    drawstring(15,40,0,"WINGS:");

    glColor3f(01, 0, 0);
    drawstring(20,35,0,"RED:");
    glColor3f(0, 0.5, 0);
    drawstring(20,30,0,"GREEN:");
    glColor3f(0, 0, 01);
    drawstring(20,25,0,"BLUE:");

    glPushMatrix();///R
    glTranslatef(-20,-73,0);
    glTranslatef(34,35,0);
    feild(wing_color[0]);
    glPopMatrix();

    glPushMatrix();///G
    glTranslatef(-20,-73,0);
    glTranslatef(34,30,0);
    feild(wing_color[1]);
    glPopMatrix();

    glPushMatrix();///B
    glTranslatef(-20,-73,0);
    glTranslatef(34,25,0);
    feild(wing_color[2]);
    glPopMatrix();


    ///display the modified plane
    glColor3f(0,0,0);
    glBegin(GL_POLYGON);
    glVertex3f(90,55,0);
    glVertex3f(115,55,0);
    glVertex3f(115,30,0);
    glVertex3f(90,30,0);
    glEnd();

    glColor3f(1,1,0);
    insert(90,55);
    insert(115,55);
    insert(115,30);
    insert(90,30);
    print();



    glPushMatrix();
    glTranslatef(-4,-2.5,0);
    glTranslatef(99,40,0);
    glScalef(2,2,0);
    drawAeroplane();
    glPopMatrix();



}

static void display(void){
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glLoadIdentity();


    /************************
    to display menu
    ***************************/
    if(displayMenu==0){

        scene1();
        Sleep(75);

    }
    else if(displayMenu==1){

        scene2();
        Sleep(75);

    }
    else if(displayMenu==2){
            if( ( airport_selected[0]==0 ) && (airport_selected[1]==0) ){
                /*************** error-message *********************/
                error_message=1;
                error_message_display();

            }

            else {

                scene3();

            }
        Sleep(200-speed);
    }
    else if(displayMenu==3){
        scene4();
        Sleep(25);
    }
    else if(displayMenu==4){
        scene5();
        Sleep(25);
    }



    /*int i;
    if(!i1)
    for(i=0;i<sizeof(airport);i++)
        if( (airport[i][0]<=20 || airport[i][0]>=70 ) && (airport[i][1]<=10 || airport[i][1]>=90)){
        point(airport[i][0],airport[i][1]);
        printf("%f , %f \n",airport[i][0],airport[i][1]);
        }
        i1++;*/



    glFlush();
    glutSwapBuffers();
    glutPostRedisplay();

}

void mouse(int button, int state, int x,int y){
    float x1,y1;
    x1=(0.1428571429)*(float)x;
    y1=(800-(float)y)*(0.125);
    printf("%d , %d\n",x,y);
    printf("%0.3f , %0.3f\n",x1,y1);

    /*if(!recal){
        recal=1;
        glutPostRedisplay();
        } */

    if(displayMenu==0){
        if( x1>=60 && x1<=140 && y1 >=10 && y1<=20)/*********** lets fly button check *********/
            displayMenu=1;
        /*else if(x1>=30 && x1<=50 && y1>=30 && y1<=50){ /*********** box1 check *********
            aeroplane[0]=1;
            aeroplane[1]=60;
        }
        else if(x1>=70 && x1<=90 && y1>=30 && y1<=50){/*********** box2 check *********
            aeroplane[0]=2;
            aeroplane[1]=80;
        }
        else if(x1>=110 && x1<=130 && y1>=30 && y1<=50){/*********** box3 check *********
            aeroplane[0]=3;
            aeroplane[1]=100;
        }
        else if(x1>=150 && x1<=170 && y1>=30 && y1<=50){/*********** box4 check *********
            aeroplane[0]=4;
            aeroplane[1]=120;
        }*/
        else if(x1>=75 && x1<=125 && y1 >=40 && y1<=50){/*****************introduction***************/
            displayMenu=3;
        }
        else if(x1>=75 && x1<=125 && y1 >=25 && y1<=35){/*******************setting************/
            displayMenu=4;
        }

    }
    else if(displayMenu==1){
        if( x1>=180 && x1<=195 && y1 >=90 && y1<=95){ /********** back button check ***************/
                displayMenu=0;
                RemdeleteList();
                airport_selected={0,0};
            }
            else if(x1>=176 && x1<=195 && y1>=10 && y1<=15){
                    /**************  fly the plane  ****************/
                    displayMenu=2;
                    dnt_repeat=-1;
                    swap1=0;
                   // x0=(GLint)aeroplane_at[0];
                    //y00=(GLint)aeroplane_at[1];
                    x0=(GLint)airport_selected[0];
                    y00=(GLint)airport_selected[1];
                    xend=(GLint)aeroplane_at[0];
                    yend=(GLint)aeroplane_at[1];
                    if(xend<x0)
                        rot=1;
                    //xend=(GLint)airport_selected[0];
                    //yend=(GLint)airport_selected[1];

            }
            else{ /********************* airport check **************/
                check_airport(x1,y1);
                if(airport_selected[0]==0 && airport_selected[1] ==0){
                        RemdeleteList();
                        check(x1,y1);

                        }
                }
    }
    else if(displayMenu==2 && error_message==1 ){
        if(x1>=116 && x1<= 123 && y1>=46.5 && y1<=50){
            error_message=0;
            displayMenu=1;
        }
    }
    else if(displayMenu==3){
        if(x1>=175 && x1<=195 && y1>=90 && y1<=95){
            displayMenu=0;
        }
    }
    else if(displayMenu==4){
        if(x1>=175 && x1<=195 && y1>=90 && y1<=95){///back
            displayMenu=0;
        }
        else if(x1>=20 && x1<=25 && y1>=73 && y1<=77){///speed - -
            if(speed>0)
                speed--;
        }
        else if(x1>=37 && x1<=42 && y1>=73 && y1<=77){/// speed- +
            if(speed<200)
                speed++;
        }
        else if(x1>=34 && x1<=38 && y1>=55 && y1<=59){///body red - -
            if(body_color[0]>0.11)
                body_color[0]=body_color[0]-0.01;
        }
        else if(x1>=51 && x1<=55 && y1>=55 && y1<=59){/// body red - +
            if(body_color[0]<1.00)
                body_color[0]=body_color[0]+0.01;
        }
        else if(x1>=34 && x1<=38 && y1>=50 && y1<=54){///body green - -
            if(body_color[1]>0.11)
                body_color[1]=body_color[1]-0.01;
        }
        else if(x1>=51 && x1<=55 && y1>=50 && y1<=54){/// body green - +
            if(body_color[1]<1.00)
                body_color[1]=body_color[1]+0.01;
        }
        else if(x1>=34 && x1<=38 && y1>=45 && y1<=49){///body blue - -
            if(body_color[2]>0.11)
                body_color[2]=body_color[2]-0.01;
        }
        else if(x1>=51 && x1<=55 && y1>=45 && y1<=49){/// body blue - +
            if(body_color[2]<1.00)
                body_color[2]=body_color[2]+0.01;
        }
        else if(x1>=34 && x1<=38 && y1>=35 && y1<=39){///wing red - -
            if(wing_color[0]>0.11)
                wing_color[0]=wing_color[0]-0.01;
        }
        else if(x1>=51 && x1<=55 && y1>=35 && y1<=39){///wing red - +
            if(wing_color[0]<1.00)
                wing_color[0]=wing_color[0]+0.01;
        }
        else if(x1>=34 && x1<=38 && y1>=30 && y1<=34){///wing green - -
            if(wing_color[1]>0.11)
                wing_color[1]=wing_color[1]-0.01;
        }
        else if(x1>=51 && x1<=55 && y1>=30 && y1<=34){///wing green - +
            if(wing_color[1]<1.00)
                wing_color[1]=wing_color[1]+0.01;
        }
        else if(x1>=34 && x1<=38 && y1>=25 && y1<=29){///wing blue - -
            if(wing_color[2]>0.11)
                wing_color[2]=wing_color[2]-0.01;
        }
        else if(x1>=51 && x1<=55 && y1>=25 && y1<=29){///wing blue - +
            if(wing_color[2]<1.00)
                wing_color[2]=wing_color[2]+0.01;
        }
    }
}

void windresized(int w,int h){
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(w<=h)
        glOrtho(0,200,0*(GLfloat)h/(GLfloat)w,100*(GLfloat)h/(GLfloat)w,0,10);
    else
        glOrtho(0*(GLfloat)w/(GLfloat)h,200*(GLfloat)w/(GLfloat)h,0,100,0,10);
    glMatrixMode(GL_MODELVIEW);
}

void init(){
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
glOrtho(0,200,0,100,0,10);
glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitWindowSize(1400,800);
    glutInitWindowPosition(50,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("world map");

    glutDisplayFunc(display);
    init();

    glutMouseFunc(mouse);

    glutMainLoop();

return 0;
}
