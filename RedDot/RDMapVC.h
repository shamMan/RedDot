//
//  RDMapVC.h
//  RedDot
//
//  Created by ShawLiao on 14-5-29.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BMapKit.h"

@interface RDMapVC : UIViewController <BMKMapViewDelegate>
{
}
@property (retain, nonatomic) IBOutlet BMKMapView *mapView;
// 路线采集的位置列表
@property (retain,nonatomic) NSMutableArray*    localtions;
// 当前位置
@property (retain,nonatomic) CLLocation*        curLocation;
// 当前位置
@property (retain,nonatomic) CLHeading*         curHeading;
//
@property (retain,nonatomic) BMKPolyline*       polyLine;


-(void)updateHeading:(CLHeading *)newHeading;
-(void)updateLocation:(CLLocation *)newLocation;

@end
