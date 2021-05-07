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
#import "TaggedPointExample.h"

static BOOL isRunLoopStop = NO;

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
        
        [[TaggedPointExample alloc] test];
        
        //获取当前线程的runloop，并且给线程添加一个NSPort（source）
        [[NSRunLoop currentRunLoop] addPort:[[NSPort alloc] init] forMode:NSDefaultRunLoopMode];
        
        // 使用弱引用，保证isStop变量在设置为YES时，能退出循环
        while (isRunLoopStop) {
            // 当程序运行到这里时，如果没有需要执行的任务，则线程会进入休眠状态，并且不会继续执行while循环
            // 直到有任务要执行时，才会被唤醒
            [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
        }
        
    }
    return 0;
}
