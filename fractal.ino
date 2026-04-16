// mandelbrot and julia fractals  

#include <SPI.h>
#include <TFT_eSPI.h> 

#include <XPT2046_Touchscreen.h>
// Touchscreen pins
#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

TFT_eSPI tft = TFT_eSPI();

int width,height,res;
float cx,cy,zoom;

void setup(void) {
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(3);

  Serial.begin(9600);

  width = tft.width();
  height = tft.height();

  res = 1;
  //mandelbrot home
  cx  = -0.6; cy = 0;
  zoom = 0.01;
}

void mandel(float cx, float cy, float scale, int i) {
 int iter, maxIter = 256;

  int w2 = width / 2;
  int h2 = height / 2;

  float x, y, zx, zy, zx2, zy2;
	//for (int i = 0; i < height; i += res) {
    tft.fillRect(0,i,width,res,0);
		for (int j = 0; j  < width; j += res) {
			x = (j - w2) * scale + cx;
 		  y = (i - h2) * scale + cy;
			
      zx = hypot(x - 0.25, y);
			if (x < zx - 2 * zx * zx + 0.25) continue;
			if ((x + 1)*(x + 1) + y * y < 1/16) continue;

			zx = zy = zx2 = zy2 = 0;
			for (iter = 0; iter <= maxIter && zx2 + zy2 < 4; iter++) {
				zy = 2 * zx * zy + y;
				zx = zx2 - zy2 + x;
				zx2 = zx * zx;
				zy2 = zy * zy;
			}
			int colour = iter  * 500;
      if (iter < maxIter) tft.fillRect(j,i,res,res,colour);
    } 
  
}

void julia(float cx,float cy,float zoom, int x){
 int maxIter = 255;
 int w2 = width / 2;
 int h2 = height / 2;

    //for (int x=0; x < width; x += res) {
        for (int y = 0; y < height; y += res) {
            float zx,zy;
            zx = 1.5 * (x - w2) / (w2 * zoom);
            zy = (y - h2) / (h2 * zoom);
            int i = 0; 
            while ( ( zx*zx + zy*zy) < 4  && i++ < maxIter ) {
                float zxnew = zx*zx - zy*zy + cx;
                zy = 2 * zx * zy + cy;
                zx = zxnew;
            }
            tft.fillRect(x,y,res,res,i);
        }
   // }
}

int i = 0;
void loop() {
  
    if (i < height) mandel(cx,cy,zoom,i++);
    
    int touch = 0;
    if (touchscreen.tirqTouched() && touchscreen.touched()) {
      TS_Point p = touchscreen.getPoint();
      int x = map(p.x, 200, 3700, 1, width);
      int y = map(p.y, 240, 3800, 1, height);

      tft.drawRect(x-50,y-25,50,50,65535);
      delay(100);
      tft.drawRect(x-50,y-25,50,50,0);

      if (x  > width-100 && y > height-100) zoom = 0.01; // bottom right - return to home
      else {
        x -= width/2;
        y -= height/2;
        cx += x / 1000.0;
        cy += y / 1000.0;
        zoom *= 0.8;
      }

      i = 0;
    }
}
