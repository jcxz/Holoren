/**
 * A common renderer interface
 */

#ifndef IRENDERER_H
#define IRENDERER_H

#include <string>


class CPointCloud;
class COpticalField;

/**
 * This class defines an interface that all renderers should comply to
 */
struct IRenderer
{
  /**
   * Destructor
   */
  virtual ~IRenderer(void) { }

  /**
   * A method to initialize renderer
   * E.g to query platform, devices, create an OpenCL/CUDA context
   * compile kernels ... and do all the oprations that are necessary
   * to be executed only once before rendering.
   * If this method would require any arguments, that can be provided
   * as setters in the derived class
   *
   * @return true of the method succeeded, false otherwise
   */
  virtual bool open(void) = 0;

  /**
   * A method to deinitialize renderer
   * This method is intended for all one time operations that have to
   * be performed after all rendering is done
   *
   * @return true of operation succeede, false otherwise
   */
  virtual bool close(void) = 0;

  /**
   * A method to return a string describing the error
   * that occured whiel rendering or intialising the renderer
   *
   * @retrun a string describing the error that happened,
   *         e.g. a build log or an error message from OpenCL/CUDA
   */
  virtual std::string getError(void) const = 0;

  /**
   * A method to render only an object wave
   *
   * @param scene the scene that will be rendered
   * @param of an optical field where the rendering results will be saved
   *
   * @return true of the operation succeeded, false otherwise
   */
  virtual bool renderObjectWave(const CPointCloud & pc, COpticalField *of) = 0;

  /**
   * A method to render the comple hologram including adding the reference wave
   *
   * @param scene the scene that will be rendered
   * @param of an optical field where the rendering results will be saved
   *
   * @return true of the operation succeeded, false otherwise
   */
  virtual bool renderHologram(const CPointCloud & pc, COpticalField *of) = 0;
};

#endif
