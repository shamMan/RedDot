//
//  RDMapVC.m
//  RedDot
//
//  Created by ShawLiao on 14-5-29.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import "RDMapVC.h"
#import "RDConfig.h"
#import "BMKUserLocationEx.h"
#import "RDLocationTools.h"


@interface RDMapVC ()
// 更新画轨迹
-(void)updateTraceLine;
@end

@implementation RDMapVC

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
    _localtions =   [[NSMutableArray alloc] init];
    CGRect  rc  =   self.view.bounds;
    _mapView    =   [[BMKMapView alloc] initWithFrame:rc];
    
    self.view   =   _mapView;
    //[self.view addSubview:_mapView];
    _mapView.centerCoordinate    =   CLLocationCoordinate2DMake(30.1784,120.1414);
    self.mapView.showsUserLocation = YES;
    self.mapView.userTrackingMode   =   BMKUserTrackingModeFollow;
    
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)dealloc
{
    [_mapView release];
    [_localtions release];
    [_curLocation release];
    [super dealloc];
}

- (void)viewWillAppear:(BOOL)animated
{
    [self.mapView viewWillAppear];
    self.mapView.delegate    =   self;
}
- (void)viewWillDisappear:(BOOL)animated
{
    [self.mapView viewWillDisappear];
    self.mapView.delegate    =   nil;
}

-(void)updateTraceLine
{
    CLLocationCoordinate2D northEastPoint;
    CLLocationCoordinate2D southWestPoint;
    NSUInteger count   =   self.localtions.count;
    CLLocationCoordinate2D* pointArr = (CLLocationCoordinate2D*)malloc(sizeof(CLLocationCoordinate2D)*count);
    for (int i=0; i<count; i++)
    {
        pointArr[i] =   ((CLLocation*)self.localtions[i]).coordinate;
        if (i == 0) {
            northEastPoint = pointArr[i];
            southWestPoint = pointArr[i];
        }
        else
        {
            if (pointArr[i].latitude > northEastPoint.latitude)
                northEastPoint.latitude = pointArr[i].latitude;
            if(pointArr[i].longitude > northEastPoint.longitude)
                northEastPoint.longitude = pointArr[i].longitude;
            if (pointArr[i].latitude < southWestPoint.latitude)
                southWestPoint.latitude = pointArr[i].latitude;
            if(pointArr[i].longitude < southWestPoint.longitude)
                southWestPoint.longitude = pointArr[i].longitude;
        }
    }
    if (self.polyLine) {
        //在地图上移除已有的坐标点
        [self.mapView removeOverlay:self.polyLine];
    }
    self.polyLine = [BMKPolyline polylineWithCoordinates:pointArr count:count] ;
    // 覆盖添加到地图
    if (nil != self.polyLine) {
        [self.mapView addOverlay:self.polyLine];
    }
    // 清楚点的早些时候分配的内存
    free(pointArr);
}

#pragma mark - MapViewDelegate
- (void)mapStatusDidChanged:(BMKMapView *)mapView
{
    //[[Config shareInstance] PLOG:@"%s",__func__];
}
- (BMKOverlayView *)mapView:(BMKMapView *)mapView viewForOverlay:(id <BMKOverlay>)overlay
{
    if ([overlay isKindOfClass:[BMKPolyline class]])
    {
        BMKPolylineView* polylineView = [[[BMKPolylineView alloc] initWithOverlay:overlay] autorelease];
        polylineView.fillColor      = [[UIColor cyanColor] colorWithAlphaComponent:1];
        polylineView.strokeColor    = [[UIColor blueColor] colorWithAlphaComponent:0.7];
        polylineView.lineWidth = 3.0;
        return polylineView;
    }
    return nil;
}

-(void)updateHeading:(CLHeading *)newHeading
{
    self.curHeading =   newHeading;
}
-(void)updateLocation:(CLLocation *)newLocation
{
    //[[Config shareInstance] PLOG:@"%s",__func__];
    if (newLocation)
    {
        BMKUserLocationEx* userLocation   =   [[BMKUserLocationEx  alloc] init];
        // 转换 GPS 到 百度坐标
        CLLocation* bLocation   =   [RDLocationTools CLLocationApplyBaiDuTransform:newLocation];
        [userLocation SetCLLocation:bLocation];
        [userLocation SetCLHeading:self.curHeading];
        CLLocation *location    =   userLocation.location;
        
        [[Config shareInstance] PLOG:@"Location:[%0.4f,%0.4f-%0.4f%]",location.coordinate.latitude,location.coordinate.longitude,location.altitude];
        // check the zero point检查零点
        if (location.coordinate.latitude == 0.0f || location.coordinate.longitude == 0.0f)
            return;
        // check the move distance检查移动的距离
        if (self.localtions.count > 0) {
            CLLocationDistance distance = [location distanceFromLocation:_curLocation];
            [[Config shareInstance] PLOG:@"distance From last:%0.2f",distance];
            if (distance < 5)
            {
                return;
            }
        }
        [self.localtions addObject:location];
        self.curLocation = location;
        [self updateTraceLine];
        CLLocationCoordinate2D coordinate = CLLocationCoordinate2DMake(location.coordinate.latitude, location.coordinate.longitude);
        [self.mapView setCenterCoordinate:coordinate animated:YES];
        [self.mapView updateLocationData:userLocation];
        [userLocation release];
    }
}
@end
