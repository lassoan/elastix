
/*======================================================================

  This file is part of the elastix software.

  Copyright (c) University Medical Center Utrecht. All rights reserved.
  See src/CopyrightElastix.txt or http://elastix.isi.uu.nl/legal.php for
  details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE. See the above copyright notices for more information.

======================================================================*/

#ifndef ELXAFFINELOGSTACKTRANSFORM_H
#define ELXAFFINELOGSTACKTRANSFORM_H

/** Include itk transforms needed. */
#include "itkAdvancedCombinationTransform.h"
#include "itkStackTransform.h"
#include "../AffineLogTransform/itkAffineLogTransform.h"

#include "elxIncludes.h"


namespace elastix
{

/**
 * \class AffineLogStackTransform
 * \brief An affine log transform based on the itkStackTransform.
 *
 *
 * \ingroup Transforms
 */

    template < class TElastix >
    class AffineLogStackTransform :
        public itk::AdvancedCombinationTransform<
        typename elx::TransformBase<TElastix>::CoordRepType,
        elx::TransformBase<TElastix>::FixedImageDimension > ,
        public elx::TransformBase<TElastix>
    {
    public:

        /** Standard ITK-stuff. */
        typedef AffineLogStackTransform                              Self;
        typedef itk::AdvancedCombinationTransform<
            typename elx::TransformBase<TElastix>::CoordRepType,
            elx::TransformBase<TElastix>::FixedImageDimension >   Superclass1;
        typedef elx::TransformBase<TElastix>                      Superclass2;
        typedef itk::SmartPointer<Self>                           Pointer;
        typedef itk::SmartPointer<const Self>                     ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( AffineLogStackTransform, itk::AdvancedCombinationTransform );

  /** Name of this class.
   * Use this name in the parameter file to select this specific transform. \n
   * example: <tt>(Transform "AffineStackTransform")</tt>\n
   */
  elxClassNameMacro( "AffineLogStackTransform" );

  /** (Reduced) dimension of the fixed image. */
  itkStaticConstMacro( SpaceDimension, unsigned int, Superclass2::FixedImageDimension );
  itkStaticConstMacro( ReducedSpaceDimension, unsigned int, Superclass2::FixedImageDimension - 1 );

  typedef itk::AffineLogTransform< typename elx::TransformBase<TElastix>::CoordRepType,
            itkGetStaticConstMacro(SpaceDimension) >        AffineLogTransformType;
  typedef typename AffineLogTransformType::Pointer             AffineLogTransformPointer;
  typedef typename AffineLogTransformType::InputPointType   InputPointType;

  /** The ITK-class for the sub transforms, which have a reduced dimension. */
   typedef itk::AffineLogTransform<typename elx::TransformBase<TElastix>::CoordRepType,
        itkGetStaticConstMacro(ReducedSpaceDimension)>                           ReducedDimensionAffineLogTransformBaseType;
   typedef typename ReducedDimensionAffineLogTransformBaseType::Pointer            ReducedDimensionAffineLogTransformBasePointer;

  typedef typename ReducedDimensionAffineLogTransformBaseType::OutputVectorType    ReducedDimensionOutputVectorType;
  typedef typename ReducedDimensionAffineLogTransformBaseType::InputPointType      ReducedDimensionInputPointType;

  /** Typedef for stack transform. */
  typedef itk::StackTransform<
    typename elx::TransformBase<TElastix>::CoordRepType,
    itkGetStaticConstMacro( SpaceDimension ),
    itkGetStaticConstMacro( SpaceDimension ) >            AffineLogStackTransformType;
  typedef typename AffineLogStackTransformType::Pointer      AffineLogStackTransformPointer;

  /** Typedefs inherited from the superclass. */
  typedef typename Superclass1::ParametersType            ParametersType;
  typedef typename Superclass1::NumberOfParametersType    NumberOfParametersType;

  /** Typedef's from TransformBase. */
  typedef typename Superclass2::ElastixType               ElastixType;
  typedef typename Superclass2::ElastixPointer            ElastixPointer;
  typedef typename Superclass2::ConfigurationType         ConfigurationType;
  typedef typename Superclass2::ConfigurationPointer      ConfigurationPointer;
  typedef typename Superclass2::RegistrationType          RegistrationType;
  typedef typename Superclass2::RegistrationPointer       RegistrationPointer;
  typedef typename Superclass2::CoordRepType              CoordRepType;
  typedef typename Superclass2::FixedImageType            FixedImageType;
  typedef typename Superclass2::MovingImageType           MovingImageType;
  typedef typename Superclass2::ITKBaseType               ITKBaseType;
  typedef typename Superclass2::CombinationTransformType  CombinationTransformType;

  /** Reduced Dimension typedef's. */
  typedef float PixelType;
  typedef itk::Image< PixelType,
      itkGetStaticConstMacro( ReducedSpaceDimension )>       ReducedDimensionImageType;
  typedef itk::ImageRegion<
      itkGetStaticConstMacro( ReducedSpaceDimension ) >         ReducedDimensionRegionType;
  typedef typename ReducedDimensionImageType::PointType         ReducedDimensionPointType;
  typedef typename ReducedDimensionImageType::SizeType          ReducedDimensionSizeType;
  typedef typename ReducedDimensionRegionType::IndexType        ReducedDimensionIndexType;
  typedef typename ReducedDimensionImageType::SpacingType       ReducedDimensionSpacingType;
  typedef typename ReducedDimensionImageType::DirectionType     ReducedDimensionDirectionType;
  typedef typename ReducedDimensionImageType::PointType         ReducedDimensionOriginType;


  /** For scales setting in the optimizer */
  typedef typename Superclass2::ScalesType                ScalesType;

    /** Other typedef's. */
    typedef typename FixedImageType::IndexType              IndexType;
    typedef typename FixedImageType::SizeType               SizeType;
    typedef typename FixedImageType::PointType              PointType;
    typedef typename FixedImageType::SpacingType            SpacingType;
    typedef typename FixedImageType::RegionType             RegionType;
    typedef typename FixedImageType::DirectionType          DirectionType;
  typedef typename itk::ContinuousIndex< CoordRepType, ReducedSpaceDimension > ReducedDimensionContinuousIndexType;
    typedef typename itk::ContinuousIndex< CoordRepType, SpaceDimension > ContinuousIndexType;


   /** Execute stuff before anything else is done:*/

  virtual int BeforeAll( void );

  /** Execute stuff before the actual registration:
   * \li Set the stack transform parameters.
   * \li Set initial sub transforms.
   * \li Create initial registration parameters.
   */
  virtual void BeforeRegistration( void );

  /** Method initialize the parameters (to 0). */
  virtual void InitializeTransform( void );

  /** Set the scales
   * \li If AutomaticScalesEstimation is "true" estimate scales
   * \li If scales are provided by the user use those,
   * \li Otherwise use some default value
   * This function is called by BeforeRegistration, after
   * the InitializeTransform function is called
   */
  virtual void SetScales( void );

  /** Function to read transform-parameters from a file. */
  virtual void ReadFromFile( void );

  /** Function to write transform-parameters to a file. */
  virtual void WriteToFile( const ParametersType & param ) const;

protected:


  /** The constructor. */
  AffineLogStackTransform();

  /** The destructor. */
  virtual ~AffineLogStackTransform() {}

      /** Try to read the CenterOfRotation from the transform parameter file
     * This is an index value, and, thus, converted to world coordinates.
     * Transform parameter files generated by elastix version < 3.402
     * saved the center of rotation in this way.
     */
    virtual bool ReadCenterOfRotationIndex( ReducedDimensionInputPointType & rotationPoint ) const;

    /** Try to read the CenterOfRotationPoint from the transform parameter file
     * The CenterOfRotationPoint is already in world coordinates.
     * Transform parameter files generated by elastix version > 3.402
     * save the center of rotation in this way.
     */
    virtual bool ReadCenterOfRotationPoint( ReducedDimensionInputPointType & rotationPoint ) const;


private:

  /** The private constructor and copy constructor. */
  AffineLogStackTransform( const Self& );  // purposely not implemented
  void operator=( const Self& );         // purposely not implemented

  /** The Affine stack transform. */
  AffineLogStackTransformPointer  m_AffineLogStackTransform;

  /** Dummy sub transform to be used to set sub transforms of stack transform. */
  ReducedDimensionAffineLogTransformBasePointer  m_AffineLogDummySubTransform;

  /** Stack variables. */
  unsigned int m_NumberOfSubTransforms;
  double m_StackOrigin, m_StackSpacing;

  /** Initialize the affine transform. */
  unsigned int InitializeAffineLogTransform();

}; // end class AffineStackTransform


} // end namespace elastix

#ifndef ITK_MANUAL_INSTANTIATION
#include "elxAffineLogStackTransform.hxx"
#endif

#endif // ELXAFFINELOGSTACKTRANSFORM_H
