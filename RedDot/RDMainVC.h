//
//  RDMainVC.h
//  RedDot
//
//  Created by ShawLiao on 14-5-29.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "RDConfig.h"
#import "RDMapVC.h"
#import "RDMagicDog.h"
#import "RDBaseLocationService.h"

@interface RDMainVC : UIViewController <DebugProctocal,RDLocationServiceDelegate>
{
    BOOL        _bCurrentIsMapMode;
    UIImage*    _btnCtrlImageD;
    UIImage*    _btnCtrlImageU;
    UIImage*    _btnCtrlImageX;
    UIImage*    _btnMapImageD;
    UIImage*    _btnMapImageU;
    UIImage*    _btnCompassImageD;
    UIImage*    _btnCompassImageU;
    UIImage*    _btnSettingImageD;
    UIImage*    _btnSettingImageU;
    
    RDMagicDog* _dog;
}
@property (retain, nonatomic) IBOutlet UITextView *debugView;
@property (retain, nonatomic) IBOutlet UIButton *btnMap;
@property (retain, nonatomic) IBOutlet UIButton *btnControl;
@property (retain, nonatomic) IBOutlet UIButton *btnSetting;

@property (retain, nonatomic) IBOutlet UIView *contentView;
@property (retain, nonatomic) IBOutlet UIView *compassView;
@property (retain, nonatomic) IBOutlet UIImageView *compassImageView;
@property (retain, nonatomic) IBOutlet UILabel *speedLabel;


@property (readonly,nonatomic) UIImage*    btnCtrlImageD;
@property (readonly,nonatomic) UIImage*    btnCtrlImageU;
@property (readonly,nonatomic) UIImage*    btnCtrlImageX;
@property (readonly,nonatomic) UIImage*    btnMapImageD;
@property (readonly,nonatomic) UIImage*    btnMaplImageU;
@property (readonly,nonatomic) UIImage*    btnCompasslImageD;
@property (readonly,nonatomic) UIImage*    btnCompassImageU;
@property (readonly,nonatomic) UIImage*    btnSettingImageD;
@property (readonly,nonatomic) UIImage*    btnSettingImageU;
// 定位服务，如果使用外置GPS可以参照这个
@property (retain,nonatomic) RDBaseLocationService*    localtionService;
// 当前位置
@property (retain,nonatomic) CLLocation*        curLocation;
// 当前位置
@property (retain,nonatomic) CLHeading*         curHeading;



@property (retain, nonatomic) RDMapVC* mapVC;
- (IBAction)doMap:(id)sender;
- (IBAction)doDogControl:(id)sender;
- (IBAction)doSetting:(id)sender;



@end
