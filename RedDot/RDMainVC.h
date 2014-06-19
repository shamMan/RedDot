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

@interface RDMainVC : UIViewController <DebugProctocal>
@property (retain, nonatomic) IBOutlet UITextView *debugView;
@property (retain, nonatomic) IBOutlet UIButton *btnMap;
@property (retain, nonatomic) IBOutlet UIButton *btnControl;
@property (retain, nonatomic) IBOutlet UIButton *btnSetting;
@property (retain, nonatomic) RDMapVC* mapVC;
- (IBAction)doMap:(id)sender;
- (IBAction)doDogControl:(id)sender;
- (IBAction)doSetting:(id)sender;



@end
