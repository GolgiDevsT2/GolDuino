//
//  GolgiStuff.m
//  GolDuino
//
//  Created by Brian Kelly on 2/26/15.
//  Copyright (c) 2015 Golgi. All rights reserved.
//

#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import "GolgiStuff.h"
#import "GOLGI_KEYS.h"
#import "GolDuinoSvcWrapper.h"
#import "AppDelegate.h"

static GolgiStuff *instance = nil;


@implementation GolgiStuff
@synthesize viewController;

+ (GolgiStuff *)getInstance
{
    if(instance == nil){
        instance = [[GolgiStuff alloc] init];
    }
    
    return instance;
}

- (void)scoobyDoo
{
    NSLog(@"Hello Scooby Doo");
}


- (void)sendRequestTo:(NSString *)dest
{
    /*
    PushData *pData = [[PushData alloc] init];
    [pData setData:[NSString stringWithFormat:@"%ld", (long)viewController.outboundCount]];
    
    NSLog(@"Sending to %@", dest);
    [PushButtonSvc sendButtonPushedUsingResultHandler:^(PushButtonButtonPushedExceptionBundle *excBundle) {
        if(viewController != nil){
            if(excBundle == nil){
                [self scoobyDoo];
                NSLog(@"Hello Scrappy Doo");
                [viewController responseReceived];
            }
            else{
                [viewController errorReceived];
            }
        }
    } withTransportOptions:stdGto andDestination:dest withPushData:pData];
    NSLog(@"Gone");
     */
}
- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag
{
 
    NSLog(@"Player finished: %d", flag);
}

// GOLGI
//********************************* Registration ***************************
//
// Setup handling of inbound SendMessage methods and then Register with Golgi
//
- (void)doGolgiRegistration
{
    //
    // Do this before registration because on registering, there may be messages queued
    // up for us that would arrive and be rejected because there is no handler in place
    //
    
    // [TapTelegraphSvc registerSendMessageRequestReceiver:self];
    
    //
    // and now do the main registration with the service
    //
    NSLog(@"Registering with golgiId: '%@'", [AppDelegate getInstanceId]);
    
    
    [GolDuinoSvc registerSetPotValueRequestHandler:^(id<GolDuinoSetPotValueResultSender> resultSender, NSInteger v) {
        NSLog(@"Set Port: %ld", v);
        if(viewController != nil){
            NSLog(@"In Foreground, set gauge");
            [viewController setSliderValue:v];
        }
        else{
            NSLog(@"In Background, ignore");
        }
        [resultSender success];
    }];
    
    [GolDuinoSvc registerButtonPressedRequestHandler:^(id<GolDuinoButtonPressedResultSender> resultSender, NSInteger t) {
        NSLog(@"Button Pressed");
        [resultSender success];
    }];

    [GolDuinoSvc registerButtonReleasedRequestHandler:^(id<GolDuinoButtonReleasedResultSender> resultSender, NSInteger t) {
        NSLog(@"Button Released");
        NSURL *url = [NSURL fileURLWithPath:[[NSBundle mainBundle]
                                             pathForResource:@"dingdong"
                                             ofType:@"wav"]];
        NSError *error = nil;
        audioPlayer = [[AVAudioPlayer alloc]
                                       initWithContentsOfURL:url
                                       error:&error];
        if (error)
        {
            NSLog(@"Error in audioPlayer: %@",[error localizedDescription]);
        }
        else
        {
            [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayback error:nil];
            [[AVAudioSession sharedInstance] setActive: YES error: nil];
            
            
            NSLog(@"No error, playing audio");
            audioPlayer.delegate = self;
            [audioPlayer play];
            [audioPlayer setNumberOfLoops:0]; // for continuous play
        }
        UILocalNotification* localNotification = [[UILocalNotification alloc] init];
        localNotification.alertBody = @"Somone at the door";
        [[UIApplication sharedApplication] cancelAllLocalNotifications];
        [[UIApplication sharedApplication] scheduleLocalNotification:localNotification];

        [resultSender success];
    }];

    
    // [Golgi setOption:@"USE_DEV_CLUSTER" withValue:@"0"];
    /*
    [GolDuinoSvc registerButtonPushedRequestHandler:^(id<PushButtonButtonPushedResultSender> resultSender, PushData *pushData) {
        NSLog(@"Received request");
        [resultSender success];
        if(viewController != nil){
            [viewController requestReceived];
        }
        
        UILocalNotification* localNotification = [[UILocalNotification alloc] init];
        localNotification.alertBody = [NSString stringWithFormat:@"Received Request: %@", [pushData getData]];
        [[UIApplication sharedApplication] cancelAllLocalNotifications];
        [[UIApplication sharedApplication] scheduleLocalNotification:localNotification];
        NSLog(@"D");
        AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
        NSLog(@"E");
        
    }];
     */
    
    NSData *pushId = [AppDelegate getPushId];
    
    if(pushId.length > 0){
#ifdef DEBUG
        [Golgi setDevPushToken:pushId];
#else
        [Golgi setProdPushToken:pushId];
#endif
    }
    NSString *instanceId = [AppDelegate getInstanceId];
    if(instanceId.length > 0){
        [Golgi registerWithDevId:GOLGI_DEV_KEY
                           appId:GOLGI_APP_KEY
                          instId:instanceId
                andResultHandler:^(NSString *errorText) {
                    if(errorText != nil){
                        NSLog(@"Golgi Registration: FAIL => '%@'", errorText);
                    }
                    else{
                        NSLog(@"Golgi Registration: PASS");
                    }
                }];
    }
}


- (void)setInstanceId:(NSString *)newInstanceId
{
    [AppDelegate setInstanceId:newInstanceId];
    
    [self doGolgiRegistration];
}

- (void)setPushId:(NSData *)newPushId
{
    NSData *pushId = [AppDelegate getPushId];
    if(pushId.length != newPushId.length || memcmp([pushId bytes], [newPushId bytes], pushId.length) != 0){
        [AppDelegate setPushId:newPushId];
        [self doGolgiRegistration];
    }
}

- (NSString *)pushTokenToString:(NSData *)token
{
    NSMutableString *hexStr = [[NSMutableString alloc]init];
    
    for(int i = 0; i < token.length; i++){
        [hexStr appendFormat:@"%02x", ((unsigned char *)[token bytes])[i]];
    }
    
    return [NSString stringWithString:hexStr];
}

- (GolgiStuff *)init
{
    self = [super init];
    viewController = nil;
    
    
    stdGto = [[GolgiTransportOptions alloc] init];
    [stdGto setValidityPeriodInSeconds:3600];
    
    launchTime = time(NULL);
    
    
    [self doGolgiRegistration];
    
    return self;
}

@end
