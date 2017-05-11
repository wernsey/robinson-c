#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "refcnt.h"
#include "refhash.h"
#include "refalist.h"
#include "dom.h"
#include "stream.h"
#include "html.h"
#include "css.h"
#include "style.h"
#include "layout.h"
#include "display.h"
#include "print.h"
#include "canvas.h"

int main(int argc, char *argv[]) {
    
	rc_init();
	
    //~ const char *html = "robinson-master/examples/test.html";
    //~ const char *css = "robinson-master/examples/test.css";	
    const char *html = "test/simple.html";
    const char *css = "test/simple.css";
    
    Stream *r = file_stream(html);
    //Stream *r = string_stream("test.html");
    //Stream *r = string_stream("<html><head>HEAD</head><body>BODY</body></html>");
    if(!r) {
        fprintf(stderr, "error: no %s\n", html);
        return 1;
    }
    Node *n = parse(r);
	strm_done(r);
	if(!n) {
		fprintf(stderr, "error: Couldn't parse %s\n", html);
		return 1;
	}
    
	r = file_stream(css);
    if(!r) {
        fprintf(stderr, "error: no %s\n", css);
        return 1;
    }
    Stylesheet *ss = parse_rules(r);
	strm_done(r);
	if(!ss) {
		fprintf(stderr, "error: Couldn't parse %s\n", css);
		return 1;
	}
	
	//print_node(n, 0);
	//print_styles(ss);
		
	StyledNode *sn = style_tree(n, ss);
	//print_stylednode(sn, 0);
	
	Dimensions viewport;
	memset(&viewport, 0, sizeof viewport);
	viewport.content.width = 800.0f;
	viewport.content.height = 600.0f;
    
    LayoutBox *lb = layout_tree(sn, viewport);
	print_layoutbox(lb, 0);
    
    ArrayList *displayList = build_display_list(lb);
    print_displaylist(displayList);
    
    Canvas *canvas = paint(displayList, viewport.content);
    
    canvas_save(canvas, "render.gif");
    
	rc_release(n);
	rc_release(ss);
	rc_release(sn);
	rc_release(lb);
	rc_release(displayList);
	rc_release(canvas);
	
    return 0;
}