//
//  IIIAsync.h
//  IIIAsync
//
//  Created by Steve Streza on 7/25/12.
//  Copyright (c) 2012 Steve Streza
//  
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//  
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//  
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.
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

-(void)iterateSeriallyWithIterator:(IIIAsyncIterator)iterator callback:(IIIAsyncCallback)callback blocks:(id) block, ...;
-(void)iterateParallelWithIterator:(IIIAsyncIterator)iterator callback:(IIIAsyncCallback)callback blocks:(id) block, ...;

-(void)runSeries:(NSArray *)tasks    callback:(IIIAsyncCallback)callback;
-(void)runParallel:(NSArray *)blocks callback:(IIIAsyncCallback)callback;

-(void)runSeriesWithCallback:(IIIAsyncCallback)callback   tasks:(id) task, ...;
-(void)runParallelWithCallback:(IIIAsyncCallback)callback blocks:(id) block, ...;

-(void)runWhileTrue:(IIIAsyncConditional)condition performBlock:(IIIAsyncBlock)block callback:(IIIAsyncCallback)callback;
-(void)runWhileFalse:(IIIAsyncConditional)condition performBlock:(IIIAsyncBlock)block callback:(IIIAsyncCallback)callback;

@end