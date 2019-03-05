#ifndef KERNEL_COMMON_H
#define KERNEL_COMMON_H

// Invalid primitive index
#define MAX_UINT                4294967295u
#define INVALID_PRIM_INDEX      MAX_UINT
#define RAY_DONE_DEPTH          MAX_UINT
#define XORSHIFT_STATE_START    52938u

typedef union
{
    unsigned int ui;
    float f;
} RandomFloatHelper;

// Generate random float in [0, 1) from a given xorshift state
inline float GenerateFloat(__global unsigned int* xorshift_state)
{
    // Generate new state
    unsigned int new_state = *xorshift_state;
    new_state ^= new_state << 13;
    new_state ^= new_state >> 17;
    new_state ^= new_state << 5;

    // Store new state
    *xorshift_state = new_state;

    // Compute float value
    RandomFloatHelper temp;
    temp.ui = (new_state >> 9) | 0x3f800000u;

    return temp.f;
}

// Solve quadratic equation
inline bool SolveQuadratic(float a, float b, float c, float* t0, float* t1)
{
    const float discr = b * b - 4.f * a * c;
    if (discr < 0.f)
    {
        return false;
    }

    const float discr_root = sqrt(discr);
    float q;
    if (b < 0.f)
    {
        q = -0.5f * (b - discr_root);
    }
    else
    {
        q = -0.5f * (b + discr_root);
    }

    t0 = q / a;
    t1 = c / q;

    if (t0 > t1)
    {
        const float temp = t0;
        t0 = t1;
        t1 = temp;
    }

    return true;
}

#endif // KERNEL_COMMON_H
