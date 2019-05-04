/*

 E vs. Pi  version 1.0
 [compile this with LARGE MEMORY MODEL, otherwise it will crash]

 silencer/darkdesigns

 silencer@minea.com.ar
 spring 2003, bs.as. argentina
 www.darkdesigns.com.ar


*/

#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <time.h>
#include <malloc.h>
#include "int9.h"
#include "vector.h"

typedef unsigned int word;
typedef unsigned char byte;

byte  *sprites;

float g=0.2;
float piso=86;
int camerahit=0;

float max_power=32;

	unsigned int backbufferdir;
	unsigned int backgrounddir;
	unsigned int spritesdir;

int sgn(float x)
{
	if(x<0)return -1;
	if(x>0)return 1;
	if(x==0)return 0;
}

int not(int x);

struct s_sprite
{
 double x,y;
 double force_y;
 double bforce_y;
 float frame;
 int animation;
 float status;  // 0 caminando; 1 saltando; 2 golpeado; 3 cubriendose
 int der;
 int washit;
 float hit_x;
 float hit_y;
 float timer;
 float dx,ldx;
 float power;
 int golpe;
 int pKEY_LEFT;
 int pKEY_RIGHT;
 int pKEY_UP;
 int pKEY_PUNCH;
 int pKEY_BLOCK;
};

struct particle
{
	float x,y;
	double force_y;
	double force_x;
	int floor_y;
	int life;
};

void particle_animate(struct particle *,int n);
void particle_initialize(struct particle *p,int np, float x, float y,int sgn);
void particle_draw(struct particle *p,int np,unsigned int where, byte *from);

void createbufferfromfile(byte *buffer,char *file, int w,int h);
struct particle particles[10]; // the black ink blood :)

unsigned char *vga = (unsigned char *) MK_FP(0xA000, 4);
void draw_sprite(int frame, int sprite,int der,int dx,int dy,int w,int h,byte *sprites, unsigned int where);
void putpixel (word X, word Y, byte Col, word Where);
void draw_picture(int sx, int sy,int w,int h,int dx,int dy,byte *sprites,int buff_w, unsigned int where);

// allow palette color reseting
void setpal(unsigned char col, unsigned char r, unsigned char g, unsigned char b)
{
	outp(0x03C8, col);
	outp(0x03C9, r);
	outp(0x03C9, g);
	outp(0x03C9, b);
}

// main sub
void main(void)
{
	int fps,it;	 	// just for frames counting
	FILE *in;				// generic file pointer
	time_t otime;
	int sinus[720], cosus[720];
	struct s_sprite player[2];
	int timer=0;
	byte  *backbuffer;
	byte *pictures;
	byte  *background;

	int i,n,c;
	char key;
	int pi_frame=0;
	int sgnx=-1;
	for(n=0;n<2;n++)
	{
		player[n].frame=0;
		player[n].der=1;
		player[n].washit=0;
		player[n].status=0;
		player[n].power=32;
		player[n].timer=0;
		player[n].dx=0;
		player[n].golpe=0;
	}
	player[0].x=100;player[0].y=piso;
	player[1].x=200;player[1].y=piso;


 player[0].pKEY_LEFT=KEY_LEFT;
 player[0].pKEY_RIGHT=KEY_RIGHT;
 player[0].pKEY_UP=KEY_UP;
 player[0].pKEY_PUNCH=KEY_INS;
 player[0].pKEY_BLOCK=KEY_DEL;

 player[1].pKEY_LEFT=KEY_F;
 player[1].pKEY_RIGHT=KEY_H;
 player[1].pKEY_UP=KEY_T;
 player[1].pKEY_PUNCH=KEY_Q;
 player[1].pKEY_BLOCK=KEY_A;


	backbuffer=(byte *) malloc(64000);
	background=(byte *) malloc(64000);
	sprites=(byte *) malloc(64000);
	pictures=(byte *) malloc(64000);

	for(n=0;n<10;n++) particles[n].life=0;

	_setcursortype(_NOCURSOR);

	clrscr();
	textcolor(15);
	textbackground(1);
	cprintf("            e vs. pi: 'the ultimate calculus' version 1.0 loader                \n");
	printf("\n");

	if (backbuffer==NULL || background==NULL || sprites==NULL || pictures==NULL)
	{
		printf("[ERROR] not enough memory to create buffers");
		getch();
		exit(1);
	}
	printf("<> 250 Kbytes allocated\n");

		// clear the backbuffer (actually it's interesting to see it all dirty)
	_fmemset(backbuffer, 2, 64000);

	// get the backbuffer address
	backbufferdir=FP_SEG(backbuffer);
	backgrounddir=FP_SEG(background);
	spritesdir=FP_SEG(sprites);

	createbufferfromfile(background,"arena1.raw",320,200);
	createbufferfromfile(sprites,"chars.raw",320,200);
	createbufferfromfile(pictures,"pics.raw",320,200);

	// fake sinus & cosinus tables to improve speed
	for(n=0;n<720;n++)
	{
		sinus[n]=(int)(32*sin(2*n*3.14/180));
		cosus[n]=(int)(32*cos(2*n*3.14/180));
	}

	printf("<> fake sinus & cosinus table created\n");

	// initialize frame counting
	otime = time(NULL); it=0;

	printf("<> frame counting initialized\n");

	gotoxy(23,20);cprintf(" - press any key to continue -");
	getch();


	asm{
		mov ax,0x13
		int 0x10
	}

	// set the palette for a nice green gradient
	for(c=0;c<255;c++)
	{
		int v=c;
		v=(int)(64*v/255);
		setpal(c,v,v,v);
	}

	 install_kb();

	 player[0].animation=1;
	 player[1].animation=1;

	 // main loop
	for(;;)
	{
		// this is for frame counting
		it++;
		if(time(NULL)!=otime)
		{
			otime=time(NULL);
			fps=it;
			it=0;
		}
		//

		// background, sprites, foreground stuff here
	 _fmemset(backbuffer, 0, 64000); // clear the back buffer

		// copy the background to the back buffer
		_fmemcpy (backbuffer, background, 64000);

		// --- idle
		timer++;

		if(timer>300)
		{
			timer=0;

			//// the fighters must be looking to each other all the time
			if(player[0].x>player[1].x)
			{
				if(player[0].status==0)player[0].der=0;
				if(player[1].status==0)player[1].der=1;
			}
			if(player[0].x<player[1].x){
				if(player[0].status==0)player[0].der=1;
				if(player[1].status==0)player[1].der=0;
			}
			// if there's any particle alive, animate it
			particle_animate(particles,10);

			for(n=0;n<2;n++)
			{
				/// status 0, caminando
				if(player[n].status==0 || player[n].status==3)
				{

					if(player[n].y>piso) player[n].y=piso;

					if(player[n].status==0) player[n].animation=1;
					if(player[n].status==3) player[n].animation=6;
					player[n].frame+=.2;
					if(player[n].frame>5) player[n].frame=1;
					player[n].x+=player[n].dx;
					player[n].dx=0;
				}
				/// status 1, saltando
				if(player[n].status==1)
				{
					player[1].hit_x=6+player[1].x+(player[1].der*8);
					player[1].hit_y=player[1].y+18;
					player[0].hit_x=0+player[0].x+(player[0].der*20);
					player[0].hit_y=player[0].y+10;

					player[n].x+=player[n].ldx;
					player[n].dx=0;
					player[n].frame+=.2;
					if(player[n].frame>5)
					{
						player[n].frame=1;
					}
				}
				/// status 2, golpeado
				if(player[n].status==2)
				{
					player[n].animation=3;
					if(player[n].timer==0) player[n].frame=1;
					player[n].timer+=.2;
					if(player[n].timer<10)
					{
						player[n].frame+=.5;
						if(player[n].der==0) player[n].x+=.5;
						if(player[n].der==1) player[n].x-=.5;
					}
					if(player[n].frame>5) player[n].frame=5;
					if(player[n].timer>=10)
					{
						player[n].frame-=.5;
						if(player[n].frame<1)
						{
							player[n].frame=1;
							player[n].status=0;
							player[n].timer=0;
						}
					}
				}

				/// status 4, golpeando
				if(player[n].status==4) //golpe
				{
					player[1].hit_x=4+player[1].x+(player[1].der*10);
					player[1].hit_y=player[1].y+10;
					player[0].hit_x=6+player[0].x+(player[0].der*8);
					player[0].hit_y=player[0].y+10;


					player[n].animation=5;
					if(player[n].timer<0) player[n].frame=1;
					player[n].timer+=2;
					if(player[n].timer<10)
					{
						player[n].frame+=1;
						if(player[not(n)].status!=2 && player[not(n)].status!=3) // no golpeado
						if(player[n].hit_y>player[not(n)].y+2.0f &&
							 player[n].hit_y<player[not(n)].y+18.0f &&
							 player[n].hit_x<player[not(n)].x+17.0f &&
							 player[n].hit_x>player[not(n)].x+2.0f)
							 {
								 sgnx=-1;
								 if(player[n].der==1) sgnx=1;
								 particle_initialize(particles,3,player[n].hit_x-13,player[n].hit_y, sgnx);
								 if(player[not(n)].washit==0)
								 {
									player[not(n)].x+=sgnx*3;
									player[not(n)].washit=1;
									player[not(n)].power-=35/15;
								 }
							}
					}
					if(player[n].frame>5) player[n].frame=5;
					if(player[n].timer>=10)
					{
						player[n].frame-=.5;
						if(player[n].frame<1)
						{
							player[n].frame=1;
							player[n].status=0;
							player[not(n)].washit=0;
							player[n].timer=0;
						}
					}
				}

				if(player[n].status==0) player[n].ldx=0;

				if(player[n].status==1 && player[n].golpe==1) //saltando & golpeando
				{
					player[n].x+=player[n].dx;
					if(player[not(n)].status!=2 && player[not(n)].status!=3) // no golpeado
					if(player[n].hit_y>player[not(n)].y+2.0f &&
						 player[n].hit_y<player[not(n)].y+18.0f &&
						 player[n].hit_x<player[not(n)].x+17.0f &&
						 player[n].hit_x>player[not(n)].x+2.0f)
						 {
							 sgnx=-1;
							 if(player[n].der==1) sgnx=1;
							 particle_initialize(particles,10,player[n].hit_x-13,player[n].hit_y,  sgnx );
							 player[not(n)].status=2; //golpeado
							 player[not(n)].bforce_y=1;
							 player[not(n)].power-=35/10;
							 player[not(n)].force_y=1;
						}
				}

			// our ultra_sofisticated gravity system  [1]
				if(player[n].status==1) //saltando
				{
					player[n].force_y-=.1;
					player[n].y-=player[n].force_y;
					if(player[n].y>piso)
					{
						if(player[n].golpe==1) player[n].golpe=0;
						player[n].y=piso;
						player[n].status=0;
						player[n].ldx=0;
					}
				}

			// our ultra_sofisticated gravity system [2]
				if(player[n].status==2) //golpeado
				{
					player[n].force_y-=.1;
					player[n].y-=player[n].force_y;
					if(player[n].y>piso && player[n].force_y<0.01)
					{
						player[n].y=piso;
						player[n].bforce_y*=.5;
						player[n].force_y=player[n].bforce_y;
					}
					if(player[n].y>piso && player[n].bforce_y<0.01)
					{
						player[n].y=piso;
						player[n].status=0;
						player[n].ldx=0;
					}
				}




	}

		draw_sprite(player[0].frame,player[0].animation,player[0].der,player[0].x,player[0].y,22,22,sprites, backbufferdir) ;
		draw_sprite(player[1].frame+5,player[1].animation,player[1].der,player[1].x,player[1].y,22,22,sprites, backbufferdir) ;
		particle_draw(particles,10,backbufferdir, sprites);
		//pi
		draw_picture(1, 1,16,20,20,10,pictures,320, backbufferdir);
		// power
		draw_picture(1, 22,1+68*(player[0].power/max_power),44,23,32,pictures,320, backbufferdir);
		// borde
		draw_picture(1, 108,73,135,20,30,pictures,320, backbufferdir);
		//e
		draw_picture(18, 1,32,20,220,10,pictures,320, backbufferdir);
		// power
		draw_picture(1, 47,62*(player[1].power/max_power),68,223,32,pictures,320, backbufferdir);
		// borde
		draw_picture(1, 108,73,135,220,30,pictures,320, backbufferdir);
		 // flip back and primary buffer
		_fmemcpy (vga, backbuffer, 64000);

			for(n=0;n<2;n++)
			if(player[n].status==3)  //cubierto
					player[n].status=0; // caminando
}

		/// TECLADO, ENTRADA DE
		for(n=0;n<2;n++)
		{

		if(keys[player[n].pKEY_PUNCH])
		{
			if(player[n].status==1) player[n].golpe=1;
			if(player[n].status==1) player[n].animation=2;
			if(player[n].status==0)  //caminando
			{
				player[n].status=4; // pegando
				player[n].timer=-1;
			}

		}
		if(keys[player[n].pKEY_RIGHT] && player[n].status==0)
		{
			player[n].dx=2;
			player[n].ldx=player[n].dx;
		}
		if(keys[player[n].pKEY_LEFT] && player[n].status==0)
		{
			player[n].dx=-2;
			player[n].ldx=player[n].dx;
		}

		if(keys[player[n].pKEY_BLOCK])
		{
			if(player[n].status==0)  //caminando
			{
				player[n].status=3; // cubierto
				player[n].animation=6;
			}
		}

		if(keys[player[n].pKEY_UP])
		{
			if(player[n].status==0)
			{
				player[n].status=1;
				player[n].force_y=2;
				player[n].animation=4;
				player[n].frame=1;
			}
		}

		 }
	/// FIN DE ENTRADA DE TECLADO
			for(n=0;n<2;n++)
			{
			if(!keys[player[n].pKEY_LEFT] && !keys[player[n].pKEY_RIGHT]) player[n].dx=0;
			if(keys[player[n].pKEY_LEFT] && keys[player[n].pKEY_RIGHT] ) player[n].dx=0;
			}

		// --------
		if(keys[KEY_ESC])
		{
			// go back to 80x25x4 (text mode)
			asm{
				mov ax,0x03
				int 0x10
			}

		_setcursortype(_NOCURSOR);
		textcolor(15);
		textbackground(1);
		cprintf("            e vs. pi: 'the ultimate calculus' version 1.0 unloader              \n");

		printf(" <> got back to 80x25\n");
		uninstall_kb();
		printf(" <> keyboard handler deinitialized\n");

		free(backbuffer);
		free(background);
		free(sprites);
		free(pictures);
		printf(" <> %i fps after quitting (REAL)\n",fps);
		printf(" <> 250 Kbytes freed \n");
		printf(" <> terminated \n");

		//end titles
		gotoxy(1,20);
		printf(" coded by silencer/darkdesigns \n");
		printf(" 2003, bs.as. argentina\n");
		printf(" msn&mail: silencer_ar@hotmail.com\n");
		printf(" (this is dedicated to gaby)\n");
		// go back to DOS
		getch();
		exit(1);
		}
	}
}

//little nice-fast-pretty_cool-asm_maniac drawing pixel routine
void putpixel (word X, word Y, byte Col, word Where) {
	asm {
		push    ds
		push    es
		mov     ax, [Where]
		mov     es, ax
		mov     bx, [X]
		mov     dx, [Y]
		push    bx
		mov     bx, dx
		mov     dh, dl
		xor     dl, dl
		shl     bx, 6
		add     dx, bx
		pop     bx
		add     bx, dx
		mov     di, bx
		xor     al, al
		mov     ah, [Col]
		mov     es:[di], ah
		pop     es
		pop     ds
	}
}

void draw_sprite(int frame, int sprite,int der,int dx,int dy,int w,int h,byte *sprites, unsigned int where)
{
 int sx=1+(w*(frame-1));
 int sy=1+(h*(sprite-1));
 int px,py;
 byte color;

 for(px=0;px<w-1;px++)
 for(py=0;py<h-1;py++)
 {
	 if(der==0) color=sprites[(py+sy)*320+((w-2-px)+sx)];
	 if(der==1) color=sprites[(py+sy)*320+((px)+sx)];
	 if(color!=255)
	 putpixel (dx+px, dy+py, color, where);
 }
}

void particle_initialize(struct particle *p,int np, float x, float y, int sgn)
{
	int n;
	for(n=0;n<np;n++)
	{
		p[n].life=1;
		p[n].force_y=2+(rand()%20)/10;
		p[n].force_x=sgn*(1.3+.5*cos(n*(3.14/np)));
		p[n].x=x; p[n].y=y;
		p[n].floor_y=piso+15+rand()%5;
	}
}
void particle_animate(struct particle *p,int np)
{
	int n;
	int fy;
	for(n=0;n<np;n++)
	{
		if(p[n].life==1)
		{
			p[n].force_y-=.4;
			p[n].force_x-=sgn(p[n].force_x)*.03;
			if(fabs(p[n].force_x)<0.01) p[n].force_x=0;
			p[n].y-=p[n].force_y;
			p[n].x+=p[n].force_x;


		if(p[n].y>p[n].floor_y)
			{
				p[n].y=p[n].floor_y;
				p[n].life=0;
				draw_sprite(1,7,0,p[n].x,p[n].y,22,22,sprites, backgrounddir);
			}
		}
	}
}
void particle_draw(struct particle *p,int np,unsigned int where, byte *from)
{
	int n;
	for(n=0;n<np;n++)
	{
			draw_sprite(1,7,0,p[n].x,p[n].y,22,22,from, where);
	}
}

int not(int x)
{
	if(x==0) return 1;
	return 0;
}

	void createbufferfromfile(byte *buffer,char *file, int w,int h)
	{
	FILE *in;
	// load the sprites pictures
	in=fopen(file,"rb");
	if(in==NULL)
	{
		printf("[ERROR] unable to open %s",file);
		getch(); exit(1);
	}
	fread(buffer,w*h,sizeof(unsigned char),in);
	fclose(in);
	printf("<> createbufferfromfile(%s) succed\n",file);
	}

void draw_picture(int sx, int sy,int sx1,int sy1,int dx,int dy,byte *sprites,int buff_w, unsigned int where)
{
 int px,py;
 byte color;
 for(px=sx;px<sx1;px++)
 for(py=sy;py<sy1;py++)
 {
	 color=sprites[(py)*buff_w+((px))];
	 if(color!=255)
	 putpixel (dx+px-sx, dy+py-sy, color, where);
 }
}
