`IIIAsync` is a control flow system for managing multiple operations serially or in parallel. It can collect the results of multiple operations, either objects or errors. When all operations finish, you get a completion handler with the results in a structured order, regardless of which order they complete. You can run these operations either on the main queue, on a single background thread pool, or on a multi-threaded background pool. It makes it easier to organize code and obtain results, letting you build powerful higher-order operations that rely on multiple asynchronous calls.

`IIIAsync` is heavily modeled after and inspired by the brilliant [async.js](https://github.com/caolan/async) library by [Caolan McMahon](https://github.com/caolan).

Installation
============

Download the code (either standalone or as a submodule) and include `IIIAsync.h` and `IIIAsync.m` in your Xcode project.

Usage
=====

There are three `IIIAsync` singletons which determine which context to run operations on. Either:

- `[IIIAsync mainThread]`,
- `[IIIAsync backgroundThreadAsync]`, or
- `[IIIAsync globalAsync]`

These run your code in a dispatch queue on the main thread, a single background thread, or the global background thread, respectively.

Each of these has the following set of APIs:

- `-[IIIAsync iterateSerially:withIteratorTask:completionHandler:]` - iterates over an array, invoking the iterator task on each item in the array, one at a time, and performs the completion handler after the iteration is finished. Order is guaranteed; each item will not be invoked until the previous item is completed. The result will be an array of return values and an optional error.
- `-[IIIAsync iterateParallel:withIteratorTask:completionHandler:]` - iterates over an array, invoking the iterator task on each item in the array, and performs the completion handler after the iteration is finished. Order is not guaranteed. The result will be an array of return values and an optional error.
- `-[IIIAsync runTasksInSeries:withCompletionHandler:]` - iterates over an array of tasks serially and performs the completion handler when the iteration is complete. Order is guaranteed; each task will not be invoked until the previous task is completed. The result will be an array of return values and an optional error.
- `-[IIIAsync runTasksInParallel:withCompletionHandler:]` - iterates over an array of tasks in parallel and performs the completion handler when the iteration is complete. Order is not guaranteed. The result will be an array of return values and an optional error.
- `-[IIIAsync runWhileTrue:performTask:withCompletionHandler:]` - Performs a task while a conditional returns true, and performs the completion handler when the conditional returns false. Returns nothing and an optional error.
- `-[IIIAsync runWhileFalse:performTask:withCompletionHandler:]` - Performs a task while a conditional returns false, and performs the completion handler when the conditional returns true. Returns nothing and an optional error.

There are a few block types that can be called, based on API:

- `IIIAsyncTask`: Accepts an `IIIAsyncTaskCompletionHandler` block (see below). You supply an array of these to the `run*:` APIs for your tasks. When the task is complete, you call the `IIIAsyncTaskCompletionHandler` block to signal that it has finished.
- `IIIAsyncIteratorTask`: Accepts an `id` object, an `NSUInteger` representing the index of the object, and an `IIIAsyncTaskCompletionHandler` block (see below). You supply one of these to the `iterate*:` APIs for your operations. When each iteration has completed, you call the `IIIAsyncTaskCompletionHandler` block to signal that it has finished.
- `IIIAsyncTaskCompletionHandler`: Accepts an `id` object and an optional error. You call this with the result of your operation and an optional `NSError`, which will be collected as necessary. You also implement this to get the results of an async operation.
- `IIIAsyncConditional`: Accepts no parameters and returns a `BOOL`. You implement this for the `runWhile*:` APIs, to determine when to break out of the loop.

Example
=======

For our purposes we will assume there is a method that looks like this:

`-(void)loadImageAtURL:(NSURL *)url handler:(void (^)(UIImage *image, NSError *error))handler;`

**Load three images in parallel:**

```objective-c
NSArray *images = @[
	[NSURL URLWithString:@"http://placekitten.com/200/300"],
	[NSURL URLWithString:@"http://placekitten.com/300/200"],
	[NSURL URLWithString:@"http://placekitten.com/450/450"]
];

[[IIIAsync mainThread] iterateParallel:images withIteratorTask:^(id object, NSUInteger index, IIIAsyncTaskCompletionHandler completionHandler){
	NSURL *url = (NSURL *)object;
	[self loadImageAtURL:url handler:^(UIImage *image, NSError *error){
		completionHandler(image, error);
	}];
} completionHandler:^(id result, NSError *error){
	if(error){
		NSLog(@"One of the operations returned an error: %@", error);
	}else{
		NSArray *images = (NSArray *)result;
		NSLog(@"All three images: %@", result);
	}
}];
```

Tests
=====

The Xcode project includes a unit test suite. You can run it by opening the Xcode project, selecting the IIIAsync scheme, and selecting "Test" from the Product menu.

No pull requests will be considered unless they contain adequate, passing unit tests.

License
=======

**tl;dr: MIT license, do what you want, just attribute for it**

Copyright (c) 2012 Steve Streza

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.