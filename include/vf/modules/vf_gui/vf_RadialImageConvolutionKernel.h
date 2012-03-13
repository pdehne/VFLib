// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_RADIALIMAGECONVOLUTIONKERNEL_VFHEADER__
#define __VF_RADIALIMAGECONVOLUTIONKERNEL_VFHEADER__

// Takes advantage of radial symmetry to implement an efficient image convolution
class RadialImageConvolutionKernel
{
public:
  // This includes the center sample
  explicit RadialImageConvolutionKernel (int radiusInSamples);
  ~RadialImageConvolutionKernel ();

  int getRadiusInSamples () { return m_radius; }

  // the gaussian radius will be (radiusInSamples-1)/2
  void createGaussianBlur ();

  void setOverallSum (float desiredTotalSum);

  void rescaleAllValues (float multiplier);
  
  // creates a convolved image with the same dimensions as the source
  VF_JUCE::Image createConvolvedImage (VF_JUCE::Image const& sourceImage) const;

  // creates a larger image that fully contains the result
  // of applying the convolution kernel to the source image.
  VF_JUCE::Image createConvolvedImageFull (VF_JUCE::Image const& sourceImage) const;

  //
  // copy a line of color components, performing edge
  // replication on either side. dest must have 2 * replicas
  // more components than src.
  //
  static void copy (int pixels,
                    uint8* dest,
                    int destSkip,
                    const uint8* src,
                    int srcSkip,
                    int replicas);

  //
  // copy a line of alpha values, inserting fully transparent
  // values (0) as replicas on either side
  //
  static void copy_alpha (int pixels,
                          uint8* dest,
                          int destSkip,
                          const uint8* src,
                          int srcSkip,
                          int replicas);

  //
  // convolve a line of color components.
  // src must have 2 * (radius - 1) more components than dest
  //
  static void convolve (int pixels,
                        uint8* dest,
                        int destSkip,
                        uint8 const* src,
                        int srcSkip,
                        int radius,
                        float const* kernel);

private:
  const int m_radius;
  VF_JUCE::HeapBlock <float> m_kernel;
};

#endif

