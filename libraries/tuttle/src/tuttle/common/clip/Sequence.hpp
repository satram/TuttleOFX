#ifndef _FILENAMEMANAGER_HPP_
#define	_FILENAMEMANAGER_HPP_

#include <tuttle/common/utils/global.hpp>

#include <ofxCore.h>

#include <boost/regex.hpp>
#include <boost/filesystem.hpp>

#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>

namespace tuttle {
namespace common {

struct FilenamesGroup
{
	std::vector< std::string > _filenames;
	std::string _prefix;
	std::string _postfix;
	size_t _first;
	size_t _last;
	size_t _step;
	size_t _numFill;
	std::string _fillCar;
};

class Sequence
{
public:
	Sequence(): _numFill(0), _step(0), _first(0), _last(0) {}
	Sequence(const boost::filesystem::path& directory, const std::string & pattern, const bool dirbase = false, const size_t start = 0, const size_t step = 1 );
	Sequence(const boost::filesystem::path& directory, const bool dirbase = false, const size_t start = 0, const size_t step = 1 );
	virtual ~Sequence();
	void  reset(boost::filesystem::path filepath, const bool dirbase = false, const size_t start = 0, const size_t step = 1);
	const std::string getFirstFilename(const ssize_t nGroup = -1) const;
	const std::string getNextFilename(const ssize_t nGroup = -1);
	const std::string getFilenameAt(const OfxTime time, const ssize_t nGroup = -1) const;
	const size_t numGroups() const;
	const OfxRangeI getRange(const ssize_t nGroup = -1) const;
	inline std::size_t step() const;
protected:
	std::vector< FilenamesGroup > matchingGroups(const boost::filesystem::path & directory, const boost::regex & regex);

protected:
	boost::filesystem::path _path;					///< Path
	std::string _pattern;							///< Current pattern
	boost::regex _regexp;							///< Internal pattern regexp
	std::vector< FilenamesGroup > _matchList;		///< Currently unused, but usefull to automatically get a pattern from a directory
	std::string _prefixDir;							///< Store directory prefix
	std::string _prefixFile;						///< Store filename prefix
	std::string _postfixFile;						///< Store postfix
	std::string _fillCar;							///< Filling character
	size_t _numFill;								///< Number of filling char
	std::size_t _step;								///< Step
	std::size_t _first;								///< Starting num
	std::size_t _last;								///< Ending num
};

inline std::size_t Sequence::step() const
{
	return _step;
}

}
}

#endif