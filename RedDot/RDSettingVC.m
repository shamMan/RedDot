//
//  RDSettingVC.m
//  RedDot
//
//  Created by air on 14-6-28.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import "RDSettingVC.h"
#import "RDConfig.h"

@interface RDSettingVC ()
- (void)updateSwitchAtIndexPath:(id)sender;
@end

@implementation RDSettingVC

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
    self.title  =   @"设置";
    self.tableView.delegate     =   self;
    self.tableView.dataSource   =   self;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)dealloc {
    [_tableView release];
    [super dealloc];
}
- (void)viewWillAppear:(BOOL)animated
{
    [self.navigationController setNavigationBarHidden:FALSE animated:TRUE];
}
- (void)viewWillDisappear:(BOOL)animated
{
    [self.navigationController setNavigationBarHidden:TRUE animated:TRUE];
}
#pragma mark -- UITableViewDelegate
- (void)deselect:(id)sender
{
    [self.tableView deselectRowAtIndexPath:[self.tableView indexPathForSelectedRow] animated:YES];
}
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [self performSelector:@selector(deselect:) withObject:nil afterDelay:0.4];
    int sec =   indexPath.section;
    int row =   indexPath.row;
    if (sec == 1) {
        if (row == 0) {
            
        }
    }
    if (sec == 2) {
        switch (row) {
            case 0:
                // 恢复默认
                [[Config shareInstance] resetSettingData];
                [tableView reloadData];
                break;
            case 1:
                // 相关信息
                break;
            case 2:
                // 软件版本信息
                break;
            default:
                break;
        }
    }
    
}
- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    int sec =   indexPath.section;
    if (sec == 0) {
        return 50;
    }
    return 40;
}
#pragma mark -- UITableViewDataSource

- (void)updateSwitchAtIndexPath:(id)sender
{
    // 获得对应的switch并判断编号
    UISwitch *switchView = (UISwitch *)sender;
    int row = switchView.tag -1000;
    bool isOn = [switchView isOn];
    // 获得配置数据
    Config* config  =   [Config shareInstance];
    switch (row)
    {
        case 0:
            config.allowUpdateOn3G      =   isOn;
            break;
        case 1:
            config.autoConnect          =   isOn;
            break;
        case 2:
            config.autoUpdate           =   isOn;
            break;
        case 3:
            config.canReportInBackground=   isOn;
            break;
        default:
            break;
    }
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    int ret[]   =   {3,1,3};
    return ret[section];
}
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    int sec =   indexPath.section;
    int row =   indexPath.row;
    static NSString *setNoCellIdentifier = @"setNocCell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:setNoCellIdentifier];
    if (!cell)
    {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:setNoCellIdentifier] autorelease];
        cell.textLabel.numberOfLines = 0;
        cell.textLabel.lineBreakMode   =   NSLineBreakByWordWrapping;
        [cell.textLabel adjustsFontSizeToFitWidth];
	}
    switch (sec) {
        case 0:
        {
            // 创建UIswitch并设置编号
            cell.selectionStyle =   UITableViewCellSelectionStyleNone;
            UISwitch *switchview = [[[UISwitch alloc] initWithFrame:CGRectZero] autorelease];
            switchview.tag      =   1000 + row;
            [switchview addTarget:self action:@selector(updateSwitchAtIndexPath:) forControlEvents:UIControlEventValueChanged];
            cell.accessoryView  =   switchview;
            NSArray* labels     =   @[@"允许使用3G流量进行更新",@"自动蓝牙连接电子狗",@"自动升级电子狗固件和数据"];
            cell.textLabel.text =   labels[row];
            Config* cfg  =   [Config shareInstance];
            BOOL isOn   =   TRUE;
            switch (row) {
                case 0:
                    isOn    =   cfg.allowUpdateOn3G;
                    break;
                case 1:
                    isOn    =   cfg.autoConnect;
                    break;
                case 2:
                    isOn    =   cfg.autoUpdate;
                    break;
                default:
                    break;
            }
            switchview.on =   isOn;
        }
            break;
        case 1:
        {
            cell.selectionStyle =   UITableViewCellSelectionStyleNone;
            UISwitch *switchview = [[[UISwitch alloc] initWithFrame:CGRectZero] autorelease];
            switchview.tag      =   1003;
            [switchview addTarget:self action:@selector(updateSwitchAtIndexPath:) forControlEvents:UIControlEventValueChanged];
            cell.accessoryView  =   switchview;
            cell.textLabel.text =   @"是否后台播报";
            switchview.on   =   [Config shareInstance].canReportInBackground;
        }
            break;
        case 2:
        {
            cell.selectionStyle =   UITableViewCellSelectionStyleBlue;
            switch (row) {
                case 0:
                    cell.textLabel.text =   @"恢复默认";
                    cell.accessoryView  =   nil;
                    cell.accessoryType  =   UITableViewCellAccessoryNone;
                    break;
                case 1:
                    cell.textLabel.text =   @"电子狗设备相关信息";
                    cell.accessoryView  =   nil;
                    cell.accessoryType  =   UITableViewCellAccessoryDisclosureIndicator;
                    break;
                case 2:
                    cell.textLabel.text =   @"软件版本信息";
                    cell.accessoryView  =   nil;
                    cell.accessoryType  =   UITableViewCellAccessoryDisclosureIndicator;
                    break;
                default:
                    break;
            }
            
        }
            break;
        default:
            break;
    }
    return cell;
}
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 3;
}
@end
