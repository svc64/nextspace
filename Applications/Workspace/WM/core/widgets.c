/*
 *  Workspace window manager
 *  Copyright (c) 2015- Sergii Stoian
 *
 *  WINGs library (Window Maker)
 *  Copyright (c) 1998 scottc
 *  Copyright (c) 1999-2004 Dan Pascu
 *  Copyright (c) 1999-2000 Alfredo K. Kojima
 *  Copyright (c) 2014 Window Maker Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <X11/Xft/Xft.h>

#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>

#include <X11/Xlocale.h>

#include "WMcore.h"
#include "util.h"
#include "log_utils.h"

#include "WM.h"
#include "wscreen.h"
#include "wappresource.h"
#include "winputmethod.h"
#include "wevent.h"
#include "wpixmap.h"
#include "wcolor.h"
#include "wballoon.h"
#include "drawing.h"

#include "widgets.h"

/********** data ************/

#define CHECK_BUTTON_ON_WIDTH 	16
#define CHECK_BUTTON_ON_HEIGHT 	16

static char *CHECK_BUTTON_ON[] = {
                                  "               %",
                                  " .............%#",
                                  " ........... .%#",
                                  " .......... #.%#",
                                  " ......... #%.%#",
                                  " ........ #%..%#",
                                  " ... #.. #%...%#",
                                  " ... #% #%....%#",
                                  " ... % #%.....%#",
                                  " ...  #%......%#",
                                  " ... #%.......%#",
                                  " ...#%........%#",
                                  " .............%#",
                                  " .............%#",
                                  " %%%%%%%%%%%%%%#",
                                  "%###############"
};

#define CHECK_BUTTON_OFF_WIDTH 	16
#define CHECK_BUTTON_OFF_HEIGHT	16

static char *CHECK_BUTTON_OFF[] = {
                                   "               %",
                                   " .............%#",
                                   " .............%#",
                                   " .............%#",
                                   " .............%#",
                                   " .............%#",
                                   " .............%#",
                                   " .............%#",
                                   " .............%#",
                                   " .............%#",
                                   " .............%#",
                                   " .............%#",
                                   " .............%#",
                                   " .............%#",
                                   " %%%%%%%%%%%%%%#",
                                   "%###############"
};

#define RADIO_BUTTON_ON_WIDTH 	15
#define RADIO_BUTTON_ON_HEIGHT	15
static char *RADIO_BUTTON_ON[] = {
                                  ".....%%%%%.....",
                                  "...%%#####%%...",
                                  "..%##.....%.%..",
                                  ".%#%..    .....",
                                  ".%#.        ...",
                                  "%#..        .. ",
                                  "%#.          . ",
                                  "%#.          . ",
                                  "%#.          . ",
                                  "%#.          . ",
                                  ".%%.        . .",
                                  ".%..        . .",
                                  "..%...    .. ..",
                                  ".... .....  ...",
                                  ".....     .....",
};

#define RADIO_BUTTON_OFF_WIDTH 	15
#define RADIO_BUTTON_OFF_HEIGHT	15
static char *RADIO_BUTTON_OFF[] = {
                                   ".....%%%%%.....",
                                   "...%%#####%%...",
                                   "..%##.......%..",
                                   ".%#%...........",
                                   ".%#............",
                                   "%#............ ",
                                   "%#............ ",
                                   "%#............ ",
                                   "%#............ ",
                                   "%#............ ",
                                   ".%%.......... .",
                                   ".%........... .",
                                   "..%......... ..",
                                   ".... .....  ...",
                                   ".....     .....",
};

#define TRISTATE_BUTTON_ON_WIDTH 	15
#define TRISTATE_BUTTON_ON_HEIGHT	15
static char *TRISTATE_BUTTON_ON[] = {
                                     "%%%%%%%%%%%%%%.",
                                     "%%%%%%%%%%%%%. ",
                                     "%%           . ",
                                     "%% ##     ## . ",
                                     "%% ###   ### . ",
                                     "%%  ### ###  . ",
                                     "%%   #####   . ",
                                     "%%    ###    . ",
                                     "%%   #####   . ",
                                     "%%  ### ###  . ",
                                     "%% ###   ### . ",
                                     "%% ##     ## . ",
                                     "%%           . ",
                                     "%............. ",
                                     ".              ",
};

#define TRISTATE_BUTTON_OFF_WIDTH 	15
#define TRISTATE_BUTTON_OFF_HEIGHT	15
static char *TRISTATE_BUTTON_OFF[] = {
                                      "%%%%%%%%%%%%%%.",
                                      "%%%%%%%%%%%%%. ",
                                      "%%           . ",
                                      "%%           . ",
                                      "%%           . ",
                                      "%%           . ",
                                      "%%           . ",
                                      "%%           . ",
                                      "%%           . ",
                                      "%%           . ",
                                      "%%           . ",
                                      "%%           . ",
                                      "%%           . ",
                                      "%............. ",
                                      ".              ",
};

#define TRISTATE_BUTTON_TRI_WIDTH 	15
#define TRISTATE_BUTTON_TRI_HEIGHT	15
static char *TRISTATE_BUTTON_TRI[] = {
                                      "%%%%%%%%%%%%%%.",
                                      "%%%%%%%%%%%%%. ",
                                      "%%           . ",
                                      "%% # # # # # . ",
                                      "%%  # # # #  . ",
                                      "%% # # # # # . ",
                                      "%%  # # # #  . ",
                                      "%% # # # # # . ",
                                      "%%  # # # #  . ",
                                      "%% # # # # # . ",
                                      "%%  # # # #  . ",
                                      "%% # # # # # . ",
                                      "%%           . ",
                                      "%............. ",
                                      ".              ",
};

static char *BUTTON_ARROW[] = {
                               "..................",
                               "....##....#### ...",
                               "...#.%....#... ...",
                               "..#..%#####... ...",
                               ".#............ ...",
                               "#............. ...",
                               ".#............ ...",
                               "..#..          ...",
                               "...#. ............",
                               "....# ............"
};

#define BUTTON_ARROW_WIDTH	18
#define BUTTON_ARROW_HEIGHT	10

static char *BUTTON_ARROW2[] = {
                                "                  ",
                                "    ##    ####.   ",
                                "   # %    #   .   ",
                                "  #  %#####   .   ",
                                " #            .   ",
                                "#             .   ",
                                " #            .   ",
                                "  #  ..........   ",
                                "   # .            ",
                                "    #.            "
};

#define BUTTON_ARROW2_WIDTH	18
#define BUTTON_ARROW2_HEIGHT	10

static char *SCROLLER_DIMPLE[] = {
                                  ".%###.",
                                  "%#%%%%",
                                  "#%%...",
                                  "#%..  ",
                                  "#%.   ",
                                  ".%.  ."
};

#define SCROLLER_DIMPLE_WIDTH   6
#define SCROLLER_DIMPLE_HEIGHT  6

static char *SCROLLER_ARROW_UP[] = {
                                    "....%....",
                                    "....#....",
                                    "...%#%...",
                                    "...###...",
                                    "..%###%..",
                                    "..#####..",
                                    ".%#####%.",
                                    ".#######.",
                                    "%#######%"
};

static char *HI_SCROLLER_ARROW_UP[] = {
                                       "    %    ",
                                       "    %    ",
                                       "   %%%   ",
                                       "   %%%   ",
                                       "  %%%%%  ",
                                       "  %%%%%  ",
                                       " %%%%%%% ",
                                       " %%%%%%% ",
                                       "%%%%%%%%%"
};

#define SCROLLER_ARROW_UP_WIDTH   9
#define SCROLLER_ARROW_UP_HEIGHT  9

static char *SCROLLER_ARROW_DOWN[] = {
                                      "%#######%",
                                      ".#######.",
                                      ".%#####%.",
                                      "..#####..",
                                      "..%###%..",
                                      "...###...",
                                      "...%#%...",
                                      "....#....",
                                      "....%...."
};

static char *HI_SCROLLER_ARROW_DOWN[] = {
                                         "%%%%%%%%%",
                                         " %%%%%%% ",
                                         " %%%%%%% ",
                                         "  %%%%%  ",
                                         "  %%%%%  ",
                                         "   %%%   ",
                                         "   %%%   ",
                                         "    %    ",
                                         "    %    "
};

#define SCROLLER_ARROW_DOWN_WIDTH   9
#define SCROLLER_ARROW_DOWN_HEIGHT  9

static char *SCROLLER_ARROW_LEFT[] = {
                                      "........%",
                                      "......%##",
                                      "....%####",
                                      "..%######",
                                      "%########",
                                      "..%######",
                                      "....%####",
                                      "......%##",
                                      "........%"
};

static char *HI_SCROLLER_ARROW_LEFT[] = {
                                         "        %",
                                         "      %%%",
                                         "    %%%%%",
                                         "  %%%%%%%",
                                         "%%%%%%%%%",
                                         "  %%%%%%%",
                                         "    %%%%%",
                                         "      %%%",
                                         "        %"
};

#define SCROLLER_ARROW_LEFT_WIDTH   9
#define SCROLLER_ARROW_LEFT_HEIGHT  9

static char *SCROLLER_ARROW_RIGHT[] = {
                                       "%........",
                                       "##%......",
                                       "####%....",
                                       "######%..",
                                       "########%",
                                       "######%..",
                                       "####%....",
                                       "##%......",
                                       "%........"
};

static char *HI_SCROLLER_ARROW_RIGHT[] = {
                                          "%        ",
                                          "%%%      ",
                                          "%%%%%    ",
                                          "%%%%%%%  ",
                                          "%%%%%%%%%",
                                          "%%%%%%%  ",
                                          "%%%%%    ",
                                          "%%%      ",
                                          "%        "
};

#define SCROLLER_ARROW_RIGHT_WIDTH   9
#define SCROLLER_ARROW_RIGHT_HEIGHT  9

static char *POPUP_INDICATOR[] = {
                                  "        #==",
                                  " ......%#==",
                                  " ......%#%%",
                                  " ......%#%%",
                                  " %%%%%%%#%%",
                                  "#########%%",
                                  "==%%%%%%%%%",
                                  "==%%%%%%%%%"
};

#define POPUP_INDICATOR_WIDTH	11
#define POPUP_INDICATOR_HEIGHT 	8

static char *PULLDOWN_INDICATOR[] = {
                                     "=#######=",
                                     "=%===== =",
                                     "==%=== ==",
                                     "==%=== ==",
                                     "===%= ===",
                                     "===%= ===",
                                     "====%===="
};

#define PULLDOWN_INDICATOR_WIDTH	9
#define PULLDOWN_INDICATOR_HEIGHT 	7

#define CHECK_MARK_WIDTH 	8
#define CHECK_MARK_HEIGHT 	10

static char *CHECK_MARK[] = {
                             "======== ",
                             "======= #",
                             "====== #%",
                             "===== #%=",
                             " #== #%==",
                             " #% #%===",
                             " % #%====",
                             "  #%=====",
                             " #%======",
                             "#%======="
};

#define STIPPLE_WIDTH 8
#define STIPPLE_HEIGHT 8
static char STIPPLE_BITS[] = {
                              0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55
};

static int userWidgetCount = 0;

/*****  end data  ******/

static void renderPixmap(WMScreen *screen, Pixmap d, Pixmap mask, char **data, int width, int height)
{
  int x, y;
  GC whiteGC = WMColorGC(screen->white);
  GC blackGC = WMColorGC(screen->black);
  GC lightGC = WMColorGC(screen->gray);
  GC darkGC = WMColorGC(screen->darkGray);

  if (mask)
    XSetForeground(screen->display, screen->monoGC, 0);

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      switch (data[y][x]) {
      case ' ':
      case 'w':
        XDrawPoint(screen->display, d, whiteGC, x, y);
        break;

      case '=':
        if (mask)
          XDrawPoint(screen->display, mask, screen->monoGC, x, y);

      case '.':
      case 'l':
        XDrawPoint(screen->display, d, lightGC, x, y);
        break;

      case '%':
      case 'd':
        XDrawPoint(screen->display, d, darkGC, x, y);
        break;

      case '#':
      case 'b':
      default:
        XDrawPoint(screen->display, d, blackGC, x, y);
        break;
      }
    }
  }
}

static WMPixmap *makePixmap(WMScreen *sPtr, char **data, int width, int height, int masked)
{
  Pixmap pixmap, mask = None;

  pixmap = XCreatePixmap(sPtr->display, WMDRAWABLE(sPtr), width, height, sPtr->depth);

  if (masked) {
    mask = XCreatePixmap(sPtr->display, WMDRAWABLE(sPtr), width, height, 1);
    XSetForeground(sPtr->display, sPtr->monoGC, 1);
    XFillRectangle(sPtr->display, mask, sPtr->monoGC, 0, 0, width, height);
  }

  renderPixmap(sPtr, pixmap, mask, data, width, height);

  return WMCreatePixmapFromXPixmaps(sPtr, pixmap, mask, width, height, sPtr->depth);
}

WMScreen *WMCreateScreenWithRContext(Display *display, int screen, RContext *context)
{
  WMScreen *scrPtr;
  XGCValues gcv;
  Pixmap stipple;
  static int initialized = 0;
  static char *atomNames[] = {
                              "_GNUSTEP_WM_ATTR",
                              "WM_DELETE_WINDOW",
                              "WM_PROTOCOLS",
                              "CLIPBOARD",
                              "XdndAware",
                              "XdndSelection",
                              "XdndEnter",
                              "XdndLeave",
                              "XdndPosition",
                              "XdndDrop",
                              "XdndFinished",
                              "XdndTypeList",
                              "XdndActionList",
                              "XdndActionDescription",
                              "XdndStatus",
                              "XdndActionCopy",
                              "XdndActionMove",
                              "XdndActionLink",
                              "XdndActionAsk",
                              "XdndActionPrivate",
                              "_WINGS_DND_MOUSE_OFFSET",
                              "WM_STATE",
                              "UTF8_STRING",
                              "_NET_WM_NAME",
                              "_NET_WM_ICON_NAME",
                              "_NET_WM_ICON",
  };
  Atom atoms[wlengthof(atomNames)];
  int i;

  if (!initialized) {
    initialized = 1;
    assert(WMApplicationInitialized());
  }

  scrPtr = malloc(sizeof(WMScreen));
  if (!scrPtr)
    return NULL;
  memset(scrPtr, 0, sizeof(WMScreen));

  scrPtr->aflags.hasAppIcon = 1;

  scrPtr->display = display;
  scrPtr->screen = screen;
  scrPtr->rcontext = context;

  scrPtr->depth = context->depth;

  scrPtr->visual = context->visual;
  scrPtr->lastEventTime = 0;

  scrPtr->colormap = context->cmap;

  scrPtr->rootWin = RootWindow(display, screen);

  scrPtr->fontCache = WMCreateHashTable(WMStringPointerHashCallbacks);

  scrPtr->xftdraw = XftDrawCreate(scrPtr->display, WMDRAWABLE(scrPtr), scrPtr->visual, scrPtr->colormap);

  /* Create missing CUT_BUFFERs */
  {
    Atom *rootWinProps;
    int exists[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    int count;

    rootWinProps = XListProperties(display, scrPtr->rootWin, &count);
    for (i = 0; i < count; i++) {
      switch (rootWinProps[i]) {
      case XA_CUT_BUFFER0:
        exists[0] = 1;
        break;
      case XA_CUT_BUFFER1:
        exists[1] = 1;
        break;
      case XA_CUT_BUFFER2:
        exists[2] = 1;
        break;
      case XA_CUT_BUFFER3:
        exists[3] = 1;
        break;
      case XA_CUT_BUFFER4:
        exists[4] = 1;
        break;
      case XA_CUT_BUFFER5:
        exists[5] = 1;
        break;
      case XA_CUT_BUFFER6:
        exists[6] = 1;
        break;
      case XA_CUT_BUFFER7:
        exists[7] = 1;
        break;
      default:
        break;
      }
    }
    if (rootWinProps) {
      XFree(rootWinProps);
    }
    for (i = 0; i < 8; i++) {
      if (!exists[i]) {
        XStoreBuffer(display, "", 0, i);
      }
    }
  }

  scrPtr->ignoredModifierMask = 0;
  {
    int i;
    XModifierKeymap *modmap;
    KeyCode nlock, slock;
    static int mask_table[8] = {
                                ShiftMask, LockMask, ControlMask, Mod1Mask,
                                Mod2Mask, Mod3Mask, Mod4Mask, Mod5Mask
    };
    unsigned int numLockMask = 0, scrollLockMask = 0;

    nlock = XKeysymToKeycode(display, XK_Num_Lock);
    slock = XKeysymToKeycode(display, XK_Scroll_Lock);

    /*
     *Find out the masks for the NumLock and ScrollLock modifiers,
     *so that we can bind the grabs for when they are enabled too.
     */
    modmap = XGetModifierMapping(display);

    if (modmap != NULL && modmap->max_keypermod > 0) {
      for (i = 0; i < 8 *modmap->max_keypermod; i++) {
        if (modmap->modifiermap[i] == nlock && nlock != 0)
          numLockMask = mask_table[i / modmap->max_keypermod];
        else if (modmap->modifiermap[i] == slock && slock != 0)
          scrollLockMask = mask_table[i / modmap->max_keypermod];
      }
    }

    if (modmap)
      XFreeModifiermap(modmap);

    scrPtr->ignoredModifierMask = numLockMask | scrollLockMask | LockMask;
  }

  /* initially allocate some colors */
  WMWhiteColor(scrPtr);
  WMBlackColor(scrPtr);
  WMGrayColor(scrPtr);
  WMDarkGrayColor(scrPtr);

  gcv.graphics_exposures = False;

  gcv.function = GXxor;
  gcv.foreground = WMPIXEL(scrPtr->white);
  if (gcv.foreground == 0)
    gcv.foreground = 1;
  scrPtr->xorGC = XCreateGC(display, WMDRAWABLE(scrPtr), GCFunction
                            | GCGraphicsExposures | GCForeground, &gcv);

  gcv.function = GXxor;
  gcv.foreground = WMPIXEL(scrPtr->gray);
  gcv.subwindow_mode = IncludeInferiors;
  scrPtr->ixorGC = XCreateGC(display, WMDRAWABLE(scrPtr), GCFunction
                             | GCGraphicsExposures | GCForeground | GCSubwindowMode, &gcv);

  gcv.function = GXcopy;
  scrPtr->copyGC = XCreateGC(display, WMDRAWABLE(scrPtr), GCFunction | GCGraphicsExposures, &gcv);

  scrPtr->clipGC = XCreateGC(display, WMDRAWABLE(scrPtr), GCFunction | GCGraphicsExposures, &gcv);

  stipple = XCreateBitmapFromData(display, WMDRAWABLE(scrPtr), STIPPLE_BITS, STIPPLE_WIDTH, STIPPLE_HEIGHT);
  gcv.foreground = WMPIXEL(scrPtr->darkGray);
  gcv.background = WMPIXEL(scrPtr->gray);
  gcv.fill_style = FillStippled;
  gcv.stipple = stipple;
  scrPtr->stippleGC = XCreateGC(display, WMDRAWABLE(scrPtr),
                                GCForeground | GCBackground | GCStipple
                                | GCFillStyle | GCGraphicsExposures, &gcv);

  scrPtr->drawStringGC = XCreateGC(display, WMDRAWABLE(scrPtr), GCGraphicsExposures, &gcv);
  scrPtr->drawImStringGC = XCreateGC(display, WMDRAWABLE(scrPtr), GCGraphicsExposures, &gcv);

  /* we need a 1bpp drawable for the monoGC, so borrow this one */
  scrPtr->monoGC = XCreateGC(display, stipple, 0, NULL);

  scrPtr->stipple = stipple;

  /* scrPtr->antialiasedText = WINGsConfiguration.antialiasedText; */
  scrPtr->antialiasedText = False; /* TODO */
  WMLogWarning("[TODO] Antialised text is set to false. No option exists to configure it.");

  scrPtr->normalFont = WMSystemFontOfSize(scrPtr, 0);

  scrPtr->boldFont = WMBoldSystemFontOfSize(scrPtr, 0);

  if (!scrPtr->boldFont)
    scrPtr->boldFont = scrPtr->normalFont;

  if (!scrPtr->normalFont) {
    WMLogWarning(_("could not load any fonts. Make sure your font installation"
               " and locale settings are correct."));

    return NULL;
  }

  /* create input method stuff */
  WMInitIM(scrPtr);

  scrPtr->checkButtonImageOn = makePixmap(scrPtr, CHECK_BUTTON_ON,
                                          CHECK_BUTTON_ON_WIDTH, CHECK_BUTTON_ON_HEIGHT, False);

  scrPtr->checkButtonImageOff = makePixmap(scrPtr, CHECK_BUTTON_OFF,
                                           CHECK_BUTTON_OFF_WIDTH, CHECK_BUTTON_OFF_HEIGHT, False);

  scrPtr->radioButtonImageOn = makePixmap(scrPtr, RADIO_BUTTON_ON,
                                          RADIO_BUTTON_ON_WIDTH, RADIO_BUTTON_ON_HEIGHT, False);

  scrPtr->radioButtonImageOff = makePixmap(scrPtr, RADIO_BUTTON_OFF,
                                           RADIO_BUTTON_OFF_WIDTH, RADIO_BUTTON_OFF_HEIGHT, False);

  scrPtr->tristateButtonImageOn = makePixmap(scrPtr, TRISTATE_BUTTON_ON,
                                             TRISTATE_BUTTON_ON_WIDTH, TRISTATE_BUTTON_ON_HEIGHT, False);

  scrPtr->tristateButtonImageOff = makePixmap(scrPtr, TRISTATE_BUTTON_OFF,
                                              TRISTATE_BUTTON_OFF_WIDTH, TRISTATE_BUTTON_OFF_HEIGHT, False);

  scrPtr->tristateButtonImageTri = makePixmap(scrPtr, TRISTATE_BUTTON_TRI,
                                              TRISTATE_BUTTON_TRI_WIDTH, TRISTATE_BUTTON_TRI_HEIGHT, False);

  scrPtr->buttonArrow = makePixmap(scrPtr, BUTTON_ARROW, BUTTON_ARROW_WIDTH, BUTTON_ARROW_HEIGHT, False);

  scrPtr->pushedButtonArrow = makePixmap(scrPtr, BUTTON_ARROW2,
                                         BUTTON_ARROW2_WIDTH, BUTTON_ARROW2_HEIGHT, False);

  scrPtr->scrollerDimple = makePixmap(scrPtr, SCROLLER_DIMPLE,
                                      SCROLLER_DIMPLE_WIDTH, SCROLLER_DIMPLE_HEIGHT, False);

  scrPtr->upArrow = makePixmap(scrPtr, SCROLLER_ARROW_UP,
                               SCROLLER_ARROW_UP_WIDTH, SCROLLER_ARROW_UP_HEIGHT, True);

  scrPtr->downArrow = makePixmap(scrPtr, SCROLLER_ARROW_DOWN,
                                 SCROLLER_ARROW_DOWN_WIDTH, SCROLLER_ARROW_DOWN_HEIGHT, True);

  scrPtr->leftArrow = makePixmap(scrPtr, SCROLLER_ARROW_LEFT,
                                 SCROLLER_ARROW_LEFT_WIDTH, SCROLLER_ARROW_LEFT_HEIGHT, True);

  scrPtr->rightArrow = makePixmap(scrPtr, SCROLLER_ARROW_RIGHT,
                                  SCROLLER_ARROW_RIGHT_WIDTH, SCROLLER_ARROW_RIGHT_HEIGHT, True);

  scrPtr->hiUpArrow = makePixmap(scrPtr, HI_SCROLLER_ARROW_UP,
                                 SCROLLER_ARROW_UP_WIDTH, SCROLLER_ARROW_UP_HEIGHT, True);

  scrPtr->hiDownArrow = makePixmap(scrPtr, HI_SCROLLER_ARROW_DOWN,
                                   SCROLLER_ARROW_DOWN_WIDTH, SCROLLER_ARROW_DOWN_HEIGHT, True);

  scrPtr->hiLeftArrow = makePixmap(scrPtr, HI_SCROLLER_ARROW_LEFT,
                                   SCROLLER_ARROW_LEFT_WIDTH, SCROLLER_ARROW_LEFT_HEIGHT, True);

  scrPtr->hiRightArrow = makePixmap(scrPtr, HI_SCROLLER_ARROW_RIGHT,
                                    SCROLLER_ARROW_RIGHT_WIDTH, SCROLLER_ARROW_RIGHT_HEIGHT, True);

  scrPtr->popUpIndicator = makePixmap(scrPtr, POPUP_INDICATOR,
                                      POPUP_INDICATOR_WIDTH, POPUP_INDICATOR_HEIGHT, True);

  scrPtr->pullDownIndicator = makePixmap(scrPtr, PULLDOWN_INDICATOR,
                                         PULLDOWN_INDICATOR_WIDTH, PULLDOWN_INDICATOR_HEIGHT, True);

  scrPtr->checkMark = makePixmap(scrPtr, CHECK_MARK, CHECK_MARK_WIDTH, CHECK_MARK_HEIGHT, True);

  scrPtr->defaultCursor = XCreateFontCursor(display, XC_left_ptr);

  scrPtr->textCursor = XCreateFontCursor(display, XC_xterm);

  {
    XColor bla;
    Pixmap blank;

    blank = XCreatePixmap(display, scrPtr->stipple, 1, 1, 1);
    XSetForeground(display, scrPtr->monoGC, 0);
    XFillRectangle(display, blank, scrPtr->monoGC, 0, 0, 1, 1);

    scrPtr->invisibleCursor = XCreatePixmapCursor(display, blank, blank, &bla, &bla, 0, 0);
    XFreePixmap(display, blank);
  }

#ifdef HAVE_XINTERNATOMS
  XInternAtoms(display, atomNames, wlengthof(atomNames), False, atoms);
#else
  for (i = 0; i < wlengthof(atomNames); i++) {
    atoms[i] = XInternAtom(display, atomNames[i], False);
  }
#endif

  i = 0;
  scrPtr->attribsAtom = atoms[i++];

  scrPtr->deleteWindowAtom = atoms[i++];

  scrPtr->protocolsAtom = atoms[i++];

  scrPtr->clipboardAtom = atoms[i++];

  scrPtr->xdndAwareAtom = atoms[i++];
  scrPtr->xdndSelectionAtom = atoms[i++];
  scrPtr->xdndEnterAtom = atoms[i++];
  scrPtr->xdndLeaveAtom = atoms[i++];
  scrPtr->xdndPositionAtom = atoms[i++];
  scrPtr->xdndDropAtom = atoms[i++];
  scrPtr->xdndFinishedAtom = atoms[i++];
  scrPtr->xdndTypeListAtom = atoms[i++];
  scrPtr->xdndActionListAtom = atoms[i++];
  scrPtr->xdndActionDescriptionAtom = atoms[i++];
  scrPtr->xdndStatusAtom = atoms[i++];

  scrPtr->xdndActionCopy = atoms[i++];
  scrPtr->xdndActionMove = atoms[i++];
  scrPtr->xdndActionLink = atoms[i++];
  scrPtr->xdndActionAsk = atoms[i++];
  scrPtr->xdndActionPrivate = atoms[i++];

  scrPtr->wmIconDragOffsetAtom = atoms[i++];

  scrPtr->wmStateAtom = atoms[i++];

  scrPtr->utf8String = atoms[i++];
  scrPtr->netwmName = atoms[i++];
  scrPtr->netwmIconName = atoms[i++];
  scrPtr->netwmIcon = atoms[i++];

  scrPtr->rootView = WMCreateRootView(scrPtr);

  scrPtr->balloon = WMCreateBalloon(scrPtr);

  WMInitApplication(scrPtr);

  return scrPtr;
}

void WMDestroyWidget(WMWidget *widget)
{
  WMUnmapView(WMVIEW(widget));
  WMDestroyView(WMVIEW(widget));
}

/* Realizes the widget and all it's children. */
void WMRealizeWidget(WMWidget *w)
{
  WMRealizeView(WMVIEW(w));
}

void WMMapWidget(WMWidget *w)
{
  WMMapView(WMVIEW(w));
}

void WMUnmapWidget(WMWidget *w)
{
  WMUnmapView(WMVIEW(w));
}

static void makeChildrenAutomap(WMView *view, int flag)
{
  view = view->childrenList;

  while (view) {
    view->flags.mapWhenRealized = flag;
    makeChildrenAutomap(view, flag);

    view = view->nextSister;
  }
}

void WMMapSubwidgets(WMWidget *w)
{
  /* make sure that subwidgets created after the parent was realized
   *are mapped too */
  if (!WMVIEW(w)->flags.realized) {
    makeChildrenAutomap(WMVIEW(w), True);
  } else {
    WMMapSubviews(WMVIEW(w));
  }
}

void WMSetWidgetBackgroundColor(WMWidget *w, WMColor *color)
{
  WMSetViewBackgroundColor(WMVIEW(w), color);
  if (WMVIEW(w)->flags.mapped)
    WMRedisplayWidget(w);
}

void WMMoveWidget(WMWidget *w, int x, int y)
{
  WMMoveView(WMVIEW(w), x, y);
}

void WMResizeWidget(WMWidget *w, unsigned int width, unsigned int height)
{
  WMResizeView(WMVIEW(w), width, height);
}

WMClass WMRegisterUserWidget(void)
{
  userWidgetCount++;

  return userWidgetCount + WC_UserWidget - 1;
}

Display *WMScreenDisplay(WMScreen *scr)
{
  return scr->display;
}

void WMRedisplayWidget(WMWidget *w)
{
  WMRedisplayView(WMVIEW(w));
}





/* RContext *WMScreenRContext(WMScreen *scr) */
/* { */
/*   return scr->rcontext; */
/* } */

/* WMColor *WMGetWidgetBackgroundColor(WMWidget *w) */
/* { */
/*   return WMVIEW(w)->backColor; */
/* } */

/* void WMSetWidgetBackgroundPixmap(WMWidget *w, WMPixmap *pix) */
/* { */
/*   if (!pix) */
/*     return; */

/*   WMSetViewBackgroundPixmap(WMVIEW(w), pix); */
/*   if (WMVIEW(w)->flags.mapped) */
/*     WMRedisplayWidget(w); */
/* } */

/* WMPixmap *WMGetWidgetBackgroundPixmap(WMWidget *w) */
/* { */
/*   return WMVIEW(w)->backImage; */
/* } */

/* void WMRaiseWidget(WMWidget *w) */
/* { */
/*   WMRaiseView(WMVIEW(w)); */
/* } */

/* void WMLowerWidget(WMWidget *w) */
/* { */
/*   WMLowerView(WMVIEW(w)); */
/* } */

/* void WMUnmapSubwidgets(WMWidget *w) */
/* { */
/*   if (!WMVIEW(w)->flags.realized) { */
/*     makeChildrenAutomap(WMVIEW(w), False); */
/*   } else { */
/*     WMUnmapSubviews(WMVIEW(w)); */
/*   } */
/* } */

/* Bool WMWidgetIsMapped(WMWidget *w) */
/* { */
/*   return WMVIEW(w)->flags.mapped; */
/* } */

/* void WMReparentWidget(WMWidget *w, WMWidget *newParent, int x, int y) */
/* { */
/*   WMReparentView(WMVIEW(w), WMVIEW(newParent), x, y); */
/* } */

/* void WMSetFocusToWidget(WMWidget *widget) */
/* { */
/*   WMSetFocusOfTopLevel(WMTopLevelOfView(WMVIEW(widget)), WMVIEW(widget)); */
/* } */

/* WMScreen *WMOpenScreen(const char *display) */
/* { */
/*   Display *dpy = XOpenDisplay(display); */

/*   if (!dpy) { */
/*     WMLogWarning(_("WINGs: could not open display %s"), XDisplayName(display)); */
/*     return NULL; */
/*   } */

/*   return WMCreateSimpleApplicationScreen(dpy); */
/* } */

/* WMScreen *WMCreateSimpleApplicationScreen(Display *display) */
/* { */
/*   WMScreen *scr; */

/*   scr = WMCreateScreen(display, DefaultScreen(display)); */

/*   scr->aflags.hasAppIcon = 0; */
/*   scr->aflags.simpleApplication = 1; */

/*   return scr; */
/* } */

/* WMScreen *WMCreateScreen(Display *display, int screen) */
/* { */
/*   return WMCreateScreenWithRContext(display, screen, RCreateContext(display, screen, NULL)); */
/* } */

/* void WMSetWidgetDefaultFont(WMScreen *scr, WMFont *font) */
/* { */
/*   WMReleaseFont(scr->normalFont); */
/*   scr->normalFont = WMRetainFont(font); */
/* } */

/* void WMSetWidgetDefaultBoldFont(WMScreen *scr, WMFont *font) */
/* { */
/*   WMReleaseFont(scr->boldFont); */
/*   scr->boldFont = WMRetainFont(font); */
/* } */

/* void WMHangData(WMWidget *widget, void *data) */
/* { */
/*   WMVIEW(widget)->hangedData = data; */
/* } */

/* void *WMGetHangedData(WMWidget *widget) */
/* { */
/*   return WMVIEW(widget)->hangedData; */
/* } */

