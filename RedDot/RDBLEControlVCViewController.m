//
//  RDBLEControlVCViewController.m
//  RedDot
//
//  Created by air on 14-6-26.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import "RDBLEControlVCViewController.h"

@interface RDBLEControlVCViewController ()

@end

@implementation RDBLEControlVCViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    [self.btnMode setImage:[UIImage imageNamed:@"img_mode_u.png"] forState:UIControlStateNormal];
    [self.btnMode setImage:[UIImage imageNamed:@"img_mode_d.png"] forState:UIControlStateHighlighted];
    [self.btn_add setImage:[UIImage imageNamed:@"img_add_u.png"] forState:UIControlStateNormal];
    [self.btn_add setImage:[UIImage imageNamed:@"img_add_d.png"] forState:UIControlStateHighlighted];
    [self.btnMinus setImage:[UIImage imageNamed:@"img_minus_u.png"] forState:UIControlStateNormal];
    [self.btnMinus setImage:[UIImage imageNamed:@"img_minus_d.png"] forState:UIControlStateHighlighted];
    [self.btnSound setImage:[UIImage imageNamed:@"img_sound_u.png"] forState:UIControlStateNormal];
    [self.btnSound setImage:[UIImage imageNamed:@"img_sound_d.png"] forState:UIControlStateHighlighted];
    [self.btnRoad setImage:[UIImage imageNamed:@"img_road_u.png"] forState:UIControlStateNormal];
    [self.btnRoad setImage:[UIImage imageNamed:@"img_road_d.png"] forState:UIControlStateHighlighted];
    
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)dealloc {
    [_bkImageView release];
    [_btnMode release];
    [_btn_add release];
    [_btnMinus release];
    [_btnSound release];
    [_btnRoad release];
    [super dealloc];
}
- (IBAction)doClose:(id)sender {
    if (_mainVc) {
        [_mainVc closeCtrlView];
    }
}

- (IBAction)doMode:(id)sender {
    [_mainVc SendVirtualKey:MagicDogKeyTypeMode];
}

- (IBAction)doAdd:(id)sender {
    [_mainVc SendVirtualKey:MagicDogKeyTypeUp];
}

- (IBAction)doMinus:(id)sender {
    [_mainVc SendVirtualKey:MagicDogKeyTypeDown];
}

- (IBAction)doSound:(id)sender {
    [_mainVc SendVirtualKey:MagicDogKeyTypeMute];
}

- (IBAction)doRoad:(id)sender {
    [_mainVc SendVirtualKey:MagicDogKeyTypeRoad];
}
@end
