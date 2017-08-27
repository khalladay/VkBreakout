# VkBreakout

This is a quick (and not terribly fun) breakout game made to try out different parts of the Vulkan API. It's the first thing I've built with Vulkan, so please don't take it as an example of good code or project architecture, because it's awful. 

The project has multiple branches, representing different approaches to passing uniform data to shaders, and allocating memory. The branches marked with the prefix 01- are the branches used in my initial performance tests, presented [on my website](http://kylehalladay.com/blog/tutorial/vulkan/2017/08/13/Vulkan-Uniform-Buffers.html). 

The branches marked with 02 or 03 prefixes were used in the [follow up post](http://kylehalladay.com/blog/tutorial/vulkan/2017/08/13/Vulkan-Uniform-Buffers.html)

The performance of the 01- branches are as follows: 

![Graph](http://i.imgur.com/1TRVFSp.png)

The performance of the 03- branches are as follows:

![Graph](http://i.imgur.com/RDbSSP0.png)

More information about how this performance data was gathered, the reasoning behind the changes in each branch, and general implementation details can be found on the two linked blog posts. 
