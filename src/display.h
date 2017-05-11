typedef struct {
    enum {SolidColor} type;
    
    union {
        struct { char *color; Rect rect; } solidColor;
        // More display commands to follow...
    };
    
} DisplayCommand;

ArrayList *build_display_list(LayoutBox *layout_root);
