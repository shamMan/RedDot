//
//  RDMapVC.m
//  RedDot
//
//  Created by ShawLiao on 14-5-29.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import "RDMapVC.h"
#import "RDConfig.h"


BOOL isRetina = FALSE;

@interface RouteAnnotation : BMKPointAnnotation

{
    int _type; ///<0:起点 1：终点 2：公交 3：地铁 4:驾乘
    int _degree;
    
}

@property (nonatomic) int type;

@property (nonatomic) int degree;

@end

@implementation RouteAnnotation

@synthesize type = _type;

@synthesize degree = _degree;

@end

@interface UIImage(InternalMethod)

- (UIImage*)imageRotatedByDegrees:(CGFloat)degrees;

@end

@implementation UIImage(InternalMethod)

- (UIImage*)imageRotatedByDegrees:(CGFloat)degrees

{
    
    CGSize rotatedSize = self.size;
    
    if (isRetina) {
        
        rotatedSize.width *= 2;
        
        rotatedSize.height *= 2;
        
    }
    
    UIGraphicsBeginImageContext(rotatedSize);
    
    CGContextRef bitmap = UIGraphicsGetCurrentContext();
    
    CGContextTranslateCTM(bitmap, rotatedSize.width/2, rotatedSize.height/2);
    
    CGContextRotateCTM(bitmap, degrees * M_PI / 180);
    
    CGContextRotateCTM(bitmap, M_PI);
    
    CGContextScaleCTM(bitmap, -1.0, 1.0);
    
    CGContextDrawImage(bitmap, CGRectMake(-rotatedSize.width/2, -rotatedSize.height/2, rotatedSize.width, rotatedSize.height), self.CGImage);
    
    UIImage* newImage = UIGraphicsGetImageFromCurrentImageContext();
    
    UIGraphicsEndImageContext();
    
    return newImage;
    
}

@end

@interface RDMapVC ()

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
    _mapView    =   [[BMKMapView alloc] initWithFrame:CGRectMake(0, 0, 320, 400)];
    self.view   =   _mapView;
    //[self.view addSubview:_mapView];
    _mapView.centerCoordinate    =   CLLocationCoordinate2DMake(120.2,30.3);
    //[self.mapView setShowsUserLocation:YES];//显示定位的蓝点儿
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)dealloc
{
    [super dealloc];
    //[_mapView release];
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

- (NSString*)getMyBundlePath1:(NSString *)filename
{
    NSBundle * libBundle = MYBUNDLE ;
    if ( libBundle && filename ){
        
        NSString * s=[[libBundle resourcePath ] stringByAppendingPathComponent : filename];
        NSLog (@"%@",s);
        return s;
    }
    return nil ;
}


@end
