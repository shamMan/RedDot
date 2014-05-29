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
    BMKMapView* mapView;//地图视图
}

@end
