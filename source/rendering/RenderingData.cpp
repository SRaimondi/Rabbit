//
// Created by Simon on 2019-03-01.
//

#include "RenderingData.hpp"

namespace CL
{
    Rays::Rays(const cl::Context& context, unsigned int num_rays)
        : num_rays{ num_rays },
          origin_x{ context, CL_MEM_READ_WRITE, num_rays * sizeof(cl_float) },
          origin_y{ context, CL_MEM_READ_WRITE, num_rays * sizeof(cl_float) },
          origin_z{ context, CL_MEM_READ_WRITE, num_rays * sizeof(cl_float) },
          direction_x{ context, CL_MEM_READ_WRITE, num_rays * sizeof(cl_float) },
          direction_y{ context, CL_MEM_READ_WRITE, num_rays * sizeof(cl_float) },
          direction_z{ context, CL_MEM_READ_WRITE, num_rays * sizeof(cl_float) },
          extent{ context, CL_MEM_READ_WRITE, num_rays * sizeof(cl_float) },
          depth{ context, CL_MEM_READ_WRITE, num_rays * sizeof(cl_uint) }
    {}

    Intersections::Intersections(const cl::Context& context, unsigned int num_intersections)
        : num_intersections(num_intersections),
          hit_point_x{ context, CL_MEM_READ_WRITE, num_intersections * sizeof(cl_float) },
          hit_point_y{ context, CL_MEM_READ_WRITE, num_intersections * sizeof(cl_float) },
          hit_point_z{ context, CL_MEM_READ_WRITE, num_intersections * sizeof(cl_float) },
          normal_x{ context, CL_MEM_READ_WRITE, num_intersections * sizeof(cl_float) },
          normal_y{ context, CL_MEM_READ_WRITE, num_intersections * sizeof(cl_float) },
          normal_z{ context, CL_MEM_READ_WRITE, num_intersections * sizeof(cl_float) },
          uv_s{ context, CL_MEM_READ_WRITE, num_intersections * sizeof(cl_float) },
          uv_t{ context, CL_MEM_READ_WRITE, num_intersections * sizeof(cl_float) },
          primitive_index{ context, CL_MEM_READ_WRITE, num_intersections * sizeof(cl_uint) }
    {}

    Samples::Samples(const cl::Context& context, unsigned int num_samples)
        : num_samples{ num_samples },
          Li_r{ context, CL_MEM_READ_WRITE, num_samples * sizeof(cl_float) },
          Li_g{ context, CL_MEM_READ_WRITE, num_samples * sizeof(cl_float) },
          Li_b{ context, CL_MEM_READ_WRITE, num_samples * sizeof(cl_float) },
          beta_r{ context, CL_MEM_READ_WRITE, num_samples * sizeof(cl_float) },
          beta_g{ context, CL_MEM_READ_WRITE, num_samples * sizeof(cl_float) },
          beta_b{ context, CL_MEM_READ_WRITE, num_samples * sizeof(cl_float) },
          pixel_x{ context, CL_MEM_READ_WRITE, num_samples * sizeof(cl_uint) },
          pixel_y{ context, CL_MEM_READ_WRITE, num_samples * sizeof(cl_uint) },
          sample_offset_x{ context, CL_MEM_READ_WRITE, num_samples * sizeof(cl_float) },
          sample_offset_y{ context, CL_MEM_READ_WRITE, num_samples * sizeof(cl_float) }
    {}

    XOrShift::XOrShift(const cl::Context& context, unsigned int num_generators)
        : num_generators{ num_generators },
          state{ context, CL_MEM_READ_WRITE, num_generators * sizeof(cl_uint) }
    {}

} // CL namespace
