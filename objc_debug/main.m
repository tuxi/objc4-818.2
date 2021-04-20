//
//  main.m
//  objc_debug
//
//  Created by objc on 2021/1/5.


// 调试项目配置
// 1: enable hardened runtime -> NO
// 1: build phase -> denpendenice -> objc


#import <Foundation/Foundation.h>
#import "objc_debug-Swift.h"

@interface Person : NSObject

@end

@implementation Person

@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // insert code here...
        
        Person *p = [Person new];
        __weak NSObject *obj = p;
        NSLog(@"%@", obj);
        
        id __autoreleasing obj1 = [NSObject new];
        NSLog(@"%@", obj1);
        
        SwiftObj *swiftObj = [SwiftObj new];
        [swiftObj run];
        NSLog(@"");
    }
    return 0;
}
