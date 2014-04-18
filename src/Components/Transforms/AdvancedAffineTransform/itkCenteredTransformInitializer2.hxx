/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkCenteredTransformInitializer2.txx,v $
  Language:  C++
  Date:      $Date: 2009-08-15 23:42:49 $
  Version:   $Revision: 1.22 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkCenteredTransformInitializer2_hxx
#define __itkCenteredTransformInitializer2_hxx

#include "itkCenteredTransformInitializer2.h"
#include "itkImageMaskSpatialObject.h"

namespace itk
{

template< class TTransform, class TFixedImage, class TMovingImage >
CenteredTransformInitializer2< TTransform, TFixedImage, TMovingImage >
::CenteredTransformInitializer2()
{
  m_FixedCalculator  = FixedImageCalculatorType::New();
  m_MovingCalculator = MovingImageCalculatorType::New();
  m_UseMoments       = false;
  m_UseOrigins       = false;
  m_UseTop           = false;
}


/** Initialize the transform using data from the images */
template< class TTransform, class TFixedImage, class TMovingImage >
void
CenteredTransformInitializer2< TTransform, TFixedImage, TMovingImage >
::InitializeTransform()
{
  // Sanity check
  if( !m_FixedImage )
  {
    itkExceptionMacro( "Fixed Image has not been set" );
    return;
  }
  if( !m_MovingImage )
  {
    itkExceptionMacro( "Moving Image has not been set" );
    return;
  }
  if( !m_Transform )
  {
    itkExceptionMacro( "Transform has not been set" );
    return;
  }

  // If images come from filters, then update those filters.
  if( m_FixedImage->GetSource() )
  {
    m_FixedImage->GetSource()->Update();
  }
  if( m_MovingImage->GetSource() )
  {
    m_MovingImage->GetSource()->Update();
  }

  InputPointType   rotationCenter;
  OutputVectorType translationVector;

  typedef ImageMaskSpatialObject< InputSpaceDimension >  FixedMaskSpatialObjectType;
  typedef ImageMaskSpatialObject< OutputSpaceDimension > MovingMaskSpatialObjectType;

  if( m_UseMoments )
  {
    // Convert the masks to spatial objects
    typename FixedMaskSpatialObjectType::Pointer fixedMaskAsSpatialObject = 0;
    if( this->m_FixedImageMask )
    {
      fixedMaskAsSpatialObject = FixedMaskSpatialObjectType::New();
      fixedMaskAsSpatialObject->SetImage( this->m_FixedImageMask );
    }

    typename MovingMaskSpatialObjectType::Pointer movingMaskAsSpatialObject = 0;
    if( this->m_MovingImageMask )
    {
      movingMaskAsSpatialObject = MovingMaskSpatialObjectType::New();
      movingMaskAsSpatialObject->SetImage( this->m_MovingImageMask );
    }

    // Moments
    m_FixedCalculator->SetImage(  m_FixedImage );
    m_FixedCalculator->SetSpatialObjectMask( fixedMaskAsSpatialObject );
    m_FixedCalculator->Compute();

    m_MovingCalculator->SetImage( m_MovingImage );
    m_MovingCalculator->SetSpatialObjectMask( movingMaskAsSpatialObject );
    m_MovingCalculator->Compute();

    typename FixedImageCalculatorType::VectorType fixedCenter = m_FixedCalculator->GetCenterOfGravity();

    typename MovingImageCalculatorType::VectorType movingCenter = m_MovingCalculator->GetCenterOfGravity();

    for( unsigned int i = 0; i < InputSpaceDimension; i++ )
    {
      rotationCenter[ i ]    = fixedCenter[ i ];
      translationVector[ i ] = movingCenter[ i ] - fixedCenter[ i ];
    }
  }
  else if( m_UseOrigins )
  {
    // Origins
    const typename MovingImageType::RegionType & movingRegion = m_MovingImage->GetLargestPossibleRegion();
    const typename MovingImageType::IndexType & movingIndex   = movingRegion.GetIndex();
    const typename MovingImageType::SizeType & movingSize     = movingRegion.GetSize();

    typedef typename InputPointType::ValueType                   CoordRepType;
    typedef ContinuousIndex< CoordRepType, InputSpaceDimension > ContinuousIndexType;

    typedef typename ContinuousIndexType::ValueType ContinuousIndexValueType;

    InputPointType      centerMovingPoint;
    ContinuousIndexType centerMovingIndex;

    for( unsigned int m = 0; m < InputSpaceDimension; m++ )
    {
      centerMovingIndex[ m ] =
        static_cast< ContinuousIndexValueType >( movingIndex[ m ] ) +
        static_cast< ContinuousIndexValueType >( movingSize[ m ] - 1 ) / 2.0;
    }

    m_MovingImage->TransformContinuousIndexToPhysicalPoint(
      centerMovingIndex, centerMovingPoint );

    // Origins points
    InputPointType originMovingPoint;
    m_MovingImage->TransformIndexToPhysicalPoint(
      movingIndex, originMovingPoint );

    const typename FixedImageType::RegionType & fixedRegion = m_FixedImage->GetLargestPossibleRegion();
    const typename FixedImageType::IndexType & fixedIndex   = fixedRegion.GetIndex();
    InputPointType originFixedPoint;
    m_FixedImage->TransformIndexToPhysicalPoint(
      fixedIndex, originFixedPoint );

    for( unsigned int i = 0; i < InputSpaceDimension; i++ )
    {
      translationVector[ i ] = originMovingPoint[ i ] - originFixedPoint[ i ];
      rotationCenter[ i ]    = centerMovingPoint[ i ] - translationVector[ i ];
    }

  }
  else if ( m_UseTop )
  {
    // Align the geometrical tops (z-direction) of the fixed and moving image.
    // When masks are used the geometrical tops (z-direction) of the bounding box
    // of the masks are used. Center of rotation point is set to the center of the fixed image.
    // Get fixed image (mask) information

    // Create eight corner points in world coordinates for both fixed and moving image.
    WorldCornersType fixedCornersWorld  = 
      this->GetWorldCorners<FixedImageType, FixedImageMaskType>( m_FixedImage, m_FixedImageMask );

    WorldCornersType movingCornersWorld = 
      this->GetWorldCorners<MovingImageType, MovingImageMaskType>( m_MovingImage, m_MovingImageMask );

    typename TransformType::InputPointType minWorldFixed, maxWorldFixed, minWorldMoving, maxWorldMoving;
    // Transform eight corner points to world coordinates and determine min and max coordinate values.
    for ( size_t i = 0; i < fixedCornersWorld.size(); ++i ) 
    {
      typename TransformType::InputPointType worldFixed, worldMoving;
      worldFixed = fixedCornersWorld[i];
      worldMoving = movingCornersWorld[i];
      if ( i == 0 )
      {
        minWorldFixed = worldFixed;
        maxWorldFixed = worldFixed;
        minWorldMoving = worldMoving;
        maxWorldMoving = worldMoving;
      }
      else
      {
        for ( size_t k = 0; k < InputSpaceDimension; ++k )
        {
          if ( worldFixed[ k ] < minWorldFixed[ k ] )   minWorldFixed[ k ]  = worldFixed[ k ];
          if ( worldFixed[ k ] > maxWorldFixed[ k ] )   maxWorldFixed[ k ]  = worldFixed[ k ];
          if ( worldMoving[ k ] < minWorldMoving[ k ] ) minWorldMoving[ k ] = worldMoving[ k ];
          if ( worldMoving[ k ] > maxWorldMoving[ k ] ) maxWorldMoving[ k ] = worldMoving[ k ];
        }
      }
    }

    // Calculate topCenterFixed coordinate.
    typename TransformType::InputPointType topCenterFixed;
    topCenterFixed[ 0 ] = ( maxWorldFixed[ 0 ] + minWorldFixed[ 0 ] ) / 2.0;
    topCenterFixed[ 1 ] = ( maxWorldFixed[ 1 ] + minWorldFixed[ 1 ] ) / 2.0;
    topCenterFixed[ 2 ] = maxWorldFixed[ 2 ];

    // Calculate topCentermoving coordinate.
    typename TransformType::InputPointType topCenterMoving;
    topCenterMoving[ 0 ] = ( maxWorldMoving[ 0 ] + minWorldMoving[ 0 ] ) / 2.0;
    topCenterMoving[ 1 ] = ( maxWorldMoving[ 1 ] + minWorldMoving[ 1 ] ) / 2.0;
    topCenterMoving[ 2 ] = maxWorldMoving[ 2 ];

    // Compute the difference between the centers
    for( unsigned int i = 0; i < InputSpaceDimension; i++ )
    {
      rotationCenter[ i ]    = ( maxWorldFixed[ i ] + minWorldFixed[ i ] ) / 2.0;
      translationVector[ i ] = topCenterMoving[ i ] - topCenterFixed[ i ];
    }
  }
  else if ( m_UseTopSliceCenter )
  {
    // Get eight corner points in world coordinates.
    WorldCornersType fixedCornersWorld  = 
        this->GetWorldCorners<FixedImageType, FixedImageMaskType>( m_FixedImage, m_FixedImageMask );
    WorldCornersType movingCornersWorld = 
      this->GetWorldCorners<MovingImageType, MovingImageMaskType>( m_MovingImage, m_MovingImageMask );

    // Sort corners based on their z coordinate.
    std::sort(fixedCornersWorld.begin(),  fixedCornersWorld.end(), 
        &CenteredTransformInitializer2::MaxZCoordinate);
    std::sort(movingCornersWorld.begin(), movingCornersWorld.end(), 
        &CenteredTransformInitializer2::MaxZCoordinate);

    const double weights[4] = {0.25, 0.25, 0.25};
    typename TransformType::InputPointType fixedTopCenter;
    fixedTopCenter.SetToBarycentricCombination( &fixedCornersWorld[4], weights, 4 );

    typename TransformType::InputPointType movingTopCenter;
    movingTopCenter.SetToBarycentricCombination( &movingCornersWorld[4], weights, 4 );

    typename TransformType::InputPointType fixedBottomCenter;
    fixedBottomCenter.SetToBarycentricCombination( &fixedCornersWorld[0], weights, 4 );

    typename TransformType::InputPointType movingBottomCenter;
    movingBottomCenter.SetToBarycentricCombination( &movingCornersWorld[0], weights, 4 );

    // Compute the difference between the centers
    rotationCenter.SetToBarycentricCombination( fixedBottomCenter, fixedTopCenter, 0.5 );
    translationVector = movingTopCenter - fixedTopCenter;
  }
  else
  {
    // Align the geometrical centers of the fixed and moving image.
    // When masks are used the geometrical centers of the bounding box
    // of the masks are used.
    // Get fixed image (mask) information
    typedef typename FixedImageType::RegionType FixedRegionType;
    FixedRegionType fixedRegion = this->m_FixedImage->GetLargestPossibleRegion();
    if( this->m_FixedImageMask )
    {
      typename FixedMaskSpatialObjectType::Pointer fixedMaskAsSpatialObject
        = FixedMaskSpatialObjectType::New();
      fixedMaskAsSpatialObject->SetImage( this->m_FixedImageMask );
      fixedRegion = fixedMaskAsSpatialObject->GetAxisAlignedBoundingBoxRegion();
    }

    // Compute center of the fixed image (mask bounding box) in physical units
    ContinuousIndex< double, InputSpaceDimension > fixedCenterCI;
    for( unsigned int k = 0; k < InputSpaceDimension; k++ )
    {
      fixedCenterCI[ k ] = fixedRegion.GetIndex()[ k ]
        + ( fixedRegion.GetSize()[ k ] - 1.0 ) / 2.0;
    }
    typename TransformType::InputPointType centerFixed;
    this->m_FixedImage->TransformContinuousIndexToPhysicalPoint(
      fixedCenterCI, centerFixed );

    // Get moving image (mask) information
    typedef typename MovingImageType::RegionType MovingRegionType;
    MovingRegionType movingRegion = this->m_MovingImage->GetLargestPossibleRegion();
    if( this->m_MovingImageMask )
    {
      typename MovingMaskSpatialObjectType::Pointer movingMaskAsSpatialObject
        = MovingMaskSpatialObjectType::New();
      movingMaskAsSpatialObject->SetImage( this->m_MovingImageMask );
      movingRegion = movingMaskAsSpatialObject->GetAxisAlignedBoundingBoxRegion();
    }

    // Compute center of the moving image (mask bounding box) in physical units
    ContinuousIndex< double, InputSpaceDimension > movingCenterCI;
    for( unsigned int k = 0; k < InputSpaceDimension; k++ )
    {
      movingCenterCI[ k ] = movingRegion.GetIndex()[ k ]
        + ( movingRegion.GetSize()[ k ] - 1.0 ) / 2.0;
    }
    typename TransformType::InputPointType centerMoving;
    this->m_MovingImage->TransformContinuousIndexToPhysicalPoint(
      movingCenterCI, centerMoving );

    // Compute the difference between the centers
    for( unsigned int i = 0; i < InputSpaceDimension; i++ )
    {
      rotationCenter[ i ]    = centerFixed[ i ];
      translationVector[ i ] = centerMoving[ i ] - centerFixed[ i ];
    }

  }

  // Set the initialization
  m_Transform->SetCenter( rotationCenter );
  m_Transform->SetTranslation( translationVector );
}

template< class TTransform, class TFixedImage, class TMovingImage >
template< typename TCornerImage, typename TMaskImage>
typename CenteredTransformInitializer2< TTransform, TFixedImage, TMovingImage >::WorldCornersType
CenteredTransformInitializer2< TTransform, TFixedImage, TMovingImage >
::GetWorldCorners(typename TCornerImage::ConstPointer image, typename TMaskImage::ConstPointer mask)
{
  typedef typename TCornerImage::RegionType RegionType;
  RegionType region;
  if ( mask )
  {
    typedef ImageMaskSpatialObject< InputSpaceDimension >  MaskSpatialObjectType;
    typename MaskSpatialObjectType::Pointer maskAsSpatialObject = MaskSpatialObjectType::New();
    maskAsSpatialObject->SetImage( mask );
    region = maskAsSpatialObject->GetAxisAlignedBoundingBoxRegion();
  } 
  else 
  {
    region = image->GetLargestPossibleRegion();
  }

  std::vector< ContinuousIndex< double, InputSpaceDimension > > corners(8);
  for ( unsigned int z = 0, index = 0; z < 2; ++z )
  {
    for ( unsigned int y = 0; y < 2; ++y )
    {
      for ( unsigned int x = 0; x < 2; ++x, ++index )
      {
        corners[ index ][ 0 ] = region.GetIndex()[ 0 ] + x * ( region.GetSize()[ 0 ] - 1);
        corners[ index ][ 1 ] = region.GetIndex()[ 1 ] + y * ( region.GetSize()[ 1 ] - 1);
        if (InputSpaceDimension > 2) 
        {
          corners[ index ][ 2 ] = region.GetIndex()[ 2 ] + z * ( region.GetSize()[ 2 ] - 1);
        }
      }
    }
  }

  // Transform eight corner points to world coordinates.
  WorldCornersType cornersWorld(corners.size());
  for ( std::size_t i = 0; i < corners.size(); ++i ) 
  {
    image->TransformContinuousIndexToPhysicalPoint( corners[i], cornersWorld[i] );
  }

  return cornersWorld;
}

template< class TTransform, class TFixedImage, class TMovingImage >
bool
CenteredTransformInitializer2< TTransform, TFixedImage, TMovingImage >
::MaxZCoordinate(const typename TransformType::InputPointType& coordinate0, 
                 const typename TransformType::InputPointType& coordinate1)
{
  return coordinate0[2] < coordinate1[2];
}

template< class TTransform, class TFixedImage, class TMovingImage >
void
CenteredTransformInitializer2< TTransform, TFixedImage, TMovingImage >
::PrintSelf( std::ostream & os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );

  os << indent << "Transform   = " << std::endl;
  if( m_Transform )
  {
    os << indent << m_Transform  << std::endl;
  }
  else
  {
    os << indent << "None" << std::endl;
  }

  os << indent << "FixedImage   = " << std::endl;
  if( m_FixedImage )
  {
    os << indent << m_FixedImage  << std::endl;
  }
  else
  {
    os << indent << "None" << std::endl;
  }

  os << indent << "MovingImage   = " << std::endl;
  if( m_MovingImage )
  {
    os << indent << m_MovingImage  << std::endl;
  }
  else
  {
    os << indent << "None" << std::endl;
  }

  os << indent << "MovingMomentCalculator   = " << std::endl;
  if( m_UseMoments && m_MovingCalculator )
  {
    os << indent << m_MovingCalculator  << std::endl;
  }
  else if( m_UseOrigins && m_MovingCalculator )
  {
    os << indent << m_MovingCalculator  << std::endl;
  }
  else
  {
    os << indent << "None" << std::endl;
  }

  os << indent << "FixedMomentCalculator   = " << std::endl;
  if( m_UseMoments && m_FixedCalculator )
  {
    os << indent << m_FixedCalculator  << std::endl;
  }
  else if( m_UseOrigins && m_FixedCalculator )
  {
    os << indent << m_FixedCalculator  << std::endl;
  }
  else
  {
    os << indent << "None" << std::endl;
  }

}


}  // namespace itk

#endif /* __itkCenteredTransformInitializer2_hxx */