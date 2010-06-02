#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <boost/gil/extension/numeric/convolve.hpp>

#include <boost/units/pow.hpp>
#include <boost/lambda/core.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/algorithm.hpp>

#include <cmath>
#include <vector>
#include <algorithm>

#include "BlurPlugin.hpp"


namespace tuttle {
namespace plugin {
namespace blur {

template<class View>
BlurProcess<View>::BlurProcess( BlurPlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
	this->setNoMultiThreading();
}

/**
 * @brief gaussian function
 * @f[
 * G(x) = \frac{1}{\sqrt{2\pi \sigma^2}} e^{-\frac{x^2}{2 \sigma^2}}
 * @f]
 * or simplified...
 * @f[
 * G(x) = a e^{- { \frac{(x-b)^2 }{ 2 c^2} } }
 * @f]
 *
 * @return y <- gauss(x)
 */
template<class View>
typename BlurProcess<View>::Scalar BlurProcess<View>::gaussianValueAt( const Scalar x, const Scalar amplitude, const Scalar yscale, const Scalar xcenter )
{
	namespace bu = boost::units;
	return yscale * std::exp( -bu::pow<2>(x-xcenter) / (2.0*bu::pow<2>(amplitude) ) );
}

/**
 * @brief create the gaussian kernel
 * * found the kernel size
 * * fill kernel values
 */
template<class View>
boost::gil::kernel_1d<typename BlurProcess<View>::Scalar> BlurProcess<View>::buildGaussian1DKernel( const Scalar size )
{
	using namespace boost::lambda;
	std::vector<Scalar> rightKernel;
	rightKernel.reserve(10);
	int x = 1;
	Scalar v;
	Scalar sum = 0.0;
	while( (v = gaussianValueAt( x, size )) > _convolutionEpsilon )
	{
		rightKernel.push_back( v );
		sum += v;
		++x;
	}
	if( rightKernel.size() == 0 || sum == 0 )
		return boost::gil::kernel_1d<Scalar>();
	sum *= 2.0;

	std::vector<Scalar> kernel( rightKernel.size()*2+1 );
	const Scalar kernelCenter = gaussianValueAt( 0, size );
	sum += kernelCenter;

	std::for_each(rightKernel.begin(), rightKernel.end(), _1 /= sum );

	kernel[rightKernel.size()] = kernelCenter / sum; // kernel center to 0
	std::copy( rightKernel.rbegin(), rightKernel.rend(), kernel.begin() );
	std::copy( rightKernel.begin(), rightKernel.end(), kernel.begin()+rightKernel.size()+1 );

	COUT_VAR( rightKernel.size() );
	COUT_VAR( kernel.size() );
	std::cout << "[";
	std::for_each(rightKernel.begin(), rightKernel.end(), std::cout << _1 << ',');
	std::cout << "]" << std::endl;
	std::cout << "[";
	std::for_each(kernel.begin(), kernel.end(), std::cout << _1 << ',');
	std::cout << "]" << std::endl;
	return boost::gil::kernel_1d<Scalar>( &(kernel[0]), kernel.size(), rightKernel.size() );
}

template <class View>
void BlurProcess<View>::setup( const OFX::RenderArguments& args )
{
	COUT_INFOS;
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams();

	COUT_X(80, "X");
	_gilKernelX = buildGaussian1DKernel( _params._size.x );
	COUT_X(80, "Y");
	_gilKernelY = buildGaussian1DKernel( _params._size.y );
	
	COUT_VAR( _params._size );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void BlurProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
							     procWindowRoW.y2 - procWindowRoW.y1 };

	View src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
							  procWindowSize.x, procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
							  procWindowSize.x, procWindowSize.y );
	View& linkSrc = src;

	convolve_boundary_option option = convolve_option_extend_mirror;
	switch( _params._border )
	{
		case eBorderExtendMirror:
			option = convolve_option_extend_mirror;
			break;
		case eBorderExtendConstant:
			option = convolve_option_extend_constant;
			break;
		case eBorderExtendBlack:
			option = convolve_option_extend_zero;
			break;
		case eBorderOutputBlack:
			option = convolve_option_output_zero;
			break;
	}

	if( _gilKernelX.size() > 2 )
	{
		COUT_X(10, "_row_");
		convolve_rows<Pixel>( linkSrc, _gilKernelX, dst, option );
		linkSrc = dst;
	}
	if( _gilKernelY.size() > 2 )
	{
		COUT_X(10, "_cols_");
		convolve_cols<Pixel>( linkSrc, _gilKernelY, dst, option );
		linkSrc = dst;
	}
}

}
}
}