/******************************************************************************/
/* This file has been generated by the uGFX-Studio                            */
/*                                                                            */
/* http://ugfx.org                                                            */
/******************************************************************************/

#include "user_colors.h"
#include "user_styles.h"
#include "gui.h"

// GListeners
GListener gl;

// GHandles
GHandle ghContainerPage0;
GHandle ghKeyboard1;
GHandle ghTextedit1_copy;
GHandle ghTextedit1_copy_1;
GHandle ghTextedit1_copy_2;
GHandle ghContainerPage1;
GHandle ghSlider1;
GHandle ghSlider1_1;
GHandle ghList1;
GHandle ghList1_1;

// Fonts
font_t dejavu_sans_20_anti_aliased;

// Images
gdispImage ugfx_150x50_282828;

static void containerDraw_Default(GWidgetObject *gw, void *param)
{
    (void)param;

    // Clear container area
    gdispGFillArea(gw->g.display, gw->g.x, gw->g.y, gw->g.width, gw->g.height, gw->pstyle->background);

    // Draw the elements
    gdispImageDraw(&ugfx_150x50_282828, 324, 6, 150, 50, 0, 0);
}

static void createPagePage0(void)
{
    GWidgetInit wi;
    gwinWidgetClearInit(&wi);

    // create container widget: ghContainerPage0
    wi.g.show = FALSE;
    wi.g.x = 0;
    wi.g.y = 0;
    wi.g.width = 480;
    wi.g.height = 272;
    wi.g.parent = 0;
    wi.text = "Container";
    wi.customDraw = containerDraw_Default;
    wi.customParam = 0;
    wi.customStyle = 0;
    ghContainerPage0 = gwinContainerCreate(0, &wi, 0);

    // Create keyboard widget: ghKeyboard1
    wi.g.show = TRUE;
    wi.g.x = 20;
    wi.g.y = 130;
    wi.g.width = 450;
    wi.g.height = 130;
    wi.g.parent = ghContainerPage0;
    wi.text = "Keyboard1";
    wi.customDraw = gwinKeyboardDraw_Normal;
    wi.customParam = 0;
    wi.customStyle = 0;
    ghKeyboard1 = gwinKeyboardCreate(0, &wi);
    gwinSetFont(ghKeyboard1, dejavu_sans_20_anti_aliased);
    gwinRedraw(ghKeyboard1);

    // Create textedit widget: ghTextedit1_copy
    wi.g.show = TRUE;
    wi.g.x = 20;
    wi.g.y = 70;
    wi.g.width = 210;
    wi.g.height = 40;
    wi.g.parent = ghContainerPage0;
    wi.text = "First Name";
    wi.customDraw = gwinTexteditDefaultDraw;
    wi.customParam = 0;
    wi.customStyle = 0;
    ghTextedit1_copy = gwinTexteditCreate(0, &wi, 30);
    gwinSetFont(ghTextedit1_copy, dejavu_sans_20_anti_aliased);
    gwinRedraw(ghTextedit1_copy);

    // Create textedit widget: ghTextedit1_copy_1
    wi.g.show = TRUE;
    wi.g.x = 260;
    wi.g.y = 70;
    wi.g.width = 210;
    wi.g.height = 40;
    wi.g.parent = ghContainerPage0;
    wi.text = "Last Name";
    wi.customDraw = gwinTexteditDefaultDraw;
    wi.customParam = 0;
    wi.customStyle = 0;
    ghTextedit1_copy_1 = gwinTexteditCreate(0, &wi, 30);

    // Create textedit widget: ghTextedit1_copy_2
    wi.g.show = TRUE;
    wi.g.x = 20;
    wi.g.y = 10;
    wi.g.width = 210;
    wi.g.height = 40;
    wi.g.parent = ghContainerPage0;
    wi.text = "User Name";
    wi.customDraw = gwinTexteditDefaultDraw;
    wi.customParam = 0;
    wi.customStyle = 0;
    ghTextedit1_copy_2 = gwinTexteditCreate(0, &wi, 30);
}

static void createPagePage1(void)
{
    GWidgetInit wi;
    gwinWidgetClearInit(&wi);

    // create container widget: ghContainerPage1
    wi.g.show = FALSE;
    wi.g.x = 0;
    wi.g.y = 0;
    wi.g.width = 480;
    wi.g.height = 272;
    wi.g.parent = 0;
    wi.text = "Container";
    wi.customDraw = 0;
    wi.customParam = 0;
    wi.customStyle = 0;
    ghContainerPage1 = gwinContainerCreate(0, &wi, 0);

    // create button widget: ghSlider1
    wi.g.show = TRUE;
    wi.g.x = 20;
    wi.g.y = 10;
    wi.g.width = 210;
    wi.g.height = 30;
    wi.g.parent = ghContainerPage1;
    wi.text = "Slider1";
    wi.customDraw = gwinSliderDraw_Std;
    wi.customParam = 0;
    wi.customStyle = 0;
    ghSlider1 = gwinSliderCreate(0, &wi);
    gwinSliderSetRange(ghSlider1, 0, 100);
    gwinSliderSetPosition(ghSlider1, 35);
    gwinSetColor(ghSlider1, Black);
    gwinSetBgColor(ghSlider1, White);

    // create button widget: ghSlider1_1
    wi.g.show = TRUE;
    wi.g.x = 20;
    wi.g.y = 50;
    wi.g.width = 210;
    wi.g.height = 30;
    wi.g.parent = ghContainerPage1;
    wi.text = "Slider1";
    wi.customDraw = gwinSliderDraw_Std;
    wi.customParam = 0;
    wi.customStyle = 0;
    ghSlider1_1 = gwinSliderCreate(0, &wi);
    gwinSliderSetRange(ghSlider1_1, 0, 100);
    gwinSliderSetPosition(ghSlider1_1, 35);
    gwinSetColor(ghSlider1_1, Black);
    gwinSetBgColor(ghSlider1_1, White);
    gwinDisable(ghSlider1_1);

    // Create list widget: ghList1
    wi.g.show = TRUE;
    wi.g.x = 280;
    wi.g.y = 20;
    wi.g.width = 170;
    wi.g.height = 100;
    wi.g.parent = ghContainerPage1;
    wi.text = "List1";
    wi.customDraw = gwinListDefaultDraw;
    wi.customParam = 0;
    wi.customStyle = 0;
    ghList1 = gwinListCreate(0, &wi, FALSE);
    gwinListSetScroll(ghList1, scrollSmooth);
    gwinListAddItem(ghList1, "Item1", FALSE);
    gwinListAddItem(ghList1, "Item2", FALSE);
    gwinListAddItem(ghList1, "Item3", FALSE);
    gwinListAddItem(ghList1, "Item4", FALSE);
    gwinListAddItem(ghList1, "Item5", FALSE);
    gwinListAddItem(ghList1, "Item6", FALSE);
    gwinListSetSelected(ghList1, 0, FALSE);
    gwinListSetSelected(ghList1, 1, FALSE);
    gwinListSetSelected(ghList1, 2, FALSE);
    gwinListSetSelected(ghList1, 3, FALSE);
    gwinListSetSelected(ghList1, 4, FALSE);
    gwinListSetSelected(ghList1, 5, FALSE);

    // Create list widget: ghList1_1
    wi.g.show = TRUE;
    wi.g.x = 280;
    wi.g.y = 130;
    wi.g.width = 170;
    wi.g.height = 100;
    wi.g.parent = ghContainerPage1;
    wi.text = "List1";
    wi.customDraw = gwinListDefaultDraw;
    wi.customParam = 0;
    wi.customStyle = 0;
    ghList1_1 = gwinListCreate(0, &wi, FALSE);
    gwinListSetScroll(ghList1_1, scrollSmooth);
    gwinListAddItem(ghList1_1, "Item1", FALSE);
    gwinListAddItem(ghList1_1, "Item2", FALSE);
    gwinListAddItem(ghList1_1, "Item3", FALSE);
    gwinListAddItem(ghList1_1, "Item4", FALSE);
    gwinListAddItem(ghList1_1, "Item5", FALSE);
    gwinListAddItem(ghList1_1, "Item6", FALSE);
    gwinListSetSelected(ghList1_1, 0, FALSE);
    gwinListSetSelected(ghList1_1, 1, FALSE);
    gwinListSetSelected(ghList1_1, 2, FALSE);
    gwinListSetSelected(ghList1_1, 3, FALSE);
    gwinListSetSelected(ghList1_1, 4, FALSE);
    gwinListSetSelected(ghList1_1, 5, FALSE);
    gwinDisable(ghList1_1);
}

void guiShowPage(unsigned pageIndex)
{
    // Hide all pages
    gwinHide(ghContainerPage0);
    gwinHide(ghContainerPage1);

    // Show page selected page
    switch (pageIndex) {
    case 0:
        gwinShow(ghContainerPage0);
        break;

    case 1:
        gwinShow(ghContainerPage1);
        break;

    default:
        break;
    }
}

void guiCreate(void)
{
    GWidgetInit wi;

    // Prepare fonts
    dejavu_sans_20_anti_aliased = gdispOpenFont("DejaVuSans20_aa");

    // Prepare images
    gdispImageOpenFile(&ugfx_150x50_282828, "rsc/ugfx_150x50_282828.bmp");

    // GWIN settings
    gwinWidgetClearInit(&wi);
    gwinSetDefaultFont(dejavu_sans_20_anti_aliased);
    gwinSetDefaultStyle(&BlackBerry, FALSE);
    gwinSetDefaultColor(Black);
    gwinSetDefaultBgColor(White);

    // Create all the display pages
    createPagePage0();
    createPagePage1();

    // Select the default display page
    guiShowPage(0);
}

void guiEventLoop(void)
{
    GEvent *pe;

    while (1) {
        // Get an event
        pe = geventEventWait(&gl, 0);
        switch (pe->type) {
            /* Todo: */
        }
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}
