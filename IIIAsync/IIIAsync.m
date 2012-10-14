//
//  Concurrently.m
//  Concurrently
//
//  Created by Steve Streza on 7/25/12.
//  Copyright (c) 2012 Mustacheware. All rights reserved.
//

#import "IIIAsync.h"
#import <dispatch/dispatch.h>

@implementation IIIAsync

@synthesize dispatchQueue;

-(id)initWithDispatchQueue:(dispatch_queue_t)queue{
	if((self = [super init])){
		dispatchQueue = queue;
	}
	return self;
}

+(IIIAsync *)mainThreadAsync{
	static IIIAsync *sMainThreadAsync = nil;
	static dispatch_once_t mainThreadToken;
	dispatch_once(&mainThreadToken, ^{
		sMainThreadAsync = [[IIIAsync alloc] initWithDispatchQueue:dispatch_get_main_queue()];
	});
	return sMainThreadAsync;
}

+(IIIAsync *)backgroundThreadAsync{
	static IIIAsync *sBackgroundAsync = nil;
	static dispatch_once_t backgroundThreadToken;
	dispatch_once(&backgroundThreadToken, ^{
		dispatch_queue_t queue = dispatch_queue_create("IIIAsyncBackground", 0);
		sBackgroundAsync = [[IIIAsync alloc] initWithDispatchQueue:queue];
	});
	return sBackgroundAsync;
}

+(IIIAsync *)globalAsync{
	static IIIAsync *sGlobalAsync = nil;
	static dispatch_once_t globalAsyncToken;
	dispatch_once(&globalAsyncToken, ^{
		dispatch_queue_t queue = dispatch_queue_create("IIIAsyncGlobal", DISPATCH_QUEUE_CONCURRENT);
		sGlobalAsync = [[IIIAsync alloc] initWithDispatchQueue:queue];
	});
	return sGlobalAsync;
}

-(void)iterateSerially:(NSArray *)items withIterator:(IIIAsyncIterator)iterator callback:(IIIAsyncCallback)callback{
	dispatch_async(dispatchQueue, ^{
		__block NSUInteger index = 0;
		
		__block dispatch_block_t callNext;
		__block dispatch_block_t continueOrCallback;
		
		NSMutableArray *returnValues = [NSMutableArray array];
		while(returnValues.count < items.count){
			[returnValues addObject:[NSNull null]];
		}
		
		continueOrCallback= ^{
			if(++index < items.count){
				callNext();
			}else{
				callback([returnValues copy], nil);
			}
			
		};
		
		callNext = ^{
			id object = [items objectAtIndex:index];
			
			iterator(object, index, ^(id result, NSError *error){
				if(error){
					callback(nil, error);
				}
				
				if(result){
					[returnValues replaceObjectAtIndex:index withObject:result];
				}
				
				dispatch_async(dispatchQueue, continueOrCallback);
			});
		};
		dispatch_async(dispatchQueue, callNext);
	});
}

-(void)iterateParallel:(NSArray *)items withIterator:(IIIAsyncIterator)iterator callback:(IIIAsyncCallback)callback{
	dispatch_async(dispatchQueue, ^{
		__block BOOL finish = NO;
		__block NSInteger count = items.count;
		
		NSMutableArray *returnValues = [NSMutableArray arrayWithCapacity:items.count];
		while(returnValues.count < items.count){
			[returnValues addObject:[NSNull null]];
		}
		
		dispatch_block_t (^blockForItemAtIndex)(NSUInteger index, IIIAsyncCallback callback) = ^(NSUInteger index, IIIAsyncCallback callback){
			return ^{
				if(finish) return;
				
				id item = [items objectAtIndex:index];
				iterator(item, index, callback);
			};
		};
		
		IIIAsyncCallback (^itemCallbackForIndex)(NSUInteger index) = ^(NSUInteger index){
			return ^(id result, NSError *error){
				if(finish) return;
				
				if(error){
					dispatch_async(dispatchQueue, ^{
						callback(nil, error);
					});
					return;
				}
				
				if(result)
					[returnValues replaceObjectAtIndex:index withObject:result];
				
				if(--count == 0){
					finish = YES;
					callback(returnValues, nil);
				}
			};
		};
		
		for(NSUInteger index = 0; index < items.count; index++){
			dispatch_async(dispatchQueue, blockForItemAtIndex(index, itemCallbackForIndex(index)));
		}
	});
}

-(void)runSeries:(NSArray *)tasks callback:(IIIAsyncCallback)callback{
	dispatch_async(dispatchQueue, ^{
		[self iterateSerially:tasks withIterator:^(id object, NSUInteger index, IIIAsyncCallback callback) {
			IIIAsyncBlock block = (IIIAsyncBlock)object;
			dispatch_async(dispatchQueue, ^{
				block(callback);
			});
		} callback:callback];
	});
}

-(void)runParallel:(NSArray *)blocks callback:(IIIAsyncCallback)callback{
	dispatch_async(dispatchQueue, ^{
		[self iterateParallel:blocks withIterator:^(id object, NSUInteger index, IIIAsyncCallback callback) {
			IIIAsyncBlock block = (IIIAsyncBlock)object;
			block(callback);
		} callback:callback];
	});
}

-(void)runWhileTrue:(IIIAsyncConditional)condition performBlock:(IIIAsyncBlock)block callback:(IIIAsyncCallback)callback{
	[self runConditional:condition whileConditionalIs:YES performBlock:block callback:callback];
}

-(void)runWhileFalse:(IIIAsyncConditional)condition performBlock:(IIIAsyncBlock)block callback:(IIIAsyncCallback)callback{
	[self runConditional:condition whileConditionalIs:NO performBlock:block callback:callback];
}

-(void)runConditional:(IIIAsyncConditional)condition whileConditionalIs:(BOOL)whileValue performBlock:(IIIAsyncBlock)block callback:(IIIAsyncCallback)callback{
	dispatch_block_t __block nextStep = ^{
		dispatch_async(dispatchQueue, ^{
			BOOL result = condition();
			if(result){
				block(^(id result, NSError *error){
					nextStep();
				});
			}else{
				callback(nil, nil);
			}
		});
	};
	nextStep();
}

@end