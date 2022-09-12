# Ξ
(xi) is a software development tool which utilizes an architecture optimized for high locality of reference and high cache friendliness, while also remaining lightweight and maleable. Most software today is much slower than it needs to be.

The proof of concept precursor to this utility was [Locality](https://github.com/LucAlexander/Locality), which was developed as a semester project for university.

## Current Development
Until now i've been using SDL2 to do all media features of the software engine. But after optimizing the internal architecture, SDL2's rendering specification has become the only bottleneck in an otherwise very fast software solution. Im slowly trying to replace SDL2 with OpenGl, which should be able to hangle rendering and user input, however audio capabilities will have to come later. Aside from being much faster, OpenGL also has shader support.

## Documentation
**TODO**



