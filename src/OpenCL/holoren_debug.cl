/**
 * This version works, but is heavily unoptimized and should be used only for debugging
 */

/**
 * A function to compute spherical wave equation for a single point source
 */
__kernel void compObjWave(__global float *pc,   /// a point cloud with point sources
                          __global float2 *of,   /// an output optical field (float2 because this is an array of complex numbers)
                          uint of_rows,          /// the number of rows of optical field
                          uint of_cols,          /// the number of columns of optical field
                          float hologram_z,      /// the z depth of hologram
                          float k,               /// wavenumber
                          float pitch,           /// pitch between samples
                          float corner_x,        /// x corner of optical field
                          float corner_y)        /// y corner of optical field
{
  int row = get_global_id(1);
  int col = get_global_id(2);
  //float corner_x = -(cols - 1) * pitch / 2;
  //float corner_y = -(rows - 1) * pitch / 2;
  
  int pc_pos = get_global_id(0) * 3;
  float3 ps = (float3) (pc[pc_pos], pc[pc_pos + 1], pc[pc_pos + 2]);
  float x = (((col - 1) * pitch) + corner_x) - ps.x; // tu uz rovno odcitavam aj point source (nerobim to pri vypocte exp)
  float y = (((row - 1) * pitch) + corner_y) - ps.y;
  float z = hologram_z - ps.z;
  float r = sqrt(x * x + y * y + z * z);
  
  /*
     OpenCL does not directly support complex numbers
     but complex exponentiation can decomposed via euler
     identity to cosines and sines:

     z = r * exp(j * phi)   ... complex number (where r is the amplitude and phi is named modulus)

     exp(j * phi) = cos(phi) + j * sin(phi)   ->  re = r * cos(phi)
                                                  im = r * sin(phi)
   */
   /*
  uint of_index = row * of_cols + col;
  float2 sample = of[of_index];
  sample += (float2) (cos(k * r), sin(k * r));
  of[of_index] = sample;
  */

  float tmp = k * r;
  float re = cos(tmp);
  float im = sin(tmp);
  uint of_index = row * of_cols + col;
  float2 sample = of[of_index];
  sample += (float2) (re, im);
  of[of_index] = sample;

  //of[row * of_cols + col].s0 +=  cos(k * r);
  //of[row * of_cols + col].s1 +=  sin(k * r);
}
