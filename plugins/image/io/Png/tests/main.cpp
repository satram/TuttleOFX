#define BOOST_TEST_MODULE plugin_Png
#include <tuttle/test/main.hpp>

#include <boost/test/unit_test.hpp>

#include <tuttle/host/Graph.hpp>

#include <boost/preprocessor/stringize.hpp>

#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::unit_test;
using namespace tuttle::host;

BOOST_AUTO_TEST_SUITE( plugin_Png_reader )
std::string pluginName = "tuttle.pngreader";
std::string filename = "png/Gradient-8bit.png";
#include <tuttle/test/io/reader.hpp>
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE( plugin_Png_writer )
std::string pluginName = "tuttle.pngwriter";
std::string filename = "test-png.png";
#include <tuttle/test/io/writer.hpp>
BOOST_AUTO_TEST_SUITE_END()
