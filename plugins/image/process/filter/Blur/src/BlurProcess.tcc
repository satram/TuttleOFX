#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/PluginException.hpp>

namespace tuttle {
namespace plugin {
namespace blur {

template<class View>
BlurProcess<View>::BlurProcess( BlurPlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
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
	
	for( int y = procWindowOutput.y1;
			 y < procWindowOutput.y2;
			 ++y )
	{
		typename View::x_iterator src_it = this->_srcView.x_at( procWindowOutput.x1, y );
		typename View::x_iterator dst_it = this->_dstView.x_at( procWindowOutput.x1, y );
		for( int x = procWindowOutput.x1;
			 x < procWindowOutput.x2;
			 ++x, ++src_it, ++dst_it )
		{
			(*dst_it) = (*src_it);
		}
		if( this->progressForward() )
			return;
	}
	/*
	OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
							     procWindowRoW.y2 - procWindowRoW.y1 };
	View src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
							  procWindowSize.x,
							  procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
							  procWindowSize.x,
							  procWindowSize.y );
	copy_pixels( src, dst );
	*/

}

}
}
}
