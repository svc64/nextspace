/*
  Copyright (C) 2015 Sergii Stoian <stoyan255@ukr.net>

  This file is a part of Terminal.app. Terminal.app is free software; you
  can redistribute it and/or modify it under the terms of the GNU General
  Public License as published by the Free Software Foundation; version 2
  of the License. See COPYING or main.m for more information.
*/

#import "Preferences.h"

#import "ColorsPrefs.h"
#import "DisplayPrefs.h"
#import "LinuxPrefs.h"
#import "ShellPrefs.h"
#import "StartupPrefs.h"
#import "TitleBarPrefs.h"
#import "WindowPrefs.h"

// This class manages key/main window state for FontPanel opening
@implementation PreferencesPanel
- (BOOL)canBecomeMainWindow
{
  return fontPanelOpened;
}
- (void)fontPanelOpened:(BOOL)isOpened
{
  fontPanelOpened = isOpened;
  if (isOpened == YES)
    {
      mainWindow = [NSApp mainWindow];
      [self makeMainWindow];
    }
  else
    {
      [mainWindow makeMainWindow];
      [self makeKeyWindow];
    }
}
@end

@implementation Preferences

static Preferences *shared = nil;

+ shared
{
  if (shared == nil)
    {
      shared = [self new];
    }
  return shared;
}

- init
{
  self = shared = [super init];
  
  prefModules = [NSDictionary dictionaryWithObjectsAndKeys:
                                [ColorsPrefs new], [ColorsPrefs name],
                              [DisplayPrefs new], [DisplayPrefs name],
                              [LinuxPrefs new], [LinuxPrefs name],
                              [ShellPrefs new], [ShellPrefs name],
                              [StartupPrefs new], [StartupPrefs name],
                              [TitleBarPrefs new], [TitleBarPrefs name],
                              [WindowPrefs new], [WindowPrefs name],
                              nil];
  [prefModules retain];

  if (!ud)
    {
      ud = [NSUserDefaults standardUserDefaults];
    }

  [[NSNotificationCenter defaultCenter]
    addObserver:self
       selector:@selector(mainWindowDidChange:)
           name:NSWindowDidBecomeMainNotification
         object:nil];

  return self;
}

- (void)activatePanel
{
  if (window == nil)
    {
      [NSBundle loadNibNamed:@"PreferencesPanel" owner:self];
    }
  else
    {
      [self switchMode:modeBtn];
    }
  [window makeKeyAndOrderFront:nil];
}

- (void)awakeFromNib
{
  [window setFrameAutosaveName:@"Preferences"];
  [modeBtn selectItemAtIndex:0];
  [self switchMode:modeBtn];
}

- (void)closePanel
{
  [window close];
}


- (void)switchMode:(id)sender
{
  id <PrefsModule> module;
  NSView *mView;

  module = [prefModules objectForKey:[sender titleOfSelectedItem]];
  mView = [module view];
  // if ([modeContentBox contentView] != mView)
  //   {
      [(NSBox *)modeContentBox setContentView:mView];
      [module showDefault:self];
    // }
}

- (void)mainWindowDidChange:(NSNotification *)notif
{
  NSLog(@"Prefrences: main window now: %@", [[notif object] title]);
}


@end
