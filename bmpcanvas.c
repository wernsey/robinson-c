#include <stdio.h>
#include <stdlib.h>

#include "refcnt.h"
#include "refalist.h"
#include "refhash.h"
#include "dom.h"
#include "css.h"
#include "style.h"
#include "layout.h"
#include "display.h"
#include "bmp.h"
#include "canvas.h"

static void canvas_dtor(void *p) {
	Canvas *canvas = p;
    Bitmap *bmp = canvas->data;
    bm_free(bmp);
}

static Canvas *new_canvas(int width, int height) {
    Canvas *canvas = rc_alloc(sizeof *canvas);
	rc_set_dtor(canvas, canvas_dtor);
    canvas->width = width;
    canvas->height = height;
    
    canvas->data = bm_create(canvas->width, canvas->height);        
    bm_set_color(canvas->data, 0xFFFFFF);
    bm_clear(canvas->data);
    
	return canvas;
}

static void paint_item(Canvas *self, DisplayCommand *item) {
    Bitmap *bm = self->data;
    switch(item->type) {
        case SolidColor: {
            bm_set_color(bm, bm_atoi(item->solidColor.color));
            Rect rect = item->solidColor.rect;
            bm_fillrect(bm, rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);            
        } break;
        default: break;
    }
}

Canvas *paint(ArrayList *displayList, Rect bounds) {
    int i;
    Canvas *canvas = new_canvas((int)bounds.width, (int)bounds.height);
    for(i = 0; i < al_size(displayList); i++) {
        DisplayCommand *item = al_get(displayList, i);
        paint_item(canvas, item);
    }
    return canvas;
}

void canvas_save(Canvas *canvas, const char *filename) {
    bm_save(canvas->data, filename);
}
