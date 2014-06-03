//
//  RDMapVC.h
//  RedDot
//
//  Created by ShawLiao on 14-5-29.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BMapKit.h"
#import "RDBaseLocationService.h"

@interface RDMapVC : UIViewController <BMKMapViewDelegate,RDLocationServiceDelegate>
{
}
@property (retain, nonatomic) IBOutlet BMKMapView *mapView;
// 路线采集的位置列表
@property (retain,nonatomic) NSMutableArray*    localtions;
// 当前位置
@property (retain,nonatomic) CLLocation*        curLocation;
//
@property (retain,nonatomic) BMKPolyline*       polyLine;
// 定位服务，如果使用外置GPS可以参照这个
@property (retain,nonatomic) RDBaseLocationService*    localtionService;

@end
