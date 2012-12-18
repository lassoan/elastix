/*======================================================================

  This file is part of the elastix software.

  Copyright (c) University Medical Center Utrecht. All rights reserved.
  See src/CopyrightElastix.txt or http://elastix.isi.uu.nl/legal.php for
  details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE. See the above copyright notices for more information.

======================================================================*/
#ifndef __itkGPUCastImageFilter_hxx
#define __itkGPUCastImageFilter_hxx

#include "itkGPUCastImageFilter.h"

namespace itk
{
/**
 * Constructor
 */
template< class TInputImage, class TOutputImage >
GPUCastImageFilter< TInputImage, TOutputImage >
::GPUCastImageFilter()
{
  std::ostringstream defines;

  if ( TInputImage::ImageDimension > 3 || TInputImage::ImageDimension < 1 )
  {
    itkExceptionMacro( "GPUCastImageFilter supports 1/2/3D image." );
  }

  defines << "#define DIM_" << TInputImage::ImageDimension << "\n";
  defines << "#define INPIXELTYPE ";
  GetTypenameInString( typeid( typename TInputImage::PixelType ), defines );
  defines << "#define OUTPIXELTYPE ";
  GetTypenameInString( typeid( typename TOutputImage::PixelType ), defines );

  // OpenCL kernel source
  const char *GPUSource = GPUCastImageFilterKernel::GetOpenCLSource();
  // Load and create kernel
  const bool loaded = this->m_GPUKernelManager->LoadProgramFromString( GPUSource, defines.str().c_str() );
  if ( loaded )
  {
    this->m_UnaryFunctorImageFilterGPUKernelHandle = this->m_GPUKernelManager->CreateKernel( "CastImageFilter" );
  }
  else
  {
    itkExceptionMacro( << "Kernel has not been loaded from string:\n" << GPUSource );
  }
}

//------------------------------------------------------------------------------
template< class TInputImage, class TOutputImage >
void GPUCastImageFilter< TInputImage, TOutputImage >::GPUGenerateData()
{
  itkDebugMacro(<< "Calling GPUCastImageFilter::GPUGenerateData()");
  GPUSuperclass::GPUGenerateData();
  itkDebugMacro(<< "GPUCastImageFilter::GPUGenerateData() finished");
}
} // end of namespace itk

#endif /* __itkGPUCastImageFilter_hxx */
