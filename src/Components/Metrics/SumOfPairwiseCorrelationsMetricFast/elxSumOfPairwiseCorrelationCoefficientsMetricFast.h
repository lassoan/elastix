/*======================================================================

  This file is part of the elastix software.

  Copyright (c) University Medical Center Utrecht. All rights reserved.
  See src/CopyrightElastix.txt or http://elastix.isi.uu.nl/legal.php for
  details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE. See the above copyright notices for more information.

======================================================================*/

#ifndef __elxSumOfPairwiseCorrelationCoefficientsMetricFast_H__
#define __elxSumOfPairwiseCorrelationCoefficientsMetricFast_H__

#include "elxIncludes.h"
#include "itkSumOfPairwiseCorrelationCoefficientsMetricFast.h"
#include "itkAdvancedBSplineDeformableTransform.h"
#include "itkStackTransform.h"

#include "elxTimer.h"

namespace elastix
{

  /** \class SumOfPairwiseCorrelationCoefficientsMetricFast
   * \brief Compute the sum of correlation coefficients between images in the 
   *  slowest varying dimension in the moving image.
   *
   *
   * This metric is based on the AdvancedImageToImageMetric.
   * It is templated over the type of the fixed and moving images to be compared.
   *
   * This metric computes the sum of variances over the slowest varying dimension in
   * the moving image. The spatial positions of the moving image are established
   * through a Transform. Pixel values are taken from the Moving image.
   *
   * This implementation is based on the AdvancedImageToImageMetric, which means that:
   * \li It uses the ImageSampler-framework
   * \li It makes use of the compact support of B-splines, in case of B-spline transforms.
   * \li Image derivatives are computed using either the B-spline interpolator's implementation
   * or by nearest neighbor interpolation of a precomputed central difference image.
   * \li A minimum number of samples that should map within the moving image (mask) can be specified.
   *
   * \parameter SampleLastDimensionRandomly: randomly sample a number of time points to
   *    to compute the variance from. When set to "false", all time points are taken into
   *    account. When set to "true", a random number of time points is selected, which can
   *    be set with parameter NumSamplesLastDimension. \n
   * \parameter NumSamplesLastDimension: the number of random samples to take in the time
   *    time direction of the data when SampleLastDimensionRandomly is set to true.
   * \parameter SubtractMean: subtract the over time computed mean parameter value from
   *    each parameter. This should be used when registration is performed directly on the moving
   *    image, without using a fixed image. Possible values are "true" or "false".
   *
   * \ingroup RegistrationMetrics
   * \ingroup Metrics
   */

  template <class TElastix >
    class SumOfPairwiseCorrelationCoefficientsMetricFast:
    public
          itk::SumOfPairwiseCorrelationCoefficientsMetricFast<
        typename MetricBase<TElastix>::FixedImageType,
        typename MetricBase<TElastix>::MovingImageType >,
    public MetricBase<TElastix>
  {
  public:

    /** Standard ITK-stuff. */
    typedef SumOfPairwiseCorrelationCoefficientsMetricFast               Self;
    typedef itk::SumOfPairwiseCorrelationCoefficientsMetricFast<
      typename MetricBase<TElastix>::FixedImageType,
      typename MetricBase<TElastix>::MovingImageType >    Superclass1;
    typedef MetricBase<TElastix>                          Superclass2;
    typedef itk::SmartPointer<Self>                       Pointer;
    typedef itk::SmartPointer<const Self>                 ConstPointer;

    /** Method for creation through the object factory. */
    itkNewMacro( Self );

    /** Run-time type information (and related methods). */
    itkTypeMacro( SumOfPairwiseCorrelationCoefficientsMetricFast, itk::SumOfPairwiseCorrelationCoefficientsMetricFast );

    /** Name of this class.
     * Use this name in the parameter file to select this specific metric. \n
     * example: <tt>(Metric "SumOfPairwiseCorrelationCoefficientsMetricFast")</tt>\n
     */
    elxClassNameMacro( "SumOfPairwiseCorrelationCoefficientsMetricFast" );

    /** Typedefs from the superclass. */
    typedef typename
      Superclass1::CoordinateRepresentationType              CoordinateRepresentationType;
    typedef typename Superclass1::ScalarType                 ScalarType;
    typedef typename Superclass1::MovingImageType            MovingImageType;
    typedef typename Superclass1::MovingImagePixelType       MovingImagePixelType;
    typedef typename Superclass1::MovingImageConstPointer    MovingImageConstPointer;
    typedef typename Superclass1::FixedImageType             FixedImageType;
    typedef typename Superclass1::FixedImageConstPointer     FixedImageConstPointer;
    typedef typename Superclass1::FixedImageRegionType       FixedImageRegionType;
    typedef typename Superclass1::FixedImageSizeType         FixedImageSizeType;
    typedef typename Superclass1::TransformType              TransformType;
    typedef typename Superclass1::TransformPointer           TransformPointer;
    typedef typename Superclass1::InputPointType             InputPointType;
    typedef typename Superclass1::OutputPointType            OutputPointType;
    typedef typename Superclass1::TransformParametersType    TransformParametersType;
    typedef typename Superclass1::TransformJacobianType      TransformJacobianType;
    typedef typename Superclass1::InterpolatorType           InterpolatorType;
    typedef typename Superclass1::InterpolatorPointer        InterpolatorPointer;
    typedef typename Superclass1::RealType                   RealType;
    typedef typename Superclass1::GradientPixelType          GradientPixelType;
    typedef typename Superclass1::GradientImageType          GradientImageType;
    typedef typename Superclass1::GradientImagePointer       GradientImagePointer;
    typedef typename Superclass1::GradientImageFilterType    GradientImageFilterType;
    typedef typename Superclass1::GradientImageFilterPointer GradientImageFilterPointer;
    typedef typename Superclass1::FixedImageMaskType         FixedImageMaskType;
    typedef typename Superclass1::FixedImageMaskPointer      FixedImageMaskPointer;
    typedef typename Superclass1::MovingImageMaskType        MovingImageMaskType;
    typedef typename Superclass1::MovingImageMaskPointer     MovingImageMaskPointer;
    typedef typename Superclass1::MeasureType                MeasureType;
    typedef typename Superclass1::DerivativeType             DerivativeType;
    typedef typename Superclass1::ParametersType             ParametersType;
    typedef typename Superclass1::FixedImagePixelType        FixedImagePixelType;
    typedef typename Superclass1::MovingImageRegionType      MovingImageRegionType;
    typedef typename Superclass1::ImageSamplerType           ImageSamplerType;
    typedef typename Superclass1::ImageSamplerPointer        ImageSamplerPointer;
    typedef typename Superclass1::ImageSampleContainerType   ImageSampleContainerType;
    typedef typename
      Superclass1::ImageSampleContainerPointer               ImageSampleContainerPointer;
    typedef typename Superclass1::FixedImageLimiterType      FixedImageLimiterType;
    typedef typename Superclass1::MovingImageLimiterType     MovingImageLimiterType;
    typedef typename
      Superclass1::FixedImageLimiterOutputType               FixedImageLimiterOutputType;
    typedef typename
      Superclass1::MovingImageLimiterOutputType              MovingImageLimiterOutputType;
    typedef typename
      Superclass1::MovingImageDerivativeScalesType           MovingImageDerivativeScalesType;

    /** The fixed image dimension. */
    itkStaticConstMacro( FixedImageDimension, unsigned int,
      FixedImageType::ImageDimension );

    /** The moving image dimension. */
    itkStaticConstMacro( MovingImageDimension, unsigned int,
      MovingImageType::ImageDimension );

    /** Typedef's inherited from Elastix. */
    typedef typename Superclass2::ElastixType               ElastixType;
    typedef typename Superclass2::ElastixPointer            ElastixPointer;
    typedef typename Superclass2::ConfigurationType         ConfigurationType;
    typedef typename Superclass2::ConfigurationPointer      ConfigurationPointer;
    typedef typename Superclass2::RegistrationType          RegistrationType;
    typedef typename Superclass2::RegistrationPointer       RegistrationPointer;
    typedef typename Superclass2::ITKBaseType               ITKBaseType;

    /** Typedef's for the B-spline transform. */
    typedef itk::AdvancedBSplineDeformableTransformBase<
      ScalarType, FixedImageDimension >                       BSplineTransformBaseType;
    typedef itk::AdvancedCombinationTransform<
      ScalarType, FixedImageDimension >                       CombinationTransformType;
    typedef itk::StackTransform<
      ScalarType, FixedImageDimension, MovingImageDimension > StackTransformType;
    typedef itk::AdvancedBSplineDeformableTransformBase<
      ScalarType, FixedImageDimension - 1 >                   ReducedDimensionBSplineTransformBaseType;

    /** Typedef for timer. */
    typedef tmr::Timer          TimerType;
    /** Typedef for timer. */
    typedef TimerType::Pointer  TimerPointer;

    /** Sets up a timer to measure the initialisation time and
     * calls the Superclass' implementation.
     */

    virtual void Initialize(void) throw (itk::ExceptionObject);

    /**
     * Do some things before each resolution:
     * \li Set CheckNumberOfSamples setting
     * \li Set UseNormalization setting
     */
    virtual void BeforeEachResolution(void);
    virtual void AfterEachIteration(void);

  protected:

    /** The constructor. */
    SumOfPairwiseCorrelationCoefficientsMetricFast(){};

    /** The destructor. */
    virtual ~SumOfPairwiseCorrelationCoefficientsMetricFast() {}

  private:

    /** The private constructor. */
    SumOfPairwiseCorrelationCoefficientsMetricFast( const Self& ); // purposely not implemented

    /** The private copy constructor. */
    void operator=( const Self& );              // purposely not implemented

  }; // end class SumOfPairwiseCorrelationCoefficientsMetricFast

} // end namespace elastix


#ifndef ITK_MANUAL_INSTANTIATION
#include "elxSumOfPairwiseCorrelationCoefficientsMetricFast.hxx"
#endif

#endif // end #ifndef __elxSumOfPairwiseCorrelationCoefficientsMetricFast_H__

