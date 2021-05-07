//
//  TaggedPointExample.m
//  Algorithm
//
//  Created by xiaoyuan on 2021/4/26.
//  Copyright © 2021 xiaoyuan. All rights reserved.
//

#import "TaggedPointExample.h"

/*
 iOS tagged pointer
 在64位机器上，苹果引进了TaggedPointer的概念，用于优化NSNumber、NSDate、NSString 等小对象存储。
 假设要存储一个NSNumber对象，其值是一个整数。正常情况下，如果这个整数只是一个NSInteger的普通变量，在64位CPU下是占8个字节的。1个字节有8位，如果我们存储一个很小的值，会出现很多位都是0的情况，这样就造成了内存浪费，苹果为了解决这个问题，引入了taggedPointer的概念。
 在没有tagged point 之前，NSNumber等小对象需要动态分配内存，来维护引用计数，然后在NSNumber指针中存储堆区NSNumber对象的地址值；而引入这个计数之后，NSNumer指针里面存储的数据是：tag + data，也就是直接将数据存储在指针中。这样做的目的是节省内存。但是当数据特别大时，指针存储不了这个数，那么会恢复之前的存储，存储在堆区，然后指针存储堆区的地址。
 */

// taggedPointer的源码
/*
static inline void * _Nonnull
_objc_makeTaggedPointer(objc_tag_index_t tag, uintptr_t value)
{
    // PAYLOAD_LSHIFT and PAYLOAD_RSHIFT are the payload extraction shifts.
    // They are reversed here for payload insertion.

    // ASSERT(_objc_taggedPointersEnabled());
    if (tag <= OBJC_TAG_Last60BitPayload) {
        // ASSERT(((value << _OBJC_TAG_PAYLOAD_RSHIFT) >> _OBJC_TAG_PAYLOAD_LSHIFT) == value);
        uintptr_t result =
            (_OBJC_TAG_MASK |
             ((uintptr_t)tag << _OBJC_TAG_INDEX_SHIFT) |
             ((value << _OBJC_TAG_PAYLOAD_RSHIFT) >> _OBJC_TAG_PAYLOAD_LSHIFT));
        return _objc_encodeTaggedPointer(result);
    } else {
        // ASSERT(tag >= OBJC_TAG_First52BitPayload);
        // ASSERT(tag <= OBJC_TAG_Last52BitPayload);
        // ASSERT(((value << _OBJC_TAG_EXT_PAYLOAD_RSHIFT) >> _OBJC_TAG_EXT_PAYLOAD_LSHIFT) == value);
        uintptr_t result =
            (_OBJC_TAG_EXT_MASK |
             ((uintptr_t)(tag - OBJC_TAG_First52BitPayload) << _OBJC_TAG_EXT_INDEX_SHIFT) |
             ((value << _OBJC_TAG_EXT_PAYLOAD_RSHIFT) >> _OBJC_TAG_EXT_PAYLOAD_LSHIFT));
        return _objc_encodeTaggedPointer(result);
    }
}
 
 static inline bool
 _objc_isTaggedPointer(const void * _Nullable ptr)
 {
     return ((uintptr_t)ptr & _OBJC_TAG_MASK) == _OBJC_TAG_MASK;
 }
 相关博客
 http://www.cocoachina.com/articles/28657
*/
@interface TaggedPointExample ()

@property (nonatomic, copy) NSString *name;

@end

@implementation TaggedPointExample
- (void)test {

    dispatch_queue_t queue = dispatch_get_global_queue(0, 0);
    for (int i = 0; i < 10000; i++) {
        dispatch_async(queue, ^{
            self.name = [NSString stringWithFormat:@"%@", @"123sdasdasfgsffa"];
            // 会产生崩溃
            // 1.由于多线程通知执行setName:方法
            // 2.name的新值是一个非taggedPointer的oc对象，而setName:方法中会执行旧值[_name release]
            // 3.Autorelease在runloop即将进入休眠前，会对自动释放池中的对象发送release操作
            // 4.当执行release时，会查询或者修改对象isa指针中的bits，发生多线程资源竞争
            if (i == 0) {
                NSLog(@"%@__index:%d", self.name.class, i);//__NSCFString
            }
        });
    }
}

- (void)test0 {
    
    dispatch_queue_t queue = dispatch_get_global_queue(0, 0);
    for (int i = 0; i < 10000; i++) {
        dispatch_async(queue, ^{
            self.name = [NSString stringWithFormat:@"%@", @"123"];
            // 通过新值可知，name是一个taggedPointer的小对象，在setName:中虽然也对旧值进行release，但是在release的实现中，如果对象是taggedPointer则不进行操作
            if (i == 0) {
                NSLog(@"%@", self.name.class);// NSTaggedPointerString
            }
        });
    }
}

// set 方法的实现
//-(void)setName:(NSString *)name{
//    if (_name != name) {
//        [_name release];
//        _name = [name copy];
//    }
//}

@end
