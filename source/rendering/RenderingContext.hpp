//
// Created by Simon on 2019-03-01.
//

#ifndef RABBIT_RENDERINGCONTEXT_HPP
#define RABBIT_RENDERINGCONTEXT_HPP

#include "RenderingData.hpp"

namespace CL
{
    // This class is responsible for managing the resources sued during rendering
    class RenderingContext
    {
    public:

    private:
        // Device where we run our rendering process, this is passed in from outside
        cl::Device target_device;
        // OpenCL context
        cl::Context context;

        // Programs and kernels, maybe in a new class

        // Data for the kernels
        Rays d_rays;
        Intersections d_intersections;
        Samples d_samples;
        XOrShift d_xorshift_state;
    };


} // CL namespace

#endif //RABBIT_RENDERINGCONTEXT_HPP
