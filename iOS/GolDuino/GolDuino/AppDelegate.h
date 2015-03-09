//
//  AppDelegate.h
//  GolDuino
//
//  Created by Brian Kelly on 2/26/15.
//  Copyright (c) 2015 Golgi. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GolgiStuff.h"

@interface AppDelegate : UIResponder <UIApplicationDelegate>
{
    GolgiStuff *golgiStuff;
}
+ (NSString *)getInstanceId;
+ (void)setInstanceId:(NSString *)instanceId;
+ (NSData *)getPushId;
+ (void)setPushId:(NSData *)pushId;

@property (strong, nonatomic) UIWindow *window;


@end

