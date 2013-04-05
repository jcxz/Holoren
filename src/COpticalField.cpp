/**
 */

#include "COpticalField.h"



/**
 */
bool COpticalField::save(std::ostream & os)
{
  /* write header */
  const SOpticalFieldPrivate & data = m_data.constData();

  HOLOREN_PACKED_STRUCT(struct SDFHeader {
    uint8_t hdr_tag_len;      /// header tag length
    uint8_t hdr_tag[4];       /// header tag
    double wavelength;        /// light wavelength
    double h_pitch;           /// horizontal pitch between samples
    double v_pitch;           /// vertical pitch between samples
    uint64_t h_res;           /// horizontal resolution of the optical field
    uint64_t v_res;           /// vertical resolution of the optical field
    double center_x;          /// optical field's center position (x component)
    double center_y;          /// optical field's center position (y component)
    uint8_t body_tag_len;     /// body tag length
    uint8_t body_tag[4];      /// body tag
  }) hdr = {
    (uint8_t) 4,
    { 'D', 'F', 'H', 'D' },
    data.m_lambda,
    data.m_pitch,
    data.m_pitch,
    (uint64_t) data.m_cols,
    (uint64_t) data.m_rows,
    (double) 0,
    (double) 0,
    (uint8_t) 4,
#ifdef USE_DOUBLE
    {'D', 'F', 'P', 'T' }
#else
    {'D', 'F', 'B', 'F' }
#endif
  };

  os.write((const char *) &hdr, sizeof(hdr));

  /* write body */
  unsigned int size = data.m_cols * data.m_rows;
  for (unsigned int i = 0; i < size; ++i)
  {
    tFPType re = data.m_samples[i].real();
    tFPType im = data.m_samples[i].imag();

#ifdef USE_DOUBLE
    os.write((const char *) &re, sizeof(double));
    os.write((const char *) &im, sizeof(double));
#else
    os.write((const char *) &re, sizeof(float));
    os.write((const char *) &im, sizeof(float));
#endif
  }

  return os.good();
}
