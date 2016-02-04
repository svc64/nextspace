/*
  Screen.m

  Controller class for Screen preferences bundle

  Author:	Sergii Stoian <stoyan255@ukr.net>
  Date:		28 Nov 2015

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public
  License along with this program; if not, write to:

  Free Software Foundation, Inc.
  59 Temple Place - Suite 330
  Boston, MA  02111-1307, USA
*/
#import <AppKit/NSApplication.h>
#import <AppKit/NSNibLoading.h>
#import <AppKit/NSView.h>
#import <AppKit/NSBox.h>
#import <AppKit/NSImage.h>
#import <AppKit/NSButton.h>
#import <AppKit/NSTableView.h>
#import <AppKit/NSTableColumn.h>
#import <AppKit/NSBrowser.h>
#import <AppKit/NSMatrix.h>
#import <AppKit/NSGraphics.h>

#import "Screen.h"

@implementation ScreenPreferences

static NSBundle                 *bundle = nil;
static NSUserDefaults           *defaults = nil;
static NSMutableDictionary      *domain = nil;

- (id)init
{
  self = [super init];
  
  defaults = [NSUserDefaults standardUserDefaults];
  domain = [[defaults persistentDomainForName:NSGlobalDomain] mutableCopy];

  bundle = [NSBundle bundleForClass:[self class]];
  NSString *imagePath = [bundle pathForResource:@"Screen" ofType:@"tiff"];
  image = [[NSImage alloc] initWithContentsOfFile:imagePath];
      
  return self;
}

- (void)dealloc
{
  [image release];
  [displayBoxList release];
  [super dealloc];
}

- (void)awakeFromNib
{
  [view retain];
  [window release];

  // Get info about monitors and layout
  displayBoxList = [[NSMutableArray alloc] init];
  [self updateDisplayBoxList];
}

- (NSView *)view
{
  if (view == nil)
    {
      if (![NSBundle loadNibNamed:@"Screen" owner:self])
        {
          NSLog (@"Screen.preferences: Could not load NIB, aborting.");
          return nil;
        }
    }
  
  return view;
}

- (NSString *)buttonCaption
{
  return @"Screen Preferences";
}

- (NSImage *)buttonImage
{
  return image;
}

//
// Action methods
//

//
// Helper methods
//
- (void)updateDisplayBoxList
{
  NSArray *displays;
  NSRect  canvasRect = [[canvas contentView] frame];
  NSRect  displayRect;
  CGFloat dMaxWidth = 0.0, dMaxHeight = 0.0;
  CGFloat scaleWidth, scaleHeight;
  DisplayBox *dBox;

  // Clear view and array
  for (dBox in displayBoxList)
    {
      [dBox removeFromSuperview];
    }
  dBox = nil;
  [displayBoxList removeAllObjects];
  
  displays = [[NXScreen sharedScreen] allDisplays];
  // Calculate scale factor
  for (NXDisplay *d in displays)
    {
      displayRect = [d frame];
      if (dMaxWidth < displayRect.size.width ||
          dMaxHeight < displayRect.size.height)
        {
          dMaxWidth = displayRect.size.width;
          dMaxHeight = displayRect.size.height;
        }
    }
  scaleWidth = (canvasRect.size.width/dMaxWidth) / 3;
  scaleHeight = (canvasRect.size.height/dMaxHeight) / 3;
  scaleFactor = (scaleWidth < scaleHeight) ? scaleWidth : scaleHeight;

  // Create and add display boxes
  for (NXDisplay *d in displays)
    {
      displayRect = [d frame];
      displayRect.origin.x = floor(displayRect.origin.x*scaleFactor)+0.5;
      displayRect.origin.y = floor(displayRect.origin.y*scaleFactor)+0.5;
      displayRect.size.width = floor(displayRect.size.width*scaleFactor);
      displayRect.size.height = floor(displayRect.size.height*scaleFactor);

      dBox = [[DisplayBox alloc] initWithFrame:displayRect];
      [displayBoxList addObject:dBox];
      [canvas addSubview:dBox];
    }

  [self arrangeDisplayBoxes];
}

- (void)arrangeDisplayBoxes
{
  NSRect  dRect, sRect = [canvas frame];
  NSSize  screenSize = [[NXScreen sharedScreen] sizeInPixels];
  CGFloat xOffset, yOffset;
  
  xOffset = (sRect.size.width - (screenSize.width * scaleFactor))/2;
  // Align boxes at that top edge
  yOffset = sRect.size.height - (sRect.size.height - (screenSize.height * scaleFactor))/2;
  
  for (DisplayBox *dBox in displayBoxList)
    {
      dRect = [dBox frame];
      dRect.origin.x += xOffset;
      dRect.origin.y += (yOffset - dRect.size.height);
      NSLog(@"Display Box rec: %@", NSStringFromRect(dRect));
      [dBox setFrame:dRect];
    }
}

@end

@implementation ScreenCanvas

- initWithFrame:(NSRect)frameRect
{
  self = [super initWithFrame:frameRect];
  [self setBorderType:NSBezelBorder];
  [self setTitlePosition:NSNoTitle];
  [self setFillColor:[NSColor grayColor]];
  [self setContentViewMargins:NSMakeSize(0, 0)];

  return self;
}

- (void)drawRect:(NSRect)rect
{
  [super drawRect:rect];

  [_fill_color set];
  NSRectFill([[self contentView] frame]);
}

@end

@implementation DisplayBox

- initWithFrame:(NSRect)frameRect
{
  self = [super initWithFrame:frameRect];
  [self setBorderType:NSLineBorder];
  [self setTitlePosition:NSNoTitle];
  [self setContentViewMargins:NSMakeSize(1, 1)];

  return self;
}

- (void)setMainDisplay:(BOOL)isMain
{
}

- (void)mouseDown
{
}

- (void)drawRect:(NSRect)rect
{
  [super drawRect:rect];
  
  [_fill_color set];
  NSRectFill([[self contentView] frame]);
}

@end
