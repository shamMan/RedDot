//
//  RDOffLineMapVC.m
//  RedDot
//
//  Created by air on 14-6-29.
//  Copyright (c) 2014年 ShawLiao. All rights reserved.
//

#import "RDOffLineMapVC.h"

@interface RDOffLineMapVC ()<BMKOfflineMapDelegate>

@end

@implementation RDOffLineMapVC

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
    //适配ios7
    if( ([[[UIDevice currentDevice] systemVersion] doubleValue]>=7.0))
    {
        //        self.edgesForExtendedLayout=UIRectEdgeNone;
        self.navigationController.navigationBar.translucent = NO;
    }
    //初始化离线地图服务
    _offlineMap = [[BMKOfflineMap alloc]init];
    //获取热门城市
    _arrayHotCityData = [[_offlineMap getHotCityList]retain];
    //获取支持离线下载城市列表
    _arrayOfflineCityData = [[_offlineMap getOfflineCityList]retain];
    //初始化Segment
    self.segment.selectedSegmentIndex = 0;
}

-(void)viewWillAppear:(BOOL)animated {
    _offlineMap.delegate = self; // 此处记得不用的时候需要置nil，否则影响内存的释放
}

-(void)viewWillDisappear:(BOOL)animated {
    _offlineMap.delegate = nil; // 不用时，置nil
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)dealloc {
    [_segment release];
    [_tableView1 release];
    [_tableView2 release];
    [_tableView3 release];
    [_arrayHotCityData release];
    [_arrayOfflineCityData release];
    [_arraylocalDownLoadMapInfo release];
    if (_offlineMap != nil) {
        [_offlineMap release];
        _offlineMap = nil;
    }
    [super dealloc];
}
- (IBAction)segmentChanged:(id)sender {
    UISegmentedControl* control = (UISegmentedControl*)sender;
    switch (control.selectedSegmentIndex) {
        case 0:
        {
            self.tableView1.hidden  = NO;
            self.tableView2.hidden  = YES;
            self.tableView3.hidden  = YES;
            [self.tableView1 reloadData];
        }
            break;
        case 1:
        {
            self.tableView1.hidden  = YES;
            self.tableView2.hidden  = NO;
            self.tableView3.hidden  = YES;
            //获取各城市离线地图更新信息
            //_arraylocalDownLoadMapInfo = [[NSMutableArray arrayWithArray:[_offlineMap getAllUpdateInfo]] retain];
            [self.tableView2 reloadData];
        }
            break;
        case 2:
        {
            self.tableView1.hidden  = YES;
            self.tableView2.hidden  = YES;
            self.tableView3.hidden  = NO;
            //获取各城市离线地图更新信息
            _arraylocalDownLoadMapInfo = [[NSMutableArray arrayWithArray:[_offlineMap getAllUpdateInfo]] retain];
            [self.tableView3 reloadData];
        }
            break;
        default:
            break;
    }
}
//离线地图delegate，用于获取通知
- (void)onGetOfflineMapState:(int)type withState:(int)state
{
    
    if (type == TYPE_OFFLINE_UPDATE) {
        //id为state的城市正在下载或更新，start后会毁掉此类型
        BMKOLUpdateElement* updateInfo;
        updateInfo = [_offlineMap getUpdateInfo:state];
        NSLog(@"城市名：%@,下载比例:%d",updateInfo.cityName,updateInfo.ratio);
    }
    if (type == TYPE_OFFLINE_NEWVER) {
        //id为state的state城市有新版本,可调用update接口进行更新
        BMKOLUpdateElement* updateInfo;
        updateInfo = [_offlineMap getUpdateInfo:state];
        NSLog(@"是否有更新%d",updateInfo.update);
    }
    if (type == TYPE_OFFLINE_UNZIP) {
        //正在解压第state个离线包，导入时会回调此类型
    }
    if (type == TYPE_OFFLINE_ZIPCNT) {
        //检测到state个离线包，开始导入时会回调此类型
        NSLog(@"检测到%d个离线包",state);
        if(state==0)
        {
            [self showImportMesg:state];
        }
    }
    if (type == TYPE_OFFLINE_ERRZIP) {
        //有state个错误包，导入完成后会回调此类型
        NSLog(@"有%d个离线包导入错误",state);
    }
    if (type == TYPE_OFFLINE_UNZIPFINISH) {
        NSLog(@"成功导入%d个离线包",state);
        //导入成功state个离线包，导入成功后会回调此类型
        [self showImportMesg:state];
    }
    
}
//导入提示框
- (void)showImportMesg:(int)count
{
    NSString* showmeg = [NSString stringWithFormat:@"成功导入离线地图包个数:%d", count];
    UIAlertView *myAlertView = [[UIAlertView alloc] initWithTitle:@"导入离线地图" message:showmeg delegate:self cancelButtonTitle:nil otherButtonTitles:@"确定",nil];
    [myAlertView show];
    [myAlertView release];
}

#pragma mark UITableView delegate
//定义表中有几个section
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    if(tableView== self.tableView1)
    {
        return 2;
    }
    else
    {
        return 1;
    }
    
}
//定义section标题
- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
    if(tableView == self.tableView1)
    {
        //定义每个section的title
        NSString *provincName=@"";
        if(section==0)
        {
            provincName=@"热门城市";
        }
        else if(section==1)
        {
            provincName=@"全国";
        }
        return provincName;
    }
    return nil;
}
//定义每个section中有几行
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    if(tableView == self.tableView1)
    {
        if(section==0)
        {
            return [_arrayHotCityData count];
        }
        else if(section==1)
        {
            return [_arrayOfflineCityData count];
        }
        else
        {
            return 0;
        }
    }
    else if(tableView == self.tableView2)
    {
        return 0;
    }
    else
    {
        return [_arraylocalDownLoadMapInfo count];
    }
}
//定义cell样式填充数据
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    static NSString *CellIdentifier = @"OfflineMapCityCell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
        
    }
    if(tableView == self.tableView1)
    {
        //热门城市列表
        if(indexPath.section==0)
        {
            BMKOLSearchRecord* item = [_arrayHotCityData objectAtIndex:indexPath.row];
            cell.textLabel.text = [NSString stringWithFormat:@"%@(%d)", item.cityName, item.cityID];
            //转换包大小
            NSString*packSize = [self getDataSizeString:item.size];
            UILabel *sizelabel =[[[UILabel alloc] initWithFrame:CGRectMake(250, 0, 60, 40)]autorelease];
            sizelabel.autoresizingMask =UIViewAutoresizingFlexibleLeftMargin;
            sizelabel.text = packSize;
            sizelabel.backgroundColor = [UIColor clearColor];
            cell.accessoryView = sizelabel;
            
        }
        //支持离线下载城市列表
        else if(indexPath.section==1)
        {
            BMKOLSearchRecord* item = [_arrayOfflineCityData objectAtIndex:indexPath.row];
            cell.textLabel.text = [NSString stringWithFormat:@"%@(%d)", item.cityName, item.cityID];
            //转换包大小
            NSString*packSize = [self getDataSizeString:item.size];
            UILabel *sizelabel =[[[UILabel alloc] initWithFrame:CGRectMake(250, 0, 60, 40)]autorelease];
            sizelabel.autoresizingMask =UIViewAutoresizingFlexibleLeftMargin;
            sizelabel.text = packSize;
            sizelabel.backgroundColor = [UIColor clearColor];
            cell.accessoryView = sizelabel;
            
        }
    }
    else if(tableView == self.tableView2)
    {
        
    }
    else
    {
        if(_arraylocalDownLoadMapInfo!=nil&&_arraylocalDownLoadMapInfo.count>indexPath.row)
        {
            BMKOLUpdateElement* item = [_arraylocalDownLoadMapInfo objectAtIndex:indexPath.row];
            //是否可更新
            if(item.update)
            {
                cell.textLabel.text = [NSString stringWithFormat:@"%@————%d(可更新)", item.cityName,item.ratio];
            }
            else
            {
                cell.textLabel.text = [NSString stringWithFormat:@"%@————%d", item.cityName,item.ratio];
            }
        }
        else
        {
            cell.textLabel.text = @"";
        }
    }
    
    return cell;
}

//是否允许table进行编辑操作
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath{
    
    if(tableView == self.tableView3)
    {
        return YES;
    }
    else
    {
        return NO;
    }
}

//提交编辑列表的结果
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        //删除poi
        if (tableView == self.tableView3) {
            BMKOLUpdateElement* item = [_arraylocalDownLoadMapInfo objectAtIndex:indexPath.row];
            //删除指定城市id的离线地图
            [_offlineMap remove:item.cityID];
            //将此城市的离线地图信息从数组中删除
            [(NSMutableArray*)_arraylocalDownLoadMapInfo removeObjectAtIndex:indexPath.row];
            [self.tableView3 reloadData];
            
        }
        
    }
    
}
//表的行选择操作
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [tableView deselectRowAtIndexPath:indexPath animated:NO];
    if(tableView == self.tableView3)
    {
//        BMKOLUpdateElement* item = [_arraylocalDownLoadMapInfo objectAtIndex:indexPath.row];
//        if(item.ratio==100 && item.update)//跳转到地图查看页面进行地图更新
//            //        if(item.ratio==100)
//        {
//            //跳转到地图浏览页面
//            OfflineDemoMapViewController *offlineMapViewCtrl = [[[OfflineDemoMapViewController alloc] init] autorelease];
//            offlineMapViewCtrl.title = @"查看离线地图";
//            offlineMapViewCtrl.cityId = item.cityID;
//            offlineMapViewCtrl.offlineServiceOfMapview = _offlineMap;
//            UIBarButtonItem *customLeftBarButtonItem = [[UIBarButtonItem alloc] init];
//            customLeftBarButtonItem.title = @"返回";
//            self.navigationItem.backBarButtonItem = customLeftBarButtonItem;
//            [customLeftBarButtonItem release];
//            [self.navigationController pushViewController:offlineMapViewCtrl animated:YES];
//            
//        }
//        else if(item.ratio<100)//弹出提示框
//        {
//            cityId.text = [NSString stringWithFormat:@"%d", item.cityID];
//            cityName.text = item.cityName;
//            downloadratio.text = [NSString stringWithFormat:@"已下载:%d", item.ratio];
//            UIAlertView *myAlertView = [[UIAlertView alloc] initWithTitle:@"提示" message:@"该离线地图未完全下载，请继续下载！" delegate:self cancelButtonTitle:nil otherButtonTitles:@"确定",nil];
//            [myAlertView show];
//            [myAlertView release];
//        }
        
    }
    else
    {
        //获得当前选中的城市信息
        if(indexPath.section==0)
        {
            BMKOLSearchRecord* item = [_arrayHotCityData objectAtIndex:indexPath.row];
            //显示子cell
            if(item.childCities != nil && item.childCities.count > 0)
            {
                for (BMKOLSearchRecord* childitem in item.childCities)
                {
                    NSString* tempStri = [NSString stringWithFormat:@"%@(%d)", childitem.cityName, childitem.cityID];
                    //转换包大小
                    NSString* tempPackSize = [self getDataSizeString:childitem.size];
                    NSLog(@"热门城市：%@--包大小：%@",tempStri,tempPackSize);
                }
            }
        }
        else if(indexPath.section==1)
        {
            BMKOLSearchRecord* item = [_arrayOfflineCityData objectAtIndex:indexPath.row];
            //显示子cell
            if(item.childCities != nil && item.childCities.count > 0)
            {
                for (BMKOLSearchRecord* childitem in item.childCities)
                {
                    NSString* tempStri = [NSString stringWithFormat:@"%@(%d)", childitem.cityName, childitem.cityID];
                    //转换包大小
                    NSString* tempPackSize = [self getDataSizeString:childitem.size];
                    NSLog(@"支持离线包城市：%@--包大小：%@",tempStri,tempPackSize);
                }
            }
        }
        
    }
    
}

#pragma mark 包大小转换工具类（将包大小转换成合适单位）
-(NSString *)getDataSizeString:(int) nSize
{
	NSString *string = nil;
	if (nSize<1024)
	{
		string = [NSString stringWithFormat:@"%dB", nSize];
	}
	else if (nSize<1048576)
	{
		string = [NSString stringWithFormat:@"%dK", (nSize/1024)];
	}
	else if (nSize<1073741824)
	{
		if ((nSize%1048576)== 0 )
        {
			string = [NSString stringWithFormat:@"%dM", nSize/1048576];
        }
		else
        {
            int decimal = 0; //小数
            NSString* decimalStr = nil;
            decimal = (nSize%1048576);
            decimal /= 1024;
            
            if (decimal < 10)
            {
                decimalStr = [NSString stringWithFormat:@"%d", 0];
            }
            else if (decimal >= 10 && decimal < 100)
            {
                int i = decimal / 10;
                if (i >= 5)
                {
                    decimalStr = [NSString stringWithFormat:@"%d", 1];
                }
                else
                {
                    decimalStr = [NSString stringWithFormat:@"%d", 0];
                }
                
            }
            else if (decimal >= 100 && decimal < 1024)
            {
                int i = decimal / 100;
                if (i >= 5)
                {
                    decimal = i + 1;
                    
                    if (decimal >= 10)
                    {
                        decimal = 9;
                    }
                    
                    decimalStr = [NSString stringWithFormat:@"%d", decimal];
                }
                else
                {
                    decimalStr = [NSString stringWithFormat:@"%d", i];
                }
            }
            
            if (decimalStr == nil || [decimalStr isEqualToString:@""])
            {
                string = [NSString stringWithFormat:@"%dMss", nSize/1048576];
            }
            else
            {
                string = [NSString stringWithFormat:@"%d.%@M", nSize/1048576, decimalStr];
            }
        }
	}
	else	// >1G
	{
		string = [NSString stringWithFormat:@"%dG", nSize/1073741824];
	}
	return string;
}

@end
