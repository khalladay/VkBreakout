# VkBreakout

A Breakout Game made with Vulkan to test the speed of different methods of setting uniform data. This is the first thing I've built in Vulkan, so please don't take it as an example of good code or project architecture, because it's awful. 

The project has multiple branches, representing different approaches. The branches marked with the prefix 01- are the branches used in my initial performance tests, presented [on my website](http://kylehalladay.com/blog/tutorial/vulkan/2017/08/30/Vulkan-Uniform-Buffers.html). 

* 01-DynamicUniformBuffer - uses a single VkBuffer to store all the per-frame uniforms. Objects are drawn with an offset passed to vkCmdBindDescriptorSets to get their specific data
* 01-Push-Constants - use push constants instead of any VkBuffer
* 01-Multi-Buffer - uses a unique buffer for each drawn object
* 01-Multi-Buffer-KeepMapped - uses a unique buffer for each drawn object, but keeps the memory for that buffer mapped

* 02-DynamicUniformBuffer - still using a single VkBuffer, but now that buffer is Device-Local, and some memory optimization have been applied
* 02-Multi-Buffer - using Device-Local memory for the uniform buffer passed to the gpu
* 02-Multi-Buffer -KeepMapped - using Device-Local memory for the uniform buffer passed to the gpu

* 03-DynamicUniformBuffer - using multi draw indirect to render all blocks in a single draw call

In addition, another test was done in 01-DynamicUniformBuffer to test the speed of a single buffer approach if you kept the entire buffer mapped all the time. This isn't in it's own branch because it's a trivial change, and I'm lazy. 

The results of the test are as follows: 

![Graph](http://i.imgur.com/1TRVFSp.png)

