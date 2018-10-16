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

float *PixelBuffer;
float *PolygonBuffer;
int polyCount; 
int wWidth; 
int wLength; 
void display();
inline int roundOff(const double a) {return (int)(a+0.5);}
void makePix(int x, int y);
inline void mainMenu(int id) {;} 
void copyBuffer();
void scaleMenu(int id); 
void rotateMenu(int id); 
void translateMenu(int id);
//void mouseFunc(int button, int state, int x, int y);
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
	PixelBuffer = new float[wWidth * wLength * 3](); 
	int scale_menu, rotate_menu, translate_menu; 

	fstream file; 
	file.open("inputFile.txt");
	vector<double> v1;
	double num; 
	if (!file) {
    		cerr << "Unable to open file\n";
    		exit(1);   // call system to stop
	}
 	while (file >> num) 
    		v1.push_back(num);
	auto vpoint = v1.begin(); 	
	int polyTotal = (int)*vpoint;
	PolygonBuffer = new float[polyTotal*wWidth*wLength*3](); 

        string Alg; 	
	cout << "Please specify DDA with \"d/D\" or Bresenham with \"B/b\"\n";  
	cin >> Alg;
        while (Alg != "d" && Alg != "D" && Alg != "B" && Alg != "b") {
		cout << "Specify DDA with \"d/D\" or Bresenham with \"B/b\"\n";
		cin >> Alg; 
	}		
	//create and set main window title
	int MainWindow = glutCreateWindow("Polygons");
	glClearColor(0, 0, 0, 0); //clears the buffer of OpenGL, sets a black background
		
	/*In the vector, the input file data appears as:
	[3,4,0,0,100,0,100,100,0,100,3,300,0,300,100,200,0,5,100,200,300,200,300,300,200,400,100,300,100,200]*/
        polyCount = 0; 	
	while (polyCount < polyTotal) // Number of polygons to process 
	{
		int lineCount = 0;
	        int lineTotal; 	
		double maxExtrema = 0; 
		double minExtrema = wLength;
		if (polyCount == 0)
	        	lineTotal = *(vpoint+1);
		else {
			lineTotal = *(vpoint+2); 
			vpoint++; 
		}	
		while (lineCount < lineTotal) {
			vpoint += 2; 	
			double x0 = *(vpoint);
      			double y0 = *(vpoint+1);
		        if (y0 > maxExtrema) 
				maxExtrema = y0; 
			if (y0 < minExtrema) 
				minExtrema = y0; 	
			double xend, yend; 
		        if (lineCount == lineTotal-1) {
				xend = *(vpoint-lineCount*2+0);
				yend = *(vpoint-lineCount*2+1);
				if (yend > maxExtrema) 
					maxExtrema = yend;
			        if (yend < minExtrema) 
					minExtrema = yend; 	
			}	
			else {
				xend = *(vpoint+2);
				yend = *(vpoint+3); 
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
				makePix(roundOff(x), roundOff(y)); 
				for (k = 0; k < steps; k++) {
					x += xinc; 
					y += yinc;
					makePix(roundOff(x), roundOff(y)); 
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
                                                       	makePix(roundOff(x),roundOff(y));
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
                                                       	makePix(roundOff(x),roundOff(y));
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
                                                       	makePix(roundOff(x), roundOff(y));
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
                                                       	makePix(roundOff(x), roundOff(y));
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
			lineCount++; 
		}
		//BEGIN RASTERIZATION
		for (int i = 0; i < wLength; i++) {
			//Each i is a particular scanline...
			bool beenOnBlue = false; 
			bool insideNow = false; 
			bool beenInside = false; 
			for (int j = 0; j < wWidth; j++) {
				//j walks across a scan line 
				int r = PolygonBuffer[polyCount*wWidth*wLength*3 + (i*wWidth+j)*3+0]; 
				int g = PolygonBuffer[polyCount*wWidth*wLength*3 + (i*wWidth+j)*3+1];
				int b = PolygonBuffer[polyCount*wWidth*wLength*3 + (i*wWidth+j)*3+2];

				if (r == 0 && g == 255 && b == 255 && !beenOnBlue && fabs((float)i-maxExtrema) > 1.0 && fabs((float)i-minExtrema) > 1.0) {
					//Toggle the flag and wait to see what happens next
					beenOnBlue = true;	
				}
				else if (r == 0 && g == 0 && b == 0 && beenOnBlue && !beenInside) {
					//Make it red!
					PolygonBuffer[polyCount*wWidth*wLength*3 + (i*wWidth+j)*3+0] = 255;
        	                        PolygonBuffer[polyCount*wWidth*wLength*3 + (i*wWidth+j)*3+1] = 0;
	                                PolygonBuffer[polyCount*wWidth*wLength*3 + (i*wWidth+j)*3+2] = 0;
 
					if (!insideNow)
						insideNow = true; 
				}	
				else if (r == 0 && g == 255 && b == 255 && insideNow) {
					beenInside = true;
					insideNow = false; 	
				} 
			}
		}//END RASTERIZATION
		copyBuffer();
		polyCount++;
	//	for (int z = 0; z < wLength*wWidth*3; z++)
	//	       PolygonBuffer[z] = 0;	

	}
	//Draw the file specified polygons 
	glutDisplayFunc(display);
	//glutMouseFunc(mouseFunc); 


	// Offer the user opportunities to transform! 
	
	translate_menu = glutCreateMenu(translateMenu);
	for (int i = 1; i <= polyTotal; i++) {
		const char base[] = "";
		char str[5];
		sprintf(str, "%s%d", base, i);
		glutAddMenuEntry(str, i);
	}

       	scale_menu = glutCreateMenu(scaleMenu);
	for (int i = 1; i <= polyTotal; i++) {
        	const char base[] = "";
                char str[5];
                sprintf(str, "%s%d", base, i);
                glutAddMenuEntry(str, i);
        }

	rotate_menu = glutCreateMenu(rotateMenu);
	for (int i = 1; i <= polyTotal; i++) {
                const char base[] = "";
                char str[5];
                sprintf(str, "%s%d", base, i);
                glutAddMenuEntry(str, i);
        }

	glutCreateMenu(mainMenu);
		glutAddSubMenu("Translate", translate_menu);
		glutAddSubMenu("Scale", scale_menu); 
		glutAddSubMenu("Rotate", rotate_menu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
        	

	glutPostRedisplay(); 



	glutMainLoop();//main display loop, will display until terminate
	file.close();

	return 0;
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
void makePix(int x, int y)
{
	PolygonBuffer[polyCount*wWidth*wLength*3 + (y*wWidth+x)*3+0] = 0;
	PolygonBuffer[polyCount*wWidth*wLength*3 + (y*wWidth+x)*3+1] = 255;
	PolygonBuffer[polyCount*wWidth*wLength*3 + (y*wWidth+x)*3+2] = 255;
}
void copyBuffer()
{
	for (int i = 0; i < wLength*wWidth*3; i++) {
		if (PolygonBuffer[polyCount*wWidth*wLength*3 + i] != 0)
			PixelBuffer[i] = PolygonBuffer[polyCount*wWidth*wLength*3 + i];
	}
}
void scaleMenu(int id)
{
	cout << "You're trying to scale polygon " << id << endl;
}
void rotateMenu(int id)
{	
	cout << "You're trying to rotate polygon " << id << endl; 
}
void translateMenu(int id)
{
	int x, y; 
	cout << "You'd like to translate polygon " << id << endl;
        cout << "Please enter the translation value along the x axis and y axis:\n";
	cin >> x;
	cin >> y;
       	cout << "You entered " + std::to_string(x) + " for x and " + std::to_string(y) + " for y.\n"; 
}


