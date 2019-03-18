# Rabbit

This project is a simple implementation of a path tracer that uses OpenCL.
The current implementation is very simple since it was done to try if the architecture ideas and kernel configuration was valid.
The focus was more on setting up a proper structure that could be extented by adding some more features to the renderer.
The system only supports spheres as geometry, has no acceleration structure and has only a diffuse and emitting material models.

Below is an output of the system rendering 100 random spheres with different colors and some emitting ones.
The image resolution is 1920x1080 and was rendered in ~26 seconds on a NVIDIA GTX 1070 using 1024 samples for each pixel.

