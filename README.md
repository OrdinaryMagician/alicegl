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
involves some manual handiwork after their creation. Same as with arrays, the
data you pass is used directly, not copied.

After all that is out of the way, I'll be moving to the transform programs. A
transform program is pretty much a function that gets called for each
primitive at each pass (vertex, fragment, framebuffer), and modifies the
resulting value(s). Shaders, pretty much, but without being as isolated as in
other APIs (which can be dangerous...).

The way screen shaders work may seem a bit alien to everyone, but it's easy to
understand. When you call aglShadeBuffer, the library will go on a loop for
each pixel inside it, calling whatever you passed as the buffer program.

It's very simple to create samplers from buffers. Just make sure you OR its
type with SMP_BUF, so the program knows its data is not to be freed at exit,
since it will be done on buffer deletion anyway. Aditionally, for making
samplers from the depth and stencil buffers, the D* and S* formats are
available.

Then, when all the graphics stuff is over, I'll move on to making the other
libraries of the whole suite. One for vector/matrix math, the other for more
game-specific functionality, like physics and animation.

## These messages, what do they mean?

AliceGL will vomit out messages to stderr every time something goes either
slightly wrong or terribly wrong ("warnings" or "errors"). Using a format
similar to ALSA's libraries to know where in the code stuff is happening, the
current messages and their explanations are the following:

- **Null pointer returned from malloc (out of memory?)**: means what it says, a
  call to malloc() returned a null pointer, this could usually mean you ran
  out of memory, somehow.
- **Oh gn0! I am teh suck!**: You're passing a nonexistent context to API
  calls, you doofus.
- **There was an object here, but it's gone now**: Deleting an object that
  doesn't exist, trying to draw a nonexistent array or pushing to an
  unallocated queue. Also, accessing null samplers/buffers.
- **Attempted to start queue with zero size**: You can guess what this means.
- **No space left in queue to push data**: You're pushing more data than the
  queue can handle. Fix this by setting a big enough capacity and reallocating
  the queue.
- **Call to aglDrawQueue with no data**: Nothing to draw, obviously.
- **Premature end to queue (bad flags?)**: This usually happens if you're not
  pushing all required data to the queue. The kind of issue the library itself
  can easily detect. If you push things in the wrong order, you're usually the
  one who can detect it, with your own eyes, being stabbed with glitched
  triangles. In the future I'll try to find a way to prevent this from actually
  happening, by keeping track of what type of element needs to be pushed every
  time.
- **Cannot push data with current flags**: The queue flags don't allow for the
  type of data you're pushing.
- **AGL will now attempt to fit a square peg in a round hole**: You're either
  accessing a 2D sampler with aglTex3D or a 3D sampler with aglTex2D. The
  value returned will be "mostly" correct for each situation, though.
- **No data has been passed to API call**: This will happen when creating
  arrays or samplers. It won't prevent the creation of either object, but just
  let you know that bad things will happen if you do NOT set actual data before
  using them.
- **Element has no data to read**: The companion to the previous warning. The
  library is attempting to read from an object with no assigned data. It will
  default to some predefined fallback value in such a case. This will also
  happen when reading from uninitialized samplers.
- **No vertices to see here**: You forgot to set the ARR_VERTICES flag on
  either the queue or an array. This will easily happen if you're drawing from
  an uninitialized queue/array.
- **No target buffer has been set**: A geometry draw operation cannot be
  performed if you didn't set the target buffer.
- **Function not implemented**: You're using something that isn't done yet.
  This will show up A LOT before the project is 100% complete.

## Okay whatever... so... is it going to be FOSS?

Of course! And obviously under the ~~nice permissive MIT license I like so
much~~ GNU LGPL v3, because I think MIT won't really fit here.
