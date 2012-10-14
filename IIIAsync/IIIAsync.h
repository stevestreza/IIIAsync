//
//  IIIAsync.h
//  IIIAsync
//
//  Created by Steve Streza on 7/25/12.
//  Copyright (c) 2012 Mustacheware. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <dispatch/dispatch.h>

typedef void(^IIIAsyncCallback)(id result, NSError *error);
typedef void(^IIIAsyncBlock)(IIIAsyncCallback callback);
typedef void(^IIIAsyncIterator)(id object, NSUInteger index, IIIAsyncCallback callback);
typedef BOOL(^IIIAsyncConditional)(void);

@interface IIIAsync : NSObject

@property (nonatomic, readonly, retain) dispatch_queue_t dispatchQueue;

+(IIIAsync *)mainThreadAsync;
+(IIIAsync *)backgroundThreadAsync;
+(IIIAsync *)globalAsync;

-(void)iterateSerially:(NSArray *)blocks withIterator:(IIIAsyncIterator)iterator callback:(IIIAsyncCallback)callback;
-(void)iterateParallel:(NSArray *)blocks withIterator:(IIIAsyncIterator)iterator callback:(IIIAsyncCallback)callback;

-(void)runSeries:(NSArray *)tasks    callback:(IIIAsyncCallback)callback;
-(void)runParallel:(NSArray *)blocks callback:(IIIAsyncCallback)callback;

-(void)runWhileTrue:(IIIAsyncConditional)condition performBlock:(IIIAsyncBlock)block callback:(IIIAsyncCallback)callback;
-(void)runWhileFalse:(IIIAsyncConditional)condition performBlock:(IIIAsyncBlock)block callback:(IIIAsyncCallback)callback;

@end