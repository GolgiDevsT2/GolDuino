//
//  AppDelegate.m
//  GolDuino
//
//  Created by Brian Kelly on 2/26/15.
//  Copyright (c) 2015 Golgi. All rights reserved.
//

#import "AppDelegate.h"
#import "libGolgi.h"

@interface AppDelegate ()

@end

@implementation AppDelegate

#define PUSH_KEY @"GOLGI-PUSH-ID"
#define INST_KEY @"GOLGI-INSTANCE-ID"

+ (NSString *)getInstanceId
{
    return [GolgiStore getStringForKey:INST_KEY withDefault:@""];
}

+ (void)setInstanceId:(NSString *)instanceId
{
    [GolgiStore deleteStringForKey:INST_KEY];
    [GolgiStore putString:instanceId forKey:INST_KEY];
    
}

+ (NSData *)getPushId
{
    return [[NSData alloc] initWithBase64EncodedString:[GolgiStore getStringForKey:PUSH_KEY withDefault:@""] options:0];
}

+ (void)setPushId:(NSData *)pushId
{
    [GolgiStore deleteStringForKey:PUSH_KEY];
    [GolgiStore putString:[pushId base64EncodedStringWithOptions:0] forKey:PUSH_KEY];
}

- (void)application:(UIApplication*)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData*)deviceToken
{
    UILocalNotification* localNotification = [[UILocalNotification alloc] init];
    localNotification.alertBody = @"Registered for Push";
    localNotification.timeZone = [NSTimeZone defaultTimeZone];
    // [[UIApplication sharedApplication] scheduleLocalNotification:localNotification];
    
    NSLog(@"Ok for push");
    
    
    [AppDelegate setPushId:deviceToken];
    
#ifdef DEBUG
    [Golgi setDevPushToken:deviceToken];
#else
    [Golgi setProdPushToken:deviceToken];
#endif
    
}

- (void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error
{
    NSLog(@"Failed to register for remote notifications: %@", error);
}

- (void)application:(UIApplication*)application didReceiveRemoteNotification:(NSDictionary*)userInfo  fetchCompletionHandler:(void (^)(UIBackgroundFetchResult))completionHandler
{
    
    if([Golgi isGolgiPushData:userInfo]){
        NSLog(@"Golgi Received notification(1): %@", userInfo);
        /*
         UILocalNotification* localNotification = [[UILocalNotification alloc] init];
         localNotification.alertBody = @"Received Golgi Push";
         [[UIApplication sharedApplication] cancelAllLocalNotifications];
         [[UIApplication sharedApplication] scheduleLocalNotification:localNotification];
         */
        
        [Golgi pushReceived:userInfo withCompletionHandler:completionHandler];
    }
    else{
        //
        // Not a PUSH for Golgi, handle as normal in the application
        //
        NSLog(@"Application received a Remote Notification not for Golgi");
        completionHandler(UIBackgroundFetchResultNoData);
    }
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo
{
    [self application:application didReceiveRemoteNotification:userInfo fetchCompletionHandler:nil];
}



- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    {extern void enableOMNLogging(); enableOMNLogging();}
    NSLog(@"applicationDidFinishLaunching()");
    
    [Golgi setOption:@"USE_TEST_SERVER" withValue:@"1"];
    
    [[UIApplication sharedApplication] beginReceivingRemoteControlEvents];

#ifdef DEBUG
    [Golgi setDevPushToken:[AppDelegate getPushId]];
    // [Golgi setProdPushToken:[AppDelegate getPushId]];
#else
    [Golgi setProdPushToken:[AppDelegate getPushId]];
#endif
    
    golgiStuff = [GolgiStuff getInstance];

    if(launchOptions != nil) {
        // Launched from push notification
        NSDictionary *d = [launchOptions objectForKey:UIApplicationLaunchOptionsRemoteNotificationKey];
        if(d != nil){
            //
            // Ok, launched into the background, setup Golgi
            //
            
            /*
             UILocalNotification* localNotification = [[UILocalNotification alloc] init];
             localNotification.alertBody = @"Launching into BG";
             [[UIApplication sharedApplication] cancelAllLocalNotifications];
             [[UIApplication sharedApplication] scheduleLocalNotification:localNotification];
             */
            
            [Golgi enteringBackground];
            [Golgi useEphemeralConnection];
        }
    }
    
    
    //
    // Lifted from StackOverflow, how to register for push
    // in a backwards compatible way
    //
    if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 8.0){
        [[UIApplication sharedApplication] registerUserNotificationSettings:[UIUserNotificationSettings settingsForTypes:(UIUserNotificationTypeSound | UIUserNotificationTypeAlert | UIUserNotificationTypeBadge) categories:nil]];
        [[UIApplication sharedApplication] registerForRemoteNotifications];
    }
    else{
        [[UIApplication sharedApplication] registerForRemoteNotificationTypes:
         (UIRemoteNotificationTypeBadge | UIRemoteNotificationTypeSound | UIRemoteNotificationTypeAlert)];
    }
    
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}


- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
    
    
    //
    // GOLGI: Tell the framework that we are going into the background
    //
    NSLog(@"applicationDidEnterBackground()");
    [Golgi enteringBackground];
    [Golgi useEphemeralConnection];
    NSLog(@"applicationDidEnterBackground() complete");
    
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    
    //
    // GOLGI: Tell the framework that we are active again
    //
    
    NSLog(@"applicationDidBecomeActive()");
    [Golgi enteringForeground];
    [Golgi usePersistentConnection];
    
}
- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
