/*
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Copyright (c) 2018 Apple Inc.  All Rights Reserved.
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/********************************************************************
 * 
 *  isa.h - Definitions of isa fields for C and assembly code.
 *
 ********************************************************************/

#ifndef _OBJC_ISA_H_
#define _OBJC_ISA_H_

#include "objc-config.h"


#if (!SUPPORT_NONPOINTER_ISA && !SUPPORT_PACKED_ISA && !SUPPORT_INDEXED_ISA) ||\
    ( SUPPORT_NONPOINTER_ISA &&  SUPPORT_PACKED_ISA && !SUPPORT_INDEXED_ISA) ||\
    ( SUPPORT_NONPOINTER_ISA && !SUPPORT_PACKED_ISA &&  SUPPORT_INDEXED_ISA)
    // good config
#else
#   error bad config
#endif

#if SUPPORT_PACKED_ISA

    // extra_rc must be the MSB-most field (so it matches carry/overflow flags)
    // nonpointer must be the LSB (fixme or get rid of it)
    // shiftcls must occupy the same bits that a real class pointer would
    // bits + RC_ONE is equivalent to extra_rc + 1
    // RC_HALF is the high bit of extra_rc (i.e. half of its range)

    // future expansion:
    // uintptr_t fast_rr : 1;     // no r/r overrides
    // uintptr_t lock : 2;        // lock for atomic property, @synch
    // uintptr_t extraBytes : 1;  // allocated with extra bytes

# if __arm64__
// ARM64 simulators have a larger address space, so use the ARM64e
// scheme even when simulators build for ARM64-not-e.
#   if __has_feature(ptrauth_calls) || TARGET_OS_SIMULATOR
#     define ISA_MASK        0x007ffffffffffff8ULL
#     define ISA_MAGIC_MASK  0x0000000000000001ULL
#     define ISA_MAGIC_VALUE 0x0000000000000001ULL
#     define ISA_HAS_CXX_DTOR_BIT 0
#     define ISA_BITFIELD                                                      \
        uintptr_t nonpointer        : 1;                                       \
        uintptr_t has_assoc         : 1;                                       \
        uintptr_t weakly_referenced : 1;                                       \
        uintptr_t shiftcls_and_sig  : 52;                                      \
        uintptr_t has_sidetable_rc  : 1;                                       \
        uintptr_t extra_rc          : 8
#     define RC_ONE   (1ULL<<56)
#     define RC_HALF  (1ULL<<7)
#   else
#     define ISA_MASK        0x0000000ffffffff8ULL
#     define ISA_MAGIC_MASK  0x000003f000000001ULL
#     define ISA_MAGIC_VALUE 0x000001a000000001ULL
#     define ISA_HAS_CXX_DTOR_BIT 1
/*
 // arm64 架构
 struct {
     uintptr_t nonpointer        : 1;  // 0:普通指针，1:优化过，使用位域存储更多信息
     uintptr_t has_assoc         : 1;  // 对象是否含有或曾经含有关联引用
     uintptr_t has_cxx_dtor      : 1;  // 表示是否有C++析构函数或OC的dealloc
     uintptr_t shiftcls          : 33; // 存放着 Class、Meta-Class 对象的内存地址信息
     uintptr_t magic             : 6;  // 用于在调试时分辨对象是否未完成初始化
     uintptr_t weakly_referenced : 1;  // 是否被弱引用指向
     uintptr_t deallocating      : 1;  // 对象是否正在释放
     uintptr_t has_sidetable_rc  : 1;  // 是否需要使用 sidetable 来存储引用计数
     uintptr_t extra_rc          : 19;  // 引用计数能够用 19 个二进制位存储时，直接存储在这里
 };

 */
#     define ISA_BITFIELD                                                      \
        uintptr_t nonpointer        : 1;                                       \
        uintptr_t has_assoc         : 1;                                       \
        uintptr_t has_cxx_dtor      : 1;                                       \
        uintptr_t shiftcls          : 33; /*MACH_VM_MAX_ADDRESS 0x1000000000*/ \
        uintptr_t magic             : 6;                                       \
        uintptr_t weakly_referenced : 1;                                       \
        uintptr_t unused            : 1;                                       \
        uintptr_t has_sidetable_rc  : 1;                                       \
        uintptr_t extra_rc          : 19
#     define RC_ONE   (1ULL<<45)
#     define RC_HALF  (1ULL<<18)
#   endif

# elif __x86_64__
#   define ISA_MASK        0x00007ffffffffff8ULL
#   define ISA_MAGIC_MASK  0x001f800000000001ULL
#   define ISA_MAGIC_VALUE 0x001d800000000001ULL
#   define ISA_HAS_CXX_DTOR_BIT 1
/*
 // x86_64 架构
 struct {
     uintptr_t nonpointer        : 1;  // 0:普通指针，1:优化过，使用位域存储更多信息
     uintptr_t has_assoc         : 1;  // 对象是否含有或曾经含有关联引用
     uintptr_t has_cxx_dtor      : 1;  // 表示是否有C++析构函数或OC的dealloc
     uintptr_t shiftcls          : 44; // 存放着 Class、Meta-Class 对象的内存地址信息
     uintptr_t magic             : 6;  // 用于在调试时分辨对象是否未完成初始化
     uintptr_t weakly_referenced : 1;  // 是否被弱引用指向
     uintptr_t deallocating      : 1;  // 对象是否正在释放
     uintptr_t has_sidetable_rc  : 1;  // 是否需要使用 sidetable 来存储引用计数
     uintptr_t extra_rc          : 8;  // 引用计数能够用 8 个二进制位存储时，直接存储在这里
 };
 */
#   define ISA_BITFIELD                                                        \
      uintptr_t nonpointer        : 1;                                         \
      uintptr_t has_assoc         : 1;                                         \
      uintptr_t has_cxx_dtor      : 1;                                         \
      uintptr_t shiftcls          : 44; /*MACH_VM_MAX_ADDRESS 0x7fffffe00000*/ \
      uintptr_t magic             : 6;                                         \
      uintptr_t weakly_referenced : 1;                                         \
      uintptr_t unused            : 1;                                         \
      uintptr_t has_sidetable_rc  : 1;                                         \
      uintptr_t extra_rc          : 8
#   define RC_ONE   (1ULL<<56)
#   define RC_HALF  (1ULL<<7)

# else
#   error unknown architecture for packed isa
# endif

/*
 这里的has_sidetable_rc 和 extra_rc 的注意点：
 1.has_sidetable_rc 表明该指针是否引用了sidetable散列表，之所以有这个选项，是因为少量的引用计数不会直接存放在SideTables表中的，
 对象的引用计数会先存放在extra_rc中，当其被存满时，才会存入相应的SideTables散列表中，SideTables中有很多张SideTable，每一个SideTable也都是一个散列表，而引用计数表也包含在SideTable 中。
 2.extra_rc 在iSA中存储的引用计数， 当处于x86_64平台时能够存储8个二进制位的引用计数，arm64 平台能存储19个二进制位的引用计数
 */

// SUPPORT_PACKED_ISA
#endif


#if SUPPORT_INDEXED_ISA

# if  __ARM_ARCH_7K__ >= 2  ||  (__arm64__ && !__LP64__)
    // armv7k or arm64_32

#   define ISA_INDEX_IS_NPI_BIT  0
#   define ISA_INDEX_IS_NPI_MASK 0x00000001
#   define ISA_INDEX_MASK        0x0001FFFC
#   define ISA_INDEX_SHIFT       2
#   define ISA_INDEX_BITS        15
#   define ISA_INDEX_COUNT       (1 << ISA_INDEX_BITS)
#   define ISA_INDEX_MAGIC_MASK  0x001E0001
#   define ISA_INDEX_MAGIC_VALUE 0x001C0001
#   define ISA_HAS_CXX_DTOR_BIT  1
#   define ISA_BITFIELD                         \
      uintptr_t nonpointer        : 1;          \
      uintptr_t has_assoc         : 1;          \
      uintptr_t indexcls          : 15;         \
      uintptr_t magic             : 4;          \
      uintptr_t has_cxx_dtor      : 1;          \
      uintptr_t weakly_referenced : 1;          \
      uintptr_t unused            : 1;          \
      uintptr_t has_sidetable_rc  : 1;          \
      uintptr_t extra_rc          : 7
#   define RC_ONE   (1ULL<<25)
#   define RC_HALF  (1ULL<<6)

# else
#   error unknown architecture for indexed isa
# endif

// SUPPORT_INDEXED_ISA
#endif


// _OBJC_ISA_H_
#endif
