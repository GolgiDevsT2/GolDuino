//
//  GolgiStuff.h
//  GolDuino
//
//  Created by Brian Kelly on 2/26/15.
//  Copyright (c) 2015 Golgi. All rights reserved.
//


#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreLocation/CoreLocation.h>
#import "GolDuinoSvcWrapper.h"
#import "ViewController.h"
#import "libGolgi.h"

@class ViewController;

@interface GolgiStuff : NSObject <AVAudioPlayerDelegate>
{
    GolgiTransportOptions *stdGto;
    long launchTime;
    AVAudioPlayer * audioPlayer;
}

@property ViewController *viewController;
+ (GolgiStuff *)getInstance;

- (void)sendRequestTo:(NSString *)dest;


- (void)setInstanceId:(NSString *)newInstanceId;

- (void)stopAutoPilot;


@end