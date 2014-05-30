//
//  RDAppDelegate.h
//  RedDot
//
//  Created by ShawLiao on 14-5-29.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BMapKit.h"

@interface RDAppDelegate : UIResponder <UIApplicationDelegate,BMKGeneralDelegate>
{
@private BMKMapManager* _mapManager;
}

@property (strong, nonatomic) UIWindow *window;

@end
