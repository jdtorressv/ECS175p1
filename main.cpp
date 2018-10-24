#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

//global variables
float *PixelBuffer;
float *PolygonBuffer;
vector<vector<double>> vArr;
int wWidth;
int wLength;

//Fuctions
void display();  
inline int roundOff(const double a) {return (int)(a+0.5);}
void makePix(int x, int y, int pid);
void copyBuffer(int pid);
void clearPixelBuffer(); 
void clearPolygonBuffer(int pid); 
void lineDrawRaster();
inline void mainMenu(int pid) {;}
void scaleMenu(int pid);
void rotateMenu(int pid);
void translateMenu(int pid);
void clippingMenu(int pid);
void clip(int points[10][2], int &vertices,int x1, int y1, int x2, int y2);
int xInter(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
int yInter(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);

int main(int argc, char *argv[])
{
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); // These are the default values 
        if (argc != 3) {
                cout << "Usage: p1 <window width> <window length>\n";
                exit(1);
        }
        wWidth = atoi(argv[1]);
        wLength = atoi(argv[2]);
        glutInitWindowSize(wWidth, wLength);
        glutInitWindowPosition(100, 100);
        int MainWindow = glutCreateWindow("Polygons");
        glClearColor(0, 0, 0, 0); //clears the buffer of OpenGL, sets a black background
        int scale_menu, rotate_menu, translate_menu, clipping_menu;//For use in graphical menu

        vector<double> v; // The main V 
	double num; 
	fstream file;
        file.open("inputFile.txt");

        if (!file) {
                cerr << "Unable to open file\n";
		exit(1);   
        }
        while (file >> num)
                v.push_back(num); //Initial vector for all polygons
	file.close(); 

	auto vpoint = v.begin(); 
        int polyTotalForBuffer = (int)*vpoint;
	
      	for (int i = 0; i < polyTotalForBuffer; i++) {
		vector<double> Vx; 
		vArr.push_back(Vx);  
		double vertices = *(++vpoint);
		vArr.at(i).push_back(vertices); 
		for (int j = 0; j < vertices; j++) { 
			vArr.at(i).push_back(*(++vpoint)); 
			vArr.at(i).push_back(*(++vpoint)); 
		}	
	}

        // Prepping area before being copied to Pixel Buffer 
        PolygonBuffer = new float[polyTotalForBuffer*wWidth*wLength*3]();
	
	//Buffer drawn to screen
        PixelBuffer = new float[wWidth * wLength * 3]();


	//Line Draw and Rasterize the original polygons! 
	lineDrawRaster(); 	


        // Offer the user opportunities to transform! 
        translate_menu = glutCreateMenu(translateMenu);
		glutAddMenuEntry("Square", 0); 
		glutAddMenuEntry("Triangle", 1); 
		glutAddMenuEntry("Pentagon", 2); 

        scale_menu = glutCreateMenu(scaleMenu);
                glutAddMenuEntry("Square", 0);
                glutAddMenuEntry("Triangle", 1);
                glutAddMenuEntry("Pentagon", 2);

        rotate_menu = glutCreateMenu(rotateMenu);
                glutAddMenuEntry("Square", 0);
                glutAddMenuEntry("Triangle", 1);
                glutAddMenuEntry("Pentagon", 2);

        clipping_menu = glutCreateMenu(clippingMenu); 
		glutAddMenuEntry("Square", 0);
                glutAddMenuEntry("Triangle", 1);
                glutAddMenuEntry("Pentagon", 2);
	
	glutCreateMenu(mainMenu);
                glutAddSubMenu("Translate", translate_menu);
                glutAddSubMenu("Scale", scale_menu);
                glutAddSubMenu("Rotate", rotate_menu);
		glutAddSubMenu("Clipping", clipping_menu); 
        glutAttachMenu(GLUT_RIGHT_BUTTON);

       
        glutDisplayFunc(display);


        glutMainLoop();//main display loop, will display until terminate

        return 0;
}

void lineDrawRaster() 
{ 
	clearPixelBuffer(); 
        int polyCount = 0; 
        int polyTotal = vArr.size(); //Works no matter what sort of vector was passed, one if particular, all if all 

        string Alg;
        cout << "Please specify DDA with \"d/D\" or Bresenham with \"B/b\"\n";
        cin >> Alg;
        while (Alg != "d" && Alg != "D" && Alg != "B" && Alg != "b") {
                cout << "Specify DDA with \"d/D\" or Bresenham with \"B/b\"\n";
                cin >> Alg;
        }
        /*In the main vector from the input file, the data appears as:
        [3,4,0,0,100,0,100,100,0,100,3,300,0,300,100,200,0,5,100,200,300,200,300,300,200,400,100,300,100,200]*/
        while (polyCount < polyTotal) { 
		int pid = polyCount;
       		clearPolygonBuffer(pid); 		
                int lineTotal;
                double maxExtrema = 0;
                double minExtrema = wLength;
                lineTotal = vArr.at(pid).at(0);
           	int rover = 1; 
                for (int line = 0; line < lineTotal; line++) {
                        double x0 = vArr.at(pid).at(1+line*2);
                        double y0 = vArr.at(pid).at(2+line*2); 
                        if (y0 > maxExtrema)
                                maxExtrema = y0;
                        if (y0 < minExtrema)
                                minExtrema = y0;
                        double xend, yend;
                        if (line == lineTotal-1) {
                                xend = vArr.at(pid).at(1);
                                yend = vArr.at(pid).at(2);
                                if (yend > maxExtrema)
                                        maxExtrema = yend;
                                if (yend < minExtrema)
                                        minExtrema = yend;
                        }
                        else {
                                xend = vArr.at(pid).at(3+line*2);
                                yend = vArr.at(pid).at(4+line*2);
                                if (yend > maxExtrema)
                                        maxExtrema = yend;
                                if (yend < minExtrema)
                                        minExtrema = yend;
                        }
                        //BEGIN DDA
                        if (Alg == "d" || Alg == "D") {
                                int dx = xend - x0, dy = yend - y0, steps, k;
                                double xinc, yinc, x = x0, y = y0;
                                if (fabs(dx) > fabs(dy))
                                        steps = fabs(dx);
                                else
                                        steps = fabs(dy);
                                	xinc = (double)dx/(double)steps;
                                yinc = (double)dy/(double)steps;
                                makePix(roundOff(x), roundOff(y), pid);
                                for (k = 0; k < steps; k++) {
                                        x += xinc;
                                        y += yinc;
                                        makePix(roundOff(x), roundOff(y), pid);
                                }
                        }//END DDA
                        //BEGIN BRESENHAM
                        else {
                                if (fabs(yend - y0) < fabs(xend - x0)) {
                                        if (x0 > xend) {
                                                double dx = x0 - xend;
                                                double dy = y0 - yend;
                                                double yi = 1;
                                                if (dy < 0) {
                                                        yi = -1;
                                                        dy = -1*dy;
                                                }
                                                double twoDyMinusDx = 2*dy - dx;
                                                double y = yend;
                                                double x = xend;
                                                while (x <= x0) {
                                                        makePix(roundOff(x),roundOff(y), pid);
                                                        if (twoDyMinusDx > 0) {
                                                                y = y + yi;
                                                                twoDyMinusDx -= 2*dx;
                                                        }
                                                        twoDyMinusDx += 2*dy;
                                                        x += 1;
                                                }
                                        }
                                        else {
                                                double dx = xend - x0;
                                                double dy = yend - y0;
                                                double yi = 1;
                                                if (dy < 0) {
                                                        yi = -1;
                                                        dy = -1*dy;
                                                }
                                                double twoDyMinusDx = 2*dy - dx;
                                                double y = y0;
                                                double x = x0;
                                                while (x <= xend) {
                                                        makePix(roundOff(x),roundOff(y), pid);
                                                        if (twoDyMinusDx > 0) {
                                                                y += yi;
                                                                twoDyMinusDx -= 2*dx;
                                                        }
                                                        twoDyMinusDx += 2*dy;
                                                        x += 1;
                                                }
                                        }
                                }
                                else {
                                        if (y0 > yend) {
                                                double dx = x0 - xend;
                                                double dy = y0 - yend;
                                                double xi = 1;
                                                if (dx < 0) {
                                                        xi = -1;
                                                        dx = -1*dx;
                                                }
                                                double twoDxMinusDy = 2*dx - dy;
                                                double x = xend;
                                                double y = yend;
                                                while (y < y0) {
                                                        makePix(roundOff(x), roundOff(y), pid);
                                                        if (twoDxMinusDy > 0) {
                                                                x += xi;
                                                                twoDxMinusDy -= 2*dy;
                                                        }
                                                        twoDxMinusDy += 2*dx;
                                                        y += 1;
                                                }
                                        }
                                        else {
                                                double dx = xend - x0;
                                                double dy = yend - y0;
                                                double xi = 1;
                                                if (dx < 0) {
                                                        xi = -1;
                                                        dx = -1*dx;
                                                }
                                                double twoDxMinusDy = 2*dx - dy;
                                                double x = x0;
                                                double y = y0;
                                                while (y < yend) {
                                                        makePix(roundOff(x), roundOff(y), pid);
                                                        if (twoDxMinusDy > 0) {
                                                                x += xi;
                                                                twoDxMinusDy -= 2*dy;
                                                        }
                                                        twoDxMinusDy += 2*dx;
                                                        y += 1;
                                                }
                                        }
                                }
                        }//END BRESENHAM
                }
                //BEGIN RASTERIZATION
                for (int i = 0; i < wLength; i++) {
                //Each i is a particular scanline...
                        bool beenOnBlue = false;
                        bool insideNow = false;
                        bool beenInside = false;
                        for (int j = 0; j < wWidth; j++) {
                                //j walks across a scan line                                                 
                                int r = PolygonBuffer[pid*wWidth*wLength*3 + (i*wWidth+j)*3+0];
                                int g = PolygonBuffer[pid*wWidth*wLength*3 + (i*wWidth+j)*3+1];
                                int b = PolygonBuffer[pid*wWidth*wLength*3 + (i*wWidth+j)*3+2];

                                if (r == 0 && g == 255 && b == 255 && !beenOnBlue && fabs((float)i-maxExtrema) > 1.0 && fabs((float)i-minExtrema) > 1.0) {
                                        //Toggle the flag and wait to see what happens next
                                        beenOnBlue = true;
                                }
                                else if (r == 0 && g == 0 && b == 0 && beenOnBlue && !beenInside) {
                                        //Make it red!
                                        PolygonBuffer[pid*wWidth*wLength*3 + (i*wWidth+j)*3+0] = 255;
                                        PolygonBuffer[pid*wWidth*wLength*3 + (i*wWidth+j)*3+1] = 0;
                                        PolygonBuffer[pid*wWidth*wLength*3 + (i*wWidth+j)*3+2] = 0;

                                        if (!insideNow) 
                                                insideNow = true;
                                }
                                else if (r == 0 && g == 255 && b == 255 && insideNow) {
					//Stop making it red!
                                        beenInside = true;
                                        insideNow = false;
                                }
                        }
                }//END RASTERIZATION
                copyBuffer(pid);
                polyCount++;
        }
}
//main display loop, this function will be called again and again by OpenGL
void display()
{
        //Displays window to screen        
        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();

        //draws pixel on screen, width and height must match pixel buffer dimension
        glDrawPixels(wWidth, wLength, GL_RGB, GL_FLOAT, PixelBuffer);

        //window refresh
        glFlush();
}
void makePix(int x, int y, int pid)
{
        PolygonBuffer[pid*wWidth*wLength*3 + (y*wWidth+x)*3+0] = 0;
        PolygonBuffer[pid*wWidth*wLength*3 + (y*wWidth+x)*3+1] = 255;
        PolygonBuffer[pid*wWidth*wLength*3 + (y*wWidth+x)*3+2] = 255;
}
void copyBuffer(int pid)
{
        for (int i = 0; i < wLength*wWidth*3; i++) {
                if (PolygonBuffer[pid*wWidth*wLength*3 + i] != 0)
                        PixelBuffer[i] = PolygonBuffer[pid*wWidth*wLength*3 + i];
        }
}
void clearPixelBuffer() 
{
	for (int i = 0; i < wLength*wWidth*3; i++)
		PixelBuffer[i] = 0; 
}
void clearPolygonBuffer(int pid)
{
	for (int i = 0; i < wLength*wWidth*3; i++) 
		PolygonBuffer[pid*wWidth*wLength*3 + i] = 0; 
}
void scaleMenu(int pid)
{
	double scale; 
	int vertices = vArr.at(pid).at(0);
        double xSum = 0;
	double ySum = 0; 	
	double centX, centY;

	cout << "Please enter the magnitude you'd like to scale by:\n";	
	cin >> scale; 

	for (int i = 0; i < vertices; i++) {
		xSum += vArr.at(pid).at(1+i*2);
	}
	centX = xSum / (double)vertices; 
	for (int i = 0; i < vertices; i++) {
		ySum += vArr.at(pid).at(2+i*2); 
	}
	centY = ySum / (double)vertices;
	for (int i = 0; i < vertices; i++) {
		vArr.at(pid).at(1+i*2) = scale*(vArr.at(pid).at(1+i*2) - centX) + centX; 
		vArr.at(pid).at(2+i*2) = scale*(vArr.at(pid).at(2+i*2) - centY) + centY; 		
	}

	ofstream file;
        file.open("inputFile.txt", std::ofstream::out | std::ofstream::trunc);
        if (!file) {
                cerr << "Unable to open file\n";
                exit(1);   
        }
        file << vArr.size() << '\n';
        for (int i = 0; i < vArr.size(); i++) {
                for (int j = 0; j < vArr.at(i).size(); j++)
                        file << vArr.at(i).at(j) << '\n';
        }

	lineDrawRaster();
        glutPostRedisplay();
}
void rotateMenu(int pid)
{
        double alpha;
        int vertices = vArr.at(pid).at(0);
        double xSum = 0;
        double ySum = 0;
        double centX, centY; 
	
        cout << "Please enter the angle in radians you'd like to rotate by:\n";
        cin >> alpha;

        for (int i = 0; i < vertices; i++) {
                xSum += vArr.at(pid).at(1+i*2);
        }
        centX = xSum / (double)vertices;
        for (int i = 0; i < vertices; i++) {
                ySum += vArr.at(pid).at(2+i*2);
        }
        centY = ySum / (double)vertices;
        for (int i = 0; i < vertices; i++) {
		double oldX, oldY; 
                oldX = vArr.at(pid).at(1+i*2) - centX;
                oldY = vArr.at(pid).at(2+i*2) - centY;
                vArr.at(pid).at(1+i*2) = (oldX*cos(alpha) - oldY*sin(alpha)) + centX;
                vArr.at(pid).at(2+i*2) = (oldX*sin(alpha) + oldY*cos(alpha)) + centY;
        }

	ofstream file;
        file.open("inputFile.txt", std::ofstream::out | std::ofstream::trunc);
        if (!file) {
                cerr << "Unable to open file\n";
                exit(1);   
        }
        file << vArr.size() << '\n';
        for (int i = 0; i < vArr.size(); i++) {
                for (int j = 0; j < vArr.at(i).size(); j++)
                        file << vArr.at(i).at(j) << '\n';
        }

        lineDrawRaster();
        glutPostRedisplay();
}
void translateMenu(int pid)
{
        int x, y; 
	int vertices = vArr.at(pid).at(0);
	cout << "Please enter the x and y translation values:\n";
	cin >> x >> y;
	for (int i = 0; i < vertices; i++) {
		vArr.at(pid).at(1+2*i) += x; 
		vArr.at(pid).at(2+2*i) += y;
	}
	ofstream file; 
	file.open("inputFile.txt", std::ofstream::out | std::ofstream::trunc);
        if (!file) {
                cerr << "Unable to open file\n";
                exit(1);   // call system to stop
        }
	file << vArr.size() << '\n';
	for (int i = 0; i < vArr.size(); i++) {
		for (int j = 0; j < vArr.at(i).size(); j++) 
			file << vArr.at(i).at(j) << '\n';
	}

	lineDrawRaster(); 
	glutPostRedisplay(); 
}
void clippingMenu(int pid) 
{
	int xMin, xMax, yMin, yMax; 
	int vertices = (int)vArr.at(pid).at(0); 
	cout << "Please enter the x lower, x upper, y lower, and y upper bounds for the clipping window and hit enter:\n"; 
	cin >> xMin >> xMax >> yMin >> yMax;
	while (xMin < 0 || xMax > wWidth || yMin < 0 || yMax > wLength || xMin >= xMax || yMin >= yMax) {
		cout << "Out of bounds. Please enter the x lower, x upper, y lower, and y upper bounds for the clipping window and hit enter:\n";
		cin >> xMin >> xMax >> yMin >> yMax; 
	}

	int points[10][2]; 		
	for (int i = 0; i < vertices; i++) {
		points[i][0] = (int)vArr.at(pid).at(1+i*2);
	        points[i][1] = (int)vArr.at(pid).at(2+i*2);	
	}		
	
	//Points in clipping window as well as in input file must be in clockwise order
	int clipWindow[][2] = {{xMin, yMin}, {xMin, yMax}, {xMax, yMax}, {xMax, yMin}}; 	

	for (int i = 0; i < 4; i++) {
		int k = (i+1) % 4; 
		clip(points, vertices, clipWindow[i][0], clipWindow[i][1], clipWindow[k][0], clipWindow[k][1]);
	}	 
	
	vArr.at(pid).clear();
      	
	vArr.at(pid).push_back((double)vertices); 
	for (int i = 0; i < vertices; i++) { 
                vArr.at(pid).push_back((double)points[i][0]); 
                vArr.at(pid).push_back((double)points[i][1]); 
	}

	ofstream file;
        file.open("inputFile.txt", std::ofstream::out | std::ofstream::trunc);
        if (!file) {
                cerr << "Unable to open file\n";
                exit(1);   
        }
        file << vArr.size() << '\n';
        for (int i = 0; i < vArr.size(); i++) {
                for (int j = 0; j < vArr.at(i).size(); j++)
                        file << vArr.at(i).at(j) << '\n';
        }

        lineDrawRaster();
        glutPostRedisplay();
}
void clip(int points[][2], int &vertices, int x1, int y1, int x2, int y2)
{
	//Sutherland-Hodgman
	int newPoints[10][2];
        int newVertices = 0; 
    	for (int i = 0; i < vertices; i++) { 
        	  
       		int k = (i+1) % vertices; 
        	int ix = points[i][0], iy = points[i][1]; 
        	int kx = points[k][0], ky = points[k][1]; 
  
        	// Find position of first point of clipper line 
        	int i_pos = (x2-x1) * (iy-y1) - (y2-y1) * (ix-x1); 
  
        	// Find position of second point 
        	int k_pos = (x2-x1) * (ky-y1) - (y2-y1) * (kx-x1); 
  
        	// Both points inside 
        	if (i_pos < 0  && k_pos < 0) {	  
            		newPoints[newVertices][0] = kx; 
            		newPoints[newVertices][1] = ky; 
            		newVertices++; 
        	}	 
  
        	// Only first point is outside 
        	else if (i_pos >= 0  && k_pos < 0) {  
            		newPoints[newVertices][0] = xInter(x1, y1, x2, y2, ix, iy, kx, ky); 
            		newPoints[newVertices][1] = yInter(x1, y1, x2, y2, ix, iy, kx, ky); 
            		newVertices++; 
  
            		newPoints[newVertices][0] = kx; 
            		newPoints[newVertices][1] = ky; 
            		newVertices++; 
        	}	 
  
        	// Only second point is outside 
        	else if (i_pos < 0  && k_pos >= 0) {  
            		newPoints[newVertices][0] = xInter(x1, y1, x2, y2, ix, iy, kx, ky); 
            		newPoints[newVertices][1] = yInter(x1, y1, x2, y2, ix, iy, kx, ky); 
            		newVertices++; 
        	} 
 
        	// Both outside 
        	else  
        		; //Do Nothing 
    	} 
    	vertices = newVertices; 
    	for (int i = 0; i < vertices; i++) {	 
        	points[i][0] = newPoints[i][0]; 
        	points[i][1] = newPoints[i][1]; 
    	}	 
}
int xInter(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
	return ((x1*y2 - y1*x2) * (x3-x4) - (x1-x2) * (x3*y4 - y3*x4)) / ((x1-x2) * (y3-y4) - (y1-y2) * (x3-x4));  
}
int yInter(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
	return ((x1*y2 - y1*x2) * (y3-y4) - (y1-y2) * (x3*y4 - y3*x4)) / ((x1-x2) * (y3-y4) - (y1-y2) * (x3-x4));  
}
