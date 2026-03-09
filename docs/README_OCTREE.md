# OctTree

To speed up calculations for triangles, I implemented [BVH (Bounding Volume Hierarchy)](https://en.wikipedia.org/wiki/Bounding_volume_hierarchy) - top-down octree that use [AABB (Axis-aligned minimum bounding box)](https://en.wikipedia.org/wiki/Minimum_bounding_box) and [BS (Bounding sphere)](https://en.wikipedia.org/wiki/Bounding_sphere)

Good article for octrees and how they works:
[Original guide](https://www.gamedev.net/articles/programming/general-and-gameplay-programming/introduction-to-octrees-r3529/), [Russian translation](https://habr.com/ru/articles/334990/)

Also, here are some speed-up tips:
+ Than raw check in triangles intersection method for intersection of AABB for them speed up program enormously too.
+ Add a minimum amount of triangles in leafes of octree - another check to end recursion, experiments says that 64-512 increases speed the most, and the longest are 1-8 and 2048+.
+ Precalculated entities for main object - triangles also improve speed - like plane, AABB, and its case(collapsed or not), you dont need calculate it every time because it is static after creation.
+ `inline` + `noexcept`

For 100000 triangles, time before was 200s+ to calculate triangles intersection. And after - 0.8s.
