//
//  RDMainVC.m
//  RedDot
//
//  Created by ShawLiao on 14-5-29.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import "RDMainVC.h"
#import "RDNetwork.h"
#import "RDBlueToothDeviceManager.h"
#import "RDMagicDog.h"

@interface RDMainVC ()<DogDelegate>
@end

@implementation RDMainVC
- (void)AppendLog:(NSString *)log
{
    NSString* newtext   =   [NSString stringWithFormat:@"%@\n%@",self.debugView.text,log];
    self.debugView.text =   newtext;
    NSLog(@"%@",log);
}
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

-(UIImage*)btnCtrlImageD
{
    if (!_btnCtrlImageD) {
        _btnCtrlImageD  =   [[UIImage imageNamed:@"remote_control_d.png"] retain];
    }
    return _btnCtrlImageD;
}
-(UIImage*)btnCtrlImageU
{
    if (!_btnCtrlImageU) {
        _btnCtrlImageU  =   [[UIImage imageNamed:@"remote_control_d.png"] retain];
    }
    return _btnCtrlImageU;
}
-(UIImage*)btnCtrlImageX
{
    if (!_btnCtrlImageX) {
        _btnCtrlImageX  =   [[UIImage imageNamed:@"remote_control_d.png"] retain];
    }
    return _btnCtrlImageX;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    [Config shareInstance].delegateD    =   self;
    _bControlPushed =   FALSE;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
    
}

- (void)dealloc
{
    [Config shareInstance].delegateD    =   nil;
    [_debugView release];
    [_btnMap release];
    [_btnControl release];
    [_btnSetting release];
    [_mapVC release];
    [_btnCtrlImageD release];
    [_btnCtrlImageU release];
    [_btnCtrlImageX release];
    [super dealloc];
}

- (void)viewWillAppear:(BOOL)animated
{
    self.navigationController.navigationBarHidden =   TRUE;
}

- (void)viewWillDisappear:(BOOL)animated
{
    self.navigationController.navigationBarHidden =   FALSE;
}

#pragma mark - Getter & Setter
- (RDMapVC*)mapVC
{
    if (!_mapVC)
    {
        _mapVC  =   [[RDMapVC alloc] initWithNibName:@"RDMapVC" bundle:nil];
    }
    return _mapVC;
}

- (IBAction)doMap:(id)sender {
    [self.navigationController pushViewController:self.mapVC animated:TRUE];
}

- (IBAction)doDogControl:(id)sender {
    _bControlPushed =   !_bControlPushed;
    UIImage* imge   =   _bControlPushed?self.btnCtrlImageD:self.btnCtrlImageU ;
    [self.btnControl setImage:imge forState:UIControlStateNormal];
    
}

- (IBAction)doSetting:(id)sender {
    RDNetwork* network  =   [RDNetwork ShareInstance];
    [network connectToServer];
}

- (void)dog:(RDMagicDog*)dog statusChanged:(DogStatus) status
{
    switch (status) {
        case DogStatusWaitPoweredOn:
            
            break;
        case DogStatusConnected:
        {
            self.btnControl.enabled =   TRUE;
            [self.btnControl setImage:[UIImage imageNamed:@"remote_control_u.png"] forState:UIControlStateNormal];
        }
            break;
        case DogStatusDisconnected:
        case DogStatusStatusError:
        {
            self.btnControl.enabled =   FALSE;
            [self.btnControl setImage:[UIImage imageNamed:@"remote_control_x.png"] forState:UIControlStateNormal];
        }
            break;
        default:
            break;
    }
}
- (void)dog:(RDMagicDog*)dog didUpdateHeading:(CLHeading *)newHeading
{
}
- (void)dog:(RDMagicDog*)dog didUpdateLocation:(CLLocation *)newLocation
{
}
@end
