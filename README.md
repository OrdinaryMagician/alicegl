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
"version" of it from the ground up. Thus, AliceGL draws a bit of inspiration
from good ol' OpenGL (including its current temporary name).

## So... how in the hell does this thing work?

The whole thing is split in two parts: Frontend and backend.

The frontend, libaglext, is the "surface" of the API. The part programs work
with, in short.

The frontend itself is separated in three sections:
* State tracker: This part is what packs up all operations to be sent
  to the backend. Having this here makes AliceGL capable of using other
  things as backend, such as OpenGL (no, seriously).
* Client API: This is what's provided to the developer(s). API calls,
  constants and such.

The backend, libagleng, is the "engine" of the API. The part that does all the
hard work.

It's separated in three sections, too:
* Settings: Contains all the switches and other global data to tweak the render
  process.
* Object pool: This part in particular is where all the primitives are stored,
  such as vertex buffers, frame buffers, samplers and shaders.
* Render pipeline: The big fat performance hog. Does all the drawing and stuff.
  It's divided in "Trigger" and "Rasterizer". The trigger is just there to
  build and process raster operation queues, which can be done in parallel, if
  possible. And the rasterizer... well, it's obvious what that one does.

The separation was mostly ideated first as preparation for the eventual AliceOS
version, which would run the backend on kernel space. The possibility of other
backends is just a side effect.

## Do you even have any idea what you're talking about?

Nope. That's why I'm doing this in the first place. With time my total and
absolute ignorance on this field will disappear. For now there's just my
"special" interpretation of things.

## Okay whatever... so... is it going to be FOSS?

Of course! And obviously under the nice permissive MIT license I like so much.
