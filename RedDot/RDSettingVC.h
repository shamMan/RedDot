//
//  RDSettingVC.h
//  RedDot
//
//  Created by air on 14-6-28.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface RDSettingVC : UIViewController<UITableViewDataSource,UITableViewDelegate>
@property (retain, nonatomic) IBOutlet UITableView *tableView;

@end
