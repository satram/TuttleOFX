#include "BitDepthPlugin.hpp"
#include "BitDepthDefinitions.hpp"

#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>

namespace tuttle {
namespace plugin {
namespace bitDepth {


template<class SView, class DView>
BitDepthProcess<SView, DView>::BitDepthProcess( BitDepthPlugin &instance )
: ImageGilProcessor<DView>( instance )
, _plugin( instance )
{
    _srcClip = _plugin.fetchClip( kOfxImageEffectSimpleSourceClipName );
}

template<class SView, class DView>
void BitDepthProcess<SView, DView>::setup( const OFX::RenderArguments& args )
{
	ImageGilProcessor<DView>::setup( args );

	// source view
	this->_src.reset( this->_srcClip->fetchImage( args.time ) );
	if( !this->_src.get( ) )
		throw( ImageNotReadyException( ) );
	if( this->_src->getRowBytes( ) <= 0 )
		throw( WrongRowBytesException( ) );
	this->_srcView = getView<SView>( this->_src.get(), this->_srcClip->getPixelRod(args.time) );
//	this->_srcPixelRod = this->_src->getRegionOfDefinition(); // bug in nuke, returns bounds
	this->_srcPixelRod = this->_srcClip->getPixelRod(args.time);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class SView, class DView>
void BitDepthProcess<SView, DView>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
							     procWindowRoW.y2 - procWindowRoW.y1 };

	SView src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
							   procWindowSize.x,
							   procWindowSize.y );
	DView dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
							   procWindowSize.x,
							   procWindowSize.y );
	
	copy_and_convert_pixels( src, dst );
}

}
}
}