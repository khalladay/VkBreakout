# VkBreakout

A Breakout Game made with Vulkan to test the speed of different methods of setting uniform data. This is the first thing I've built in Vulkan, so please don't take it as an example of good code or project architecture, because it's awful. 

The project has 4 branches, representing different approaches:

* master - uses a single VkBuffer to store all the per-frame uniforms. Objects are drawn with an offset passed to vkCmdBindDescriptorSets to get their specific data
* push-constants - use push constants instead of any VkBuffer
* multi-buffer - uses a unique buffer for each drawn object
* multi-buffer-nomapping - uses a unique buffer for each drawn object, but keeps the memory for that buffer mapped

In addition, another test was done in master to test the speed of a single buffer approach if you kept the entire buffer mapped all the time. This isn't in it's own branch because it's a trivial change, and I'm lazy. 

The results of the test are as follows: 

![Graph](http://i.imgur.com/fR6drO4.png)

