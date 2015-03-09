//
//  ViewController.m
//  GolDuino
//
//  Created by Brian Kelly on 2/26/15.
//  Copyright (c) 2015 Golgi. All rights reserved.
//

#import "ViewController.h"
#import "GolgiStuff.h"

@interface ViewController ()

@end

@implementation ViewController
@synthesize ledSwitch;
@synthesize ledIv;
@synthesize golgiStuff;
@synthesize slider;

- (void)setSliderValue:(NSInteger)value
{
    slider.value = (float)((float)value / 1023.0);
}

- (void)ledSwitchChanged:(UISwitch *)sw
{
    BOOL on = sw.on;
    NSLog(@"Switch Changed");
    if(on){
        [ledIv setImage:[UIImage imageNamed:@"redled.png"]];
    }
    else{
        [ledIv setImage:[UIImage imageNamed:@"redled_dark.png"]];        
    }
    
    IOState *ios = [[IOState alloc] init];
    [ios setLedState:(on) ? 1 : 0];
    
    [GolDuinoSvc sendSetIOUsingResultHandler:^(GolDuinoSetIOExceptionBundle *bundle) {
        if(bundle.golgiException != nil){
            NSLog(@"setIO: FAIL");
        }
        else{
            NSLog(@"setIO: SUCCESS");
        }
        
    } andDestination:@"BK-DUINO" withIoState:ios];
    
    
}

- (void)viewDidLoad {
    [super viewDidLoad];
    [ledSwitch addTarget:self action:@selector(ledSwitchChanged:) forControlEvents:UIControlEventValueChanged];
    
    golgiStuff = [GolgiStuff getInstance];
    golgiStuff.viewController = self;
    [golgiStuff setInstanceId:@"iPhone-5S"];

}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
