// RessourcesDirectory.mm

#include "RessourcesDirectory.hpp"
#import <Foundation/Foundation.h>

std::string GetRPath(void) {
    // Get the resources dir path.
	NSBundle* bundle = [NSBundle mainBundle];
//	if (!bundle) {
//		NSLog(@"bundle is nil... thus no resources path can be found.");
//	} else {
		NSString* path = [bundle resourcePath];
		return [path UTF8String] + std::string("/");
//	}
}
