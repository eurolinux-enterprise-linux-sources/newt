#include <slang.h>
#include <stdlib.h>
#include <string.h>

#include "newt.h"
#include "newt_pr.h"

struct scale {
    long long fullValue;
    int charsSet;
    unsigned int percentage;
};

static void scaleDraw(newtComponent co);

static struct componentOps scaleOps = {
    scaleDraw,
    newtDefaultEventHandler,
    NULL,
    newtDefaultPlaceHandler,
    newtDefaultMappedHandler,
} ;

newtComponent newtScale(int left, int top, int width, long long fullValue) {
    newtComponent co;
    struct scale * sc;

    co = malloc(sizeof(*co));
    sc = malloc(sizeof(struct scale));
    co->data = sc;
    co->destroyCallback = NULL;

    co->ops = &scaleOps;

    co->height = 1;
    co->width = width;
    co->top = top;
    co->left = left;
    co->takesFocus = 0;

    sc->fullValue = fullValue;
    sc->charsSet = 0;
    sc->percentage = 0;

    return co;
}

void newtScaleSet(newtComponent co, unsigned long long amount) {
    struct scale * sc = co->data;
    int newPercentage;

    if (amount >= sc->fullValue) {
	newPercentage = 100;
	sc->charsSet = co->width;
    } else if (sc->fullValue >= -1ULL / (100 > co->width ? 100 : co->width)) {
	/* avoid overflow on large numbers */
	sc->charsSet = amount / (sc->fullValue / co->width);
	newPercentage = amount / (sc->fullValue / 100);
    } else {
	sc->charsSet = (amount * co->width) / sc->fullValue;
	newPercentage = (amount * 100) / sc->fullValue;
    }
    
    if (newPercentage != sc->percentage) {
	sc->percentage = newPercentage;
	scaleDraw(co);
    }
}

static void scaleDraw(newtComponent co) {
    struct scale * sc = co->data;
    int i;
    int xlabel = (co->width-4) /2;
    char percent[10];
    
    if (co->top == -1) return;

    newtGotorc(co->top, co->left);

    sprintf(percent, "%3d%%", sc->percentage);

    SLsmg_set_color(NEWT_COLORSET_FULLSCALE);
    
    for (i = 0; i < co->width; i++) {
        if (i == sc->charsSet)
            SLsmg_set_color(NEWT_COLORSET_EMPTYSCALE);
        if (i >= xlabel && i < xlabel+4)
            SLsmg_write_char(percent[i-xlabel]);
        else
            SLsmg_write_char(' ');
    }
    /* put cursor at beginning of text for better accessibility */
    newtGotorc(co->top, co->left + xlabel);
}
