#include <cmath>
#include <algorithm>
#include <iostream>
#include <vector>
#include <map>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <strstream>
#include <string>
#include <cinder/app/AppBasic.h>
#include <cinder/gl/GlslProg.h>
#include <cinder/gl/Texture.h>
#include <cinder/ImageIo.h>
//#include <cinder/audio/Output.h>
//#include <cinder/audio/Callback.h>
#include <cinder/CinderMath.h>
#include <cinder/Vector.h>
#include <cinder/Rand.h>
#include <cinder/Perlin.h>
#include "cinder/params/Params.h"
#include <boost/thread.hpp>
//#include <boost/range/join.hpp>
#include <boost/foreach.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/range.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/numeric.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/irange.hpp>
#include <boost/assign.hpp>
#include <regex>
#include <complex>
#include <fftw3.h>
#include <ppl.h> // VS parallel_for
//#include <glm/glm/gtc/matrix_transform.hpp>
//#include <glm/glm/glm.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace boost::assign;
using boost::irange;
//#define GLM_FORCE_INLINE
//#define GLM_SWIZZLE
//#define GLM_FORCE_SSE2

#define foreach BOOST_FOREACH