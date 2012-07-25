//
//  Concurrently.h
//  Concurrently
//
//  Created by Steve Streza on 7/25/12.
//  Copyright (c) 2012 Mustacheware. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <dispatch/dispatch.h>

typedef void(^IIIAsyncCallback)(id result, NSError *error);
typedef void(^IIIAsyncBlock)(IIIAsyncCallback callback);
typedef void(^IIIAsyncIterator)(id object, NSUInteger index, IIIAsyncCallback callback);

@interface IIIAsync : NSObject

+(IIIAsync *)mainThreadAsync;
+(IIIAsync *)backgroundThreadAsync;
+(IIIAsync *)globalAsync;

@property (nonatomic, retain) dispatch_queue_t dispatchQueue;

-(void)iterateSerially:(NSArray *)blocks withIterator:(IIIAsyncIterator)iterator callback:(IIIAsyncCallback)callback;
-(void)iterateParallel:(NSArray *)blocks withIterator:(IIIAsyncIterator)iterator callback:(IIIAsyncCallback)callback;

-(void)runSeries:(NSArray *)tasks    callback:(IIIAsyncCallback)callback;
-(void)runParallel:(NSArray *)blocks callback:(IIIAsyncCallback)callback;

@end
