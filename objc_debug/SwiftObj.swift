//
//  SwiftObj.swift
//  objc_debug
//
//  Created by xiaoyuan on 2021/4/7.
//

import Foundation

@objc class SwiftObj: NSObject {

    var num: Int = 1
    
     @objc func run() {
        print("run")
        
        let obj = SwiftObj()
        obj.num += 1
        print(obj.num)
    }
}
