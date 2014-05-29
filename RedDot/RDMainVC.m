//
//  RDMainVC.m
//  RedDot
//
//  Created by ShawLiao on 14-5-29.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import "RDMainVC.h"
#import "RDMapVC.h"

@interface RDMainVC ()
@end

@implementation RDMainVC
- (void)AppendLog:(NSString *)log
{
    NSString* newtext   =   [NSString stringWithFormat:@"%@\n%@",self.debugView.text,log];
    self.debugView.text =   newtext;
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

- (IBAction)doMap:(id)sender {
    RDMapVC* mapview    =   [[[RDMapVC alloc] initWithNibName:@"RDMapVC" bundle:nil] autorelease];
    [self.navigationController pushViewController:mapview animated:TRUE];
}

- (IBAction)doBlueT:(id)sender {
}

- (IBAction)doSocket:(id)sender {
}
@end
