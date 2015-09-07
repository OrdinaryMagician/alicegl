# AliceGL
### The new graphics API for magical girls

## Wait... what?

Yes, it's AliceGL! The API that won't probably bring peace to the everlasting
API war out there between Direct3D and OpenGL (mostly because it's just a mere
software rasterizer), but it might as well be nice to have around.

This thing is yet another of my learning projects, as part of AOS. Because I
recently started getting into working with low level graphics, and boy do I
love it.

However, I believe the best way to learn about something is to build your own
"version" of it from the ground up. Please do not expect AliceGL to be in any
way similar to OpenGL except on its (temporary) name. OpenGL is an awfully
designed API, although it's "less worse" than Direct3D anyway.

## So... how in the hell does this thing work?

The main important aspect of AliceGL is the context object. A context object
contains pretty much all the render state along with buffers and other
resources, it is passed around across pretty much all API calls, which is only
necessary because this is C, not C++ or some other language that has classes
and as such would automatically handle this behind your back (that is how
classes work, after all).

The API is designed to be as small as possible, focusing only on what's
essential for rasterization of triangles and applying modifications to
vertices, fragments, and render buffers (the latter being a much more
straightforward way of doing screen-space shaders, rather than hackily using
render to texture like in other APIs; plus it provides full, unrestricted
access to the whole thing, rather than being a function applied per-pixel).

There are two modes of operation for passing geometry data to the rasterizer:
 - The first works similarly to the fixed pipeline in good ol' OpenGL 1.x, you
just queue up data bit by bit, then "end" the whole thing. However unlike in
OpenGL, you can queue up all the data then repeat the final draw multiple
times. There's just one queue, and due to the low-level design of this
library, you have to give it a maximum capacity yourself when initializing
it, along with allocating its memory.
 - The second is pretty much like modern VBOs. You build the array of all
geometry data, then call aglMakeArray() with it. Note that since there's no
memory separation here, the data you passed will be used directly, so don't
free it. With the array created, you can just call aglDrawArray() with whatever
array you want to draw, anytime. Oh, and if you want you can modify the
contents of said array, if you know its internal structure, just don't free the
data yourself.

Now, the important part, the rasterizer itself. I've been reading up various
methods of drawing triangles, and I think I'm getting the general idea already.
Just draw horizontal spans between two edges in a loop, that's it. There seem
to be various ways of doing this, each with their own advantages/disadvantages.

With that out of the way I can easily guess how passing coords, normals and
whatnot would happen, along with how to do depth testing. I probably won't
come up with a fast way to do it all, but keep in mind I'm just learning.

Another part of the API deals with texture handling. At first I'll handle just
2D textures, then move to 3D textures (both cube maps and volume maps). The
various forms of coordinate wrapping/clamping/whatever and subpixel filtering
will probably keep me up at night. Note: The process of setting up samplers
involves some manual handiwork after their creation.

After all that is out of the way, I'll be moving to the transform programs. A
transform program is pretty much a function that gets called for each
primitive at each pass (vertex, fragment, framebuffer), and modifies the
resulting value(s). Shaders, pretty much, but without being as isolated as in
other APIs (which can be dangerous...).

Then, when all the graphics stuff is over, I'll move on to making the other
libraries of the whole suite. One for vector/matrix math, the other for more
game-specific functionality, like physics and animation.

## Okay whatever... so... is it going to be FOSS?

Of course! And obviously under the ~~nice permissive MIT license I like so
much~~ GNU LGPL v3, because I think MIT won't really fit here.
