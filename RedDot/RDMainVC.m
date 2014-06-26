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
#import "RDInnerLocationService.h"
#import "RDBLEControlVCViewController.h"


@interface RDMainVC ()<DogDelegate>
@property (retain, nonatomic) RDBLEControlVCViewController* bleCtrlVC;
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

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    [Config shareInstance].delegateD    =   self;
    self.curLocation    =   nil;
    // 设置UI细节
    _bCurrentIsMapMode  =   FALSE;
    [self.btnControl setImage:self.btnCtrlImageX forState:UIControlStateDisabled];
    [self.btnControl setImage:self.btnCtrlImageU forState:UIControlStateNormal];
    [self.btnControl setImage:self.btnCtrlImageD forState:UIControlStateHighlighted];
    if (!_bCurrentIsMapMode) {
        [self.btnMap setImage:self.btnMapImageU forState:UIControlStateNormal];
        [self.btnMap setImage:self.btnMapImageD forState:UIControlStateHighlighted];
    }
    else
    {
        [self.btnMap setImage:self.btnCompassImageU forState:UIControlStateNormal];
        [self.btnMap setImage:self.btnCompassImageD forState:UIControlStateHighlighted];
    }
    [self.btnSetting setImage:self.btnSettingImageU forState:UIControlStateNormal];
    [self.btnSetting setImage:self.btnSettingImageD forState:UIControlStateHighlighted];
    
    // 开启蓝牙连接
    _dog    =   [RDMagicDog linkDogWithDelegate:self];
    // 开始定位
    _localtionService   =   [[RDInnerLocationService alloc] init];
    _localtionService.delegate  =   self;
    [_localtionService startLocationService];
    
    [self addChildViewController:self.bleCtrlVC];
    [self.view addSubview:self.bleCtrlVC.view];
    int startPos    =   self.view.frame.size.height;
    CGRect oldRc    =   self.bleCtrlVC.view.frame;
    oldRc.origin.y  =   startPos;
    self.bleCtrlVC.view.frame   =   oldRc;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
    
}

- (void)dealloc
{
    [self.bleCtrlVC removeFromParentViewController];
    [self.bleCtrlVC.view removeFromSuperview];
    
    [_localtionService stopLocationService];
    [_localtionService release];
    [Config shareInstance].delegateD    =   nil;
    [_debugView release];
    [_btnMap release];
    [_btnControl release];
    [_btnSetting release];
    [_mapVC release];
    [_bleCtrlVC release];
    [_btnCtrlImageD release];
    [_btnCtrlImageU release];
    [_btnCtrlImageX release];
    [_btnMapImageD release];
    [_btnMapImageU release];
    [_btnCompassImageD release];
    [_btnCompassImageU release];
    [_btnSettingImageD release];
    [_btnSettingImageU release];
    [_contentView release];
    [_compassView release];
    [_compassImageView release];
    [_speedLabel release];
    [_stausViewSat release];
    [_statusViewCloud release];
    [_statusViewBle release];
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
-(RDBLEControlVCViewController*)bleCtrlVC
{
    if (!_bleCtrlVC) {
        _bleCtrlVC  =   [[RDBLEControlVCViewController   alloc] initWithNibName:@"RDBLEControlVCViewController" bundle:nil];
        _bleCtrlVC.mainVc   =   self;
    }
    return _bleCtrlVC;
}

- (RDMapVC*)mapVC
{
    if (!_mapVC)
    {
        _mapVC  =   [[RDMapVC alloc] initWithNibName:@"RDMapVC" bundle:nil];
    }
    return _mapVC;
}

- (IBAction)doMap:(id)sender
{
    //[self.navigationController pushViewController:self.mapVC animated:TRUE];
    _bCurrentIsMapMode  =   !_bCurrentIsMapMode;
    if (!_bCurrentIsMapMode) {
        [self.btnMap setImage:self.btnMapImageU forState:UIControlStateNormal];
        [self.btnMap setImage:self.btnMapImageD forState:UIControlStateHighlighted];
        self.compassView.hidden =   FALSE;
        [self.mapVC removeFromParentViewController];
        [self.mapVC.view removeFromSuperview];
    }
    else
    {
        [self.btnMap setImage:self.btnCompassImageU forState:UIControlStateNormal];
        [self.btnMap setImage:self.btnCompassImageD forState:UIControlStateHighlighted];
        self.compassView.hidden =   TRUE;
        [self addChildViewController:self.mapVC];
        [self.contentView addSubview:self.mapVC.view];
    }
}


- (IBAction)doSetting:(id)sender {
    //RDNetwork* network  =   [RDNetwork ShareInstance];
    //[network connectToServer];
}

// 打开遥控器窗口
- (IBAction)doDogControl:(id)sender {
    // 添加遥控器窗口
    int startPos    =   self.view.frame.size.height;
    CGRect oldRc    =   self.bleCtrlVC.view.frame;
    oldRc.origin.y  =   startPos;
    CGRect newRc    =   oldRc;
    newRc.origin.y  =   startPos - 350;
    self.bleCtrlVC.view.frame   =   oldRc;
    [UIView animateWithDuration:0.3 animations:^{
        self.bleCtrlVC.view.frame    =  newRc;
    } completion:^(BOOL finished) {
        
    }];
}

// 关闭遥控器窗口
- (void)closeCtrlView
{
    //
    int startPos    =   self.view.frame.size.height;
    CGRect oldRc    =   self.bleCtrlVC.view.frame;
    CGRect newRc    =   oldRc;
    newRc.origin.y  =   startPos;
    self.bleCtrlVC.view.frame   =   oldRc;
    [UIView animateWithDuration:0.3 animations:^{
        self.bleCtrlVC.view.frame    =  newRc;
    } completion:^(BOOL finished) {
        
    }];
}
-(BOOL)SendVirtualKey:(MagicDogKeyType)key
{
    return [_dog SendVirtualKey:key];
}

// 检测蓝牙版本信息
- (void)askDogDetail
{
    __block RDMagicDog* weakDog =   _dog;
    [_dog askUpdateDetail:^(BOOL bSuccess) {
        if (bSuccess) {
            // 成功，打印下
            [[Config shareInstance] PLOG:@"%@",weakDog];
        }
        else
        {
            // 蓝牙连接失败
        }
    }];
}

-(void)startLocate
{
    [_dog openNMEAEN];
}

- (void)askMode
{
    //__block RDMainVC* weakSelf    = self;
    [_dog shakeHand:^(BOOL bSuccess) {
        if (bSuccess) {
            // 准备升级检查
            [self performSelector:@selector(askDogDetail) withObject:nil afterDelay:0.1];
            //[self performSelector:@selector(startLocate) withObject:nil afterDelay:0.1];
        }
        else
        {
            // 获取版本失败，弹出对话框
            
        }
    }];
}

- (void)dog:(RDMagicDog*)dog statusChanged:(DogStatus) status
{
    switch (status) {
        case DogStatusWaitPoweredOn:
            
            break;
        case DogStatusConnected:
        {
            [self performSelector:@selector(askMode) withObject:nil afterDelay:0.3];
            self.btnControl.enabled =   TRUE;
            // 设置图片
            self.statusViewBle.image    =   [UIImage imageNamed:@"icon_status_ble_on.png"];
            
        }
            break;
        case DogStatusDisconnected:
        case DogStatusStatusError:
        {
            self.btnControl.enabled =   FALSE;
            self.statusViewBle.image    =   [UIImage imageNamed:@"icon_status_ble.png"];
        }
            break;
        default:
            break;
    }
}
- (void)dog:(RDMagicDog*)dog didUpdateHeading:(CLHeading *)newHeading
{
    self.curHeading =   newHeading;
}
- (void)dog:(RDMagicDog*)dog didUpdateLocation:(CLLocation *)newLocation
{
    self.curLocation    =   newLocation;
}

#pragma mark - RDLocationServiceDelegate
- (void)serviceWillStartLocating:(id)service
{
    [[Config shareInstance] PLOG:@"%s",__func__];
}
- (void)serviceDidStopLocating:(id)service
{
    [[Config shareInstance] PLOG:@"%s",__func__];
}
- (void)service:(id)service didUpdateHeading:(CLHeading *)newHeading
{
    //[[Config shareInstance] PLOG:@"%s",__func__];
    self.curHeading =   newHeading;
    
}
- (void)service:(id)service didUpdateLocation:(CLLocation *)newLocation
{
    self.curLocation    =   newLocation;
}
- (void)service:(id)service didFailToLocateUserWithError:(NSError *)error
{
    [[Config shareInstance] PLOG:@"%s error:%@",__func__,error];
}
#pragma mark -- Getter
-(void)setCurLocation:(CLLocation *)newLocation
{
    if (_curLocation != newLocation) {
        [_curLocation release];
        _curLocation    =   [newLocation retain];
        if (self.speedLabel) {
            float speedInKm =   0;
            if(_curLocation)
            {
                speedInKm =   _curLocation.speed * 60 / 1000.0;
                if (speedInKm < 0.1) {
                    speedInKm   =   0.0f;
                }
            }
            self.speedLabel.text    =   [NSString stringWithFormat:@"%0.0f",speedInKm];
        }
        if (_bCurrentIsMapMode) {
            [self.mapVC updateLocation:newLocation];
        }
    }
}
-(void)setCurHeading:(CLHeading *)newHeading
{
    if (_curHeading != newHeading)
    {
        [_curHeading release];
        _curHeading =   [newHeading retain];
        if (_curHeading && self.compassImageView) {
            //NSLog(@"heading: %f",_curHeading.trueHeading);
            CGAffineTransform endAngle = CGAffineTransformMakeRotation(-1 * _curHeading.trueHeading * (M_PI / 180.0f));
            [UIView animateWithDuration:0.3 delay:0 options:UIViewAnimationOptionCurveLinear animations:^{
                //self.compassImageView.layer.anchorPoint = CGPointMake(0.5f,0.048245f);//围绕点
                //self.compassImageView.layer.position    = CGPointMake(160, 37+5);//位置</span>
                self.compassImageView.transform         = endAngle;
                
            } completion:^(BOOL finished) {
                
            }];
        }
        if (_bCurrentIsMapMode) {
            [self.mapVC updateHeading:newHeading];
        }
    }
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
        _btnCtrlImageX  =   [[UIImage imageNamed:@"remote_control_x.png"] retain];
    }
    return _btnCtrlImageX;
}
-(UIImage*)btnMapImageD
{
    if (!_btnMapImageD) {
        _btnMapImageD   =   [[UIImage imageNamed:@"switch_d.png"] retain];
    }
    return _btnMapImageD;
}
-(UIImage*)btnMapImageU
{
    if (!_btnMapImageU) {
        _btnMapImageU   =   [[UIImage imageNamed:@"switch_u.png"] retain];
    }
    return _btnMapImageU;
}
-(UIImage*)btnCompassImageD
{
    if (!_btnCompassImageD) {
        _btnCompassImageD   =   [[UIImage imageNamed:@"compass_d.png"] retain];
    }
    return _btnCompassImageD;
}
-(UIImage*)btnCompassImageU
{
    if (!_btnCompassImageU) {
        _btnCompassImageU   =   [[UIImage imageNamed:@"compass_u.png"] retain];
    }
    return _btnCompassImageU;
}
-(UIImage*)btnSettingImageD
{
    if (!_btnSettingImageD) {
        _btnSettingImageD   =   [[UIImage imageNamed:@"setting_d.png"] retain];
    }
    return _btnSettingImageD;
}
-(UIImage*)btnSettingImageU
{
    if (!_btnSettingImageU) {
        _btnSettingImageU   =   [[UIImage imageNamed:@"setting_u.png"] retain];
    }
    return _btnSettingImageU;
}
@end
