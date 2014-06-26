//
//  RDBLEControlVCViewController.h
//  RedDot
//
//  Created by air on 14-6-26.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "RDMainVC.h"

@interface RDBLEControlVCViewController : UIViewController
@property (retain, nonatomic) IBOutlet UIImageView *bkImageView;
@property (retain, nonatomic) IBOutlet UIButton *btnMode;
@property (retain, nonatomic) IBOutlet UIButton *btn_add;
@property (retain, nonatomic) IBOutlet UIButton *btnMinus;
@property (retain, nonatomic) IBOutlet UIButton *btnSound;
@property (retain, nonatomic) IBOutlet UIButton *btnRoad;

@property (assign,nonatomic) RDMainVC*  mainVc;

- (IBAction)doClose:(id)sender;
- (IBAction)doMode:(id)sender;
- (IBAction)doAdd:(id)sender;
- (IBAction)doMinus:(id)sender;
- (IBAction)doSound:(id)sender;
- (IBAction)doRoad:(id)sender;

@end
