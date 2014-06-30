//
//  RDOffLineMapVC.h
//  RedDot
//
//  Created by air on 14-6-29.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BMapKit.h"

@interface RDOffLineMapVC : UIViewController
{
    BMKOfflineMap* _offlineMap;
    NSArray* _arrayHotCityData;                     //热门城市
    NSArray* _arrayOfflineCityData;                 //全国支持离线地图的城市
    //NSMutableArray * _arrayDownLoadingMapInfo;      //正在下载的离线地图
    //NSMutableArray * _arraylocalDownLoadMapInfo;    //本地下载的离线地图
}
@property (retain, nonatomic) IBOutlet UISegmentedControl *segment;
@property (retain, nonatomic) IBOutlet UITableView *tableView1;
@property (retain, nonatomic) IBOutlet UITableView *tableView2;
@property (retain, nonatomic) IBOutlet UITableView *tableView3;

@property (retain, nonatomic) NSMutableArray*   arrayDownLoadingMapInfo;
@property (retain, nonatomic) NSMutableArray*   arraylocalDownLoadMapInfo;

- (IBAction)segmentChanged:(id)sender;

@end
