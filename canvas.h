typedef struct {
    int width;
    int height;
    void *data;
} Canvas;

Canvas *paint(ArrayList *displayList, Rect bounds);

void canvas_save(Canvas *canvas, const char *filename);
