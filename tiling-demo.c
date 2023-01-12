/*
 * Copyright (c) 2023 Andrei Datcu
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>

#define WIDTH 854
#define HEIGHT 480

Display *dpy;
Window *win;
XEvent ev;

void
open_dpy()
{
	dpy = XOpenDisplay(NULL);
	(dpy != NULL) ? fprintf(stdout, "Connection to X Server succesful...\n") : fprintf(stderr, "Connection to X Server failed!\nTerminating...");
}

void
terminator()
{
	fprintf(stdout, "Terminating...");
	XCloseDisplay(dpy);
}

Window
generate_win()
{
	/*
	 * seed for choosing colors
	 */
	srand(time(NULL));

	/*
	 * credits to gruvbox for the color scheme
	 */
	long colors[] = {0xa89984, 0xbdae93, 0xd5c4a1, 0xebdbb2, 0xfbf1c7};
	
	Window gen = XCreateSimpleWindow(dpy,
					win,
					0,
					0,
					WIDTH,
					HEIGHT,
					2,
					0x928374,
					colors[rand()%5]);
	return gen;
}

void
main(void)
{
	open_dpy();

	/*
	 * main window
	 */
	win = XCreateSimpleWindow(dpy,
				XDefaultRootWindow(dpy),
				0,
				0,
				WIDTH,
				HEIGHT,
				1,
				0x282828,
				0x282828);

	XSelectInput(dpy, win, KeyPressMask|ButtonPressMask);
	XMapWindow(dpy, win);
	
	char run = 1;
	char kp[255];
	int active_wins = 0;
	int i = 0;
	Window windows[4];
	
	/*
	 * these variables were created so that the code
	 * would be easier to read
	 */
	int w = WIDTH;
	int h = HEIGHT;

	while(run)
	{
		XNextEvent(dpy, &ev);
		KeySym key;

		if(ev.type == KeyPress && XLookupString(&ev.xkey, kp, 255, &key, 0) == 1)
		{
			switch(kp[0])
			{
				case ' ':
					/*
					 * we do not want to have more than 4 windows
					 * on screen because it doesn't look pretty.
					 */
					if(active_wins > 3)
					{
						terminator();
						exit(0);	
						break;
					}

					active_wins++;
					windows[i] = generate_win(w, h);
					XMapWindow(dpy, windows[i]);
					i++;

					/*
					 * this rough code was deducted by me.
					 * the first window uses half the screen,
					 * the second half of the remainder and so on.
					 *
					 * the formula used for the 4th window doesn't
					 * make any sense, but it just works(TM).
					 */ 
					if(active_wins % 3 == 0)
					{
						XResizeWindow(dpy, windows[i-1], (active_wins == 3) ? w/2 : w, (active_wins == 3) ? h/2 : h);
						XResizeWindow(dpy, windows[i-2], (active_wins == 3) ? w/2 : w, (active_wins == 3) ? h/2 : h);
						XMoveWindow(dpy, windows[i-1], w/2, h/2);

					}
					if(active_wins % 2 == 0)
					{
						XResizeWindow(dpy, windows[i-1], (active_wins == 4) ? w/4 : w/2, (active_wins == 4) ? h/2 : h);
						XResizeWindow(dpy, windows[i-2], (active_wins == 4) ? w/4 : w/2, (active_wins == 4) ? h/2 : h);
						XMoveWindow(dpy, windows[i-1], (active_wins == 4) ? ((w+h)/2 - h/18) : w/2, (active_wins == 4) ? h/2 : 0);
					}
					break;	
			}
		}
	}	
}
