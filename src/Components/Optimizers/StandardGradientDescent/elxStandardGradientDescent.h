#ifndef __elxStandardGradientDescent_h
#define __elxStandardGradientDescent_h

#include "itkStandardGradientDescentOptimizer.h"
#include "elxIncludes.h"

namespace elastix
{
using namespace itk;


	/**
	 * **************** StandardGradientDescent ******************
	 *
	 * The StandardGradientDescent class ....
	 */

	template <class TElastix>
		class StandardGradientDescent :
		public
			itk::StandardGradientDescentOptimizer,
		public
			OptimizerBase<TElastix>
	{
	public:

		/** Standard ITK.*/
		typedef StandardGradientDescent									Self;
		typedef StandardGradientDescentOptimizer	Superclass1;
		typedef OptimizerBase<TElastix>											Superclass2;
		typedef SmartPointer<Self>									Pointer;
		typedef SmartPointer<const Self>						ConstPointer;
		
		/** Method for creation through the object factory. */
		itkNewMacro( Self );
		
		/** Run-time type information (and related methods). */
		itkTypeMacro( StandardGradientDescent, StandardGradientDescentOptimizer );
		
		/** Name of this class.*/
		elxClassNameMacro( "StandardGradientDescent" );

		/** Typedef's inherited from Superclass1.*/
	  typedef Superclass1::CostFunctionType			CostFunctionType;
		typedef Superclass1::CostFunctionPointer	CostFunctionPointer;
		typedef Superclass1::StopConditionType		StopConditionType;
		

		/** Typedef's inherited from Elastix.*/
		typedef typename Superclass2::ElastixType						ElastixType;
		typedef typename Superclass2::ElastixPointer				ElastixPointer;
		typedef typename Superclass2::ConfigurationType			ConfigurationType;
		typedef typename Superclass2::ConfigurationPointer	ConfigurationPointer;
		typedef typename Superclass2::RegistrationType			RegistrationType;
		typedef typename Superclass2::RegistrationPointer		RegistrationPointer;
		typedef typename Superclass2::ITKBaseType						ITKBaseType;
		
		/** Methods that have to be present everywhere.*/
		virtual void BeforeRegistration(void);
		virtual void BeforeEachResolution(void);
		virtual void AfterEachResolution(void);
		virtual void AfterEachIteration(void);
		virtual void AfterRegistration(void);		
		
		/** Override the SetInitialPosition.*/
		virtual void SetInitialPosition( const ParametersType & param );
		
	protected:

		  StandardGradientDescent();
			virtual ~StandardGradientDescent() {};
				
	private:

		  StandardGradientDescent( const Self& );	// purposely not implemented
			void operator=( const Self& );							// purposely not implemented
			
	}; // end class StandardGradientDescent
	

} // end namespace elastix

#ifndef ITK_MANUAL_INSTANTIATION
#include "elxStandardGradientDescent.hxx"
#endif

#endif // end #ifndef __elxStandardGradientDescent_h
