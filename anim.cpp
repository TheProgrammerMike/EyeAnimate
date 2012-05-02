// -*- mode: C++; indent-tabs-mode: t; tab-width: 4 -*-
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_Box.H>
#include <cstdlib>
#include <cstring>
#include <cstdio>
using namespace std;

// animate.cxx -- Animate playback of three jpg images (0001.jpg, 0002.jpg..)
//                Hold each image for RATE seconds
//

#define RATE    0.1             // how long to hold each image
#define TOTAL   3               // total images: /var/tmp/000{1,2,3}.jpg
#define CYCLE   20
#define ACCEL   0.25

Fl_Window     *win = 0;         // main window
Fl_JPEG_Image *jpgs[TOTAL];     // loaded images
Fl_Group      *grp = 0;         // group in which images are displayed
int global_x, global_y;

class SpyWindow : public Fl_Double_Window
{
public:
	SpyWindow();
	int handle(int event);
};

SpyWindow::SpyWindow()
: Fl_Double_Window(720,486)
{
}

int SpyWindow::handle(int event)
{
	switch(event)
	{
	case FL_ENTER:
		printf("Enter!\n");
		return 1;
	case FL_MOVE:
		printf("At %d,%d\n", Fl::event_x(), Fl::event_y());
		global_x = Fl::event_x();
		global_y = Fl::event_y();
		//grp->position(Fl::event_x(), Fl::event_y());
		//win->redraw();
		return 1;
	}
	return 0;
}

// SHOW NEXT IMAGE
//     Slaps next image up on screen, resets frame timer.
//
void ShowNextImage_CB(void*) {
    static int x = 0;
	if(x % CYCLE < TOTAL) {
		grp->image(jpgs[x % CYCLE]);
	}
	x++;
	// move toward pointer
	int dx = ACCEL * (global_x - grp->x());
	int dy = ACCEL * (global_y - grp->y());
	grp->position(grp->x() + dx, grp->y() + dy);
    win->redraw();
    Fl::repeat_timeout(RATE, ShowNextImage_CB);	// steady rate
}

// LOAD ALL IMAGES INTO MEMORY
int LoadImages() {
    for ( int t=0; t<TOTAL; t++ ) {
        char filename[80];
        sprintf(filename, "%04d.jpg", t+1);     // 0001.jpg, 0002.jpg..
        jpgs[t] = new Fl_JPEG_Image(filename);
        if ( jpgs[t]->w() == 0 ) { perror(filename); return(1); }
    }
    return(0);
}

// MAIN -- OPEN DOUBLE BUFFERED WINDOW, LOAD IMAGES, START PLAYBACK
int main() {
    fl_register_images();                               // initialize image lib
    win = new SpyWindow();                // make a window
	win->color(FL_WHITE);
    grp = new Fl_Group(0,0,50,50);
    grp->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    win->show();
    if ( LoadImages() ) return(1);
    Fl::add_timeout(RATE, ShowNextImage_CB);
    return(Fl::run());
	
}

