//
//  ViewController.h
//  GolDuino
//
//  Created by Brian Kelly on 2/26/15.
//  Copyright (c) 2015 Golgi. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GolgiStuff.h"

@class GolgiStuff;

@interface ViewController : UIViewController

@property IBOutlet UISwitch *ledSwitch;
@property IBOutlet UIImageView *ledIv;
@property IBOutlet UISlider *slider;
@property GolgiStuff *golgiStuff;

- (void)ledSwitchChanged:(UISwitch *)sw;
- (void)setSliderValue:(NSInteger)value;

@end

