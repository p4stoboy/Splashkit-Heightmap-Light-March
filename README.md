
1. create abstract grid of cells as some function of screen size
2. use interp function (noise, IDW etc) to assign height value to each cell
3. create light at arbitrary (x,y,z) position relative to grid
4. march _from each cell_ (z position = height value) toward light

    (we do this backwards as with some proper heurisitcs we can get accurate looking light but can still break early on occlusion)
5. use luminosity value of each cell to inform render of grid
