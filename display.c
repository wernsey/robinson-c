#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "refcnt.h"
#include "refalist.h"
#include "refhash.h"
#include "dom.h"
#include "css.h"
#include "style.h"
#include "layout.h"
#include "display.h"

static void render_layout_box(ArrayList *list, LayoutBox *layout_box);
static void render_background(ArrayList *list, LayoutBox *layout_box);
static void render_borders(ArrayList *list, LayoutBox *layout_box);

#if 0
#define TRACE(s)  do{printf("%d: %s\n", __LINE__, s);fflush(stdout);}while(0);
#else
#define TRACE(s)
#endif

static void display_command_dtor(void *p) {
	DisplayCommand *cmd = p;
    switch(cmd->type) {
        case SolidColor: rc_release(cmd->solidColor.color); break;
    }
}

static DisplayCommand *new_display_command() {
	DisplayCommand *root = rc_alloc(sizeof *root);
	rc_set_dtor(root, display_command_dtor);
	return root;
}

ArrayList *build_display_list(LayoutBox *layout_root) {
    ArrayList *list = al_create();
    render_layout_box(list, layout_root);
    return list;
}

static void render_layout_box(ArrayList *list, LayoutBox *layout_box) {
    
    TRACE("render_layout_box");
    
    render_background(list, layout_box);
    render_borders(list, layout_box);
    
    // TODO: render text
    
    int i;
	for(i = 0; i < al_size(layout_box->children); i++) {
		LayoutBox *child = al_get(layout_box->children, i);
        render_layout_box(list, child);
    }
}

static char *get_color(LayoutBox *layout_box, const char *name) {
    TRACE("get_color");
    if(layout_box->box_type.type == BlockNode || layout_box->box_type.type == InlineNode) {
        TRACE("get_color");
        Value *v = style_value(layout_box->box_type.node, name);
        TRACE("get_color");
        if(v && v->type == Color) return v->color_txt;
        TRACE("get_color");
    }
    TRACE("get_color");
    return NULL;
}

static void render_background(ArrayList *list, LayoutBox *layout_box) {
    TRACE("render_background");
    char *color = get_color(layout_box, "background");
    if(color) {
        TRACE("render_background");
        DisplayCommand *cmd = new_display_command();
        TRACE("render_background");
        cmd->type = SolidColor;
        TRACE("render_background");
        cmd->solidColor.rect = border_box(&layout_box->dimensions);
        TRACE("render_background");
        cmd->solidColor.color = rc_retain(color);
        TRACE("render_background");
        al_add(list, cmd);
    }
}

static DisplayCommand *make_rect_cmd(float x, float y, float w, float h, char *color) {
    DisplayCommand *cmd = new_display_command();
    cmd->type = SolidColor;
    cmd->solidColor.rect.x = x;
    cmd->solidColor.rect.y = y;
    cmd->solidColor.rect.width = w;
    cmd->solidColor.rect.height = h;
    cmd->solidColor.color = rc_retain(color);
    return cmd;
}

static void render_borders(ArrayList *list, LayoutBox *layout_box) {
    TRACE("render_borders");
    char *color = get_color(layout_box, "border-color");
    if(!color) return;
    
    Dimensions *d = &layout_box->dimensions;
    Rect bb = border_box(d);
    
    // Left border
    al_add(list, make_rect_cmd(
        bb.x, 
        bb.y, 
        d->border.left, 
        bb.height, 
        color));
    // Right border
    al_add(list, make_rect_cmd(
        bb.x + bb.width - d->border.right, 
        bb.y, 
        d->border.right, 
        bb.height, 
        color));
    // Top border
    al_add(list, make_rect_cmd(
        bb.x, 
        bb.y, 
        bb.width, 
        d->border.top, 
        color));
    // Bottom border
    al_add(list, make_rect_cmd(
        bb.x, 
        bb.y + bb.height - d->border.bottom, 
        bb.width,
        d->border.bottom, 
        color));
}
