/**
 */

#include "global.h"
#include "CImagePointCloudRW.h"
#include "CPointCloud.h"

#include <cstring>
#include <SDL_image.h>


/**
 * A macro to get the full 32bit value of a pixel (with all color channels)
 */
#define GET_PIXEL32(surf, x, y) \
  (*((Uint32 *) (((Uint8 *) (surf)->pixels) + (y) * (surf)->pitch + (x))))

/**
 * A macro to get the first 8-bit component (color channel) of a pixel
 */
#define GET_PIXEL8(surf, x, y) \
  (*(((Uint8 *) (surf)->pixels) + (y) * (surf)->pitch + (x)))

/**
 * Size of a single pixel (this is 4 bytes for a 32-bit RGBA pixel format)
 */
#define BYTES_PER_PIXEL 4



/**
 * A free function to perform clean-up for this reader-writer module
 */
static void cleanup_handler(void)
{
  IMG_Quit();
  SDL_Quit();
  return;
}


/**
 */
bool CImagePointCloudRW::init(tCleanUpHandler *p_handler)
{
  /* do some intial setup */
  if (p_handler != NULL)
  {
    *p_handler = NULL;
  }

  m_error = "";

  /* initialize SDL library */
  if (SDL_Init(SDL_INIT_VIDEO) == -1)
  {
    m_error = SDL_GetError();
    return false;
  }

  /* initialize SDL_image library */
  int flags = (IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF);

  /* IMG_Init retruns an or-ed mask of flags of all libraries
     that it could initialize */
  if (IMG_Init(flags) != flags)
  {
    SDL_Quit();
    m_error = IMG_GetError();
    return false;
  }

  /* setup clean-up handler */
  if (p_handler != NULL)
  {
    *p_handler = cleanup_handler;
  }

  return true;
}


/**
 */
bool CImagePointCloudRW::read(CPointCloud *pc)
{
  m_error = "";

  /* load the image from file */
  DBG("loading image");
  SDL_Surface *img_surf = IMG_Load(m_filename.c_str());
  if (img_surf == NULL)
  {
    m_error = "Failed to load image: \"";
    m_error += IMG_GetError();
    m_error += "\"";
    return false;
  }

  /* convert the surface to rgba format if necessary */
  DBG("Converting surface");
  if (!toRGBASurface(&img_surf))
  {
    SDL_FreeSurface(img_surf);
    m_error = "Failed to convert SDL surface: \"";
    m_error += SDL_GetError();
    m_error += "\"";
    return false;
  }

  /* convert surface to greyscale */
  DBG("Generating greyscale surface");
  if (!toGreyScale(img_surf))
  {
    SDL_FreeSurface(img_surf);
    m_error = "Failed to convert SDL surface: \"";
    m_error += SDL_GetError();
    m_error += "\"";
    return false;
  }

#ifdef DEBUG
  DBG("Saving greyscale debug image to: \"dbg_gs_img.bmp\"");
  if (SDL_SaveBMP(img_surf, "dbg_gs_img.bmp") == -1)
  {
    DBG("Failed to save greyscale debug image");
  }
#endif

  /* pick the mean value from histogram of the surface */
  DBG("Getting treshold");
  uint8_t treshold = 0;
  if (!getAvgIntensity(img_surf, &treshold))
  {
    SDL_FreeSurface(img_surf);
    m_error = "Failed to convert SDL surface: \"";
    m_error += SDL_GetError();
    m_error += "\"";
    return false;
  }

  DBG("Treshold: " << (unsigned) treshold);

  /* create point sources from grey pixels */
  DBG("Generating point sources");
  if (!genPointSources(img_surf, treshold, 0.0, 0.000025, pc))
  {
    SDL_FreeSurface(img_surf);
    m_error = "Failed to generate point sources from image: \"";
    m_error += SDL_GetError();
    m_error += "\"";
    return false;
  }

  /* binarize surface for debugging purposes */
#ifdef DEBUG
  if (toBinary(img_surf, treshold))
  {
    DBG("Saving binarized debug image to: \"dbg_bw_img.bmp\"");
    if (SDL_SaveBMP(img_surf, "dbg_bw_img.bmp") == -1)
    {
      DBG("Failed to save binarized debug image");
    }
  }
  else
  {
    DBG("Failed to binarize surface");
  }
#endif

  /* free the SDL surface */
  SDL_FreeSurface(img_surf);

  return true;
}


/**
 */
bool CImagePointCloudRW::write(CPointCloud *pc)
{
  HOLOREN_UNUSED(pc);
  return false;
}


/**
 */
bool CImagePointCloudRW::toRGBASurface(SDL_Surface **surf)
{
  HOLOREN_ASSERT(surf != NULL);

  /* check if we need to convert */
  if ((*surf)->format->BytesPerPixel == 4)
  {
    DBG("Skipping conversion: bytes per pixel is 4");
    return true;
  }

  /* setup desired image format */
  SDL_PixelFormat fmt;
  std::memset(&fmt, 0, sizeof(fmt));

  fmt.BitsPerPixel = 32;
  fmt.BytesPerPixel = BYTES_PER_PIXEL;
  fmt.Rshift = 24;
  fmt.Gshift = 16;
  fmt.Bshift = 8;
  fmt.Ashift = 0;

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
  fmt.Rmask = 0x000000FF;
  fmt.Gmask = 0x0000FF00;
  fmt.Bmask = 0x00FF0000;
  fmt.Amask = 0xFF000000;
#else
  fmt.Rmask = 0xFF000000;
  fmt.Gmask = 0x00FF0000;
  fmt.Bmask = 0x0000FF00;
  fmt.Amask = 0x000000FF;
#endif
  
  /* convert surface it to RGBA image */
  SDL_Surface *rgba_surf = SDL_ConvertSurface(*surf, &fmt, SDL_SWSURFACE);
  if (rgba_surf == NULL)
  {
    return false;
  }

  SDL_FreeSurface(*surf);  // free the previous surface

  *surf = rgba_surf;  // set a pointer to converted surface

  return true;
}


/**
 */
bool CImagePointCloudRW::toGreyScale(SDL_Surface *surf)
{
  HOLOREN_ASSERT(surf != NULL);
  
  /* lock surface before accessing its pixels */
  if ((SDL_MUSTLOCK(surf)) && (SDL_LockSurface(surf) == -1))
  {
    return false;
  }

  /* convert it to grayscale */
  for (int y = 0; y < surf->h; ++y)
  {
    for (int x = 0; x < (surf->w * BYTES_PER_PIXEL); x += BYTES_PER_PIXEL)
    {
      Uint8 r = 0;
      Uint8 g = 0;
      Uint8 b = 0;
      SDL_GetRGB(GET_PIXEL32(surf, x, y), surf->format, &r, &g, &b);
      Uint8 c = 0.299f * r + 0.587f * g + 0.114f * b;
      GET_PIXEL32(surf, x, y) = SDL_MapRGB(surf->format, c, c, c);
    }
  }
  
  /* unlock surface before returning */
  if (SDL_MUSTLOCK(surf))
  {
    SDL_UnlockSurface(surf);
  }

  return true;
}


/**
 */
bool CImagePointCloudRW::toBinary(SDL_Surface *surf, uint8_t treshold)
{
  HOLOREN_ASSERT(surf != NULL);
  
  /* lock surface before accessing its pixels */
  if ((SDL_MUSTLOCK(surf)) && (SDL_LockSurface(surf) == -1))
  {
    return false;
  }

  /* binarize the surface */
  Uint32 black = SDL_MapRGB(surf->format, 0, 0, 0);
  Uint32 white = SDL_MapRGB(surf->format, 255, 255, 255);
  
  for (int y = 0; y < surf->h; ++y)
  {
    for (int x = 0; x < (surf->w * BYTES_PER_PIXEL); x += BYTES_PER_PIXEL)
    {
      if (GET_PIXEL8(surf, x, y) <= treshold)
      { // pixels that are less than treshold (closer to black) are turned black
        GET_PIXEL32(surf, x, y) = black;
      }
      else
      { // pixels with value greater than treshold (closer to white) are turned white
        GET_PIXEL32(surf, x, y) = white;
      }
    }
  }
    
  /* unlock surface before returning */
  if (SDL_MUSTLOCK(surf))
  {
    SDL_UnlockSurface(surf);
  }

  return true;
}


/**
 */
bool CImagePointCloudRW::getAvgIntensity(SDL_Surface *surf, uint8_t *intensity)
{
  HOLOREN_ASSERT(surf != NULL);
  HOLOREN_ASSERT(intensity != NULL);

  /* lock surface before accessing its pixels */
  if ((SDL_MUSTLOCK(surf)) && (SDL_LockSurface(surf) == -1))
  {
    return false;
  }

  /* compute average intensity */
  Uint32 sum = 0;

  for (int y = 0; y < surf->h; ++y)
  {
    for (int x = 0; x < (surf->w * BYTES_PER_PIXEL); x += BYTES_PER_PIXEL)
    {
      sum += GET_PIXEL8(surf, x, y);
    }
  }

  *intensity = sum / (surf->w * surf->h);

  /* unlock surface before returning */
  if (SDL_MUSTLOCK(surf))
  {
    SDL_UnlockSurface(surf);
  }

  return true;
}


/**
 */
bool CImagePointCloudRW::genPointSources(SDL_Surface *surf, uint8_t treshold, tFPType z, double sampling, CPointCloud *pc)
{
  HOLOREN_ASSERT(surf != NULL);
  HOLOREN_ASSERT(pc != NULL);
  
  /* lock the access to pixels on surface if necessary */
  if ((SDL_MUSTLOCK(surf)) && (SDL_LockSurface(surf) == -1))
  {
    return false;
  }

  /* extract point sources from all black pixels in image */
#ifdef DEBUG
  int ps_cnt = 0;
#endif

  for (int y = 0; y < surf->h; ++y)
  {
    for (int x = 0; x < (surf->w * BYTES_PER_PIXEL); x += BYTES_PER_PIXEL)
    {
      if (GET_PIXEL8(surf, x, y) <= treshold)
      {
        pc->addPointSource(SPointSource((tFPType) x * sampling, (tFPType) y * sampling, z));
#ifdef DEBUG
        ps_cnt++;
#endif
      }
    }
  }
    
  /* unlock pixels from surface */
  if (SDL_MUSTLOCK(surf))
  {
    SDL_UnlockSurface(surf);
  }

  DBG("Number of point sources added: " << ps_cnt);

  return true;
}
