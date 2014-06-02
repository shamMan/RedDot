//
//  RDMainVC.m
//  RedDot
//
//  Created by ShawLiao on 14-5-29.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import "RDMainVC.h"

@interface RDMainVC ()
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
    [_btnBlueT release];
    [_btnSocket release];
    [_mapVC release];
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

- (IBAction)doBlueT:(id)sender {
}

- (IBAction)doSocket:(id)sender {
}
@end
