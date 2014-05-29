//
//  RDMainVC.h
//  RedDot
//
//  Created by ShawLiao on 14-5-29.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "RDConfig.h"

@interface RDMainVC : UIViewController <DebugProctocal>
@property (retain, nonatomic) IBOutlet UITextView *debugView;
@property (retain, nonatomic) IBOutlet UIButton *btnMap;
@property (retain, nonatomic) IBOutlet UIButton *btnBlueT;
@property (retain, nonatomic) IBOutlet UIButton *btnSocket;
- (IBAction)doMap:(id)sender;
- (IBAction)doBlueT:(id)sender;
- (IBAction)doSocket:(id)sender;

@end
