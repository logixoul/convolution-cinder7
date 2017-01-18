#include "stdafx.h"
#include "util.h"
#include "colorspaces.h"
#include "picopng.h"
#include "Dragging.h"
#include "gl.h"
#include "median.h"
#include "types.h"
#include "getOpt.h"
#include "sat.h"
#include "profiling.h"
#include "misc.h"
#include "fftwrap.h"
#include "D.h"
#include "expblur.h"
#include "render.h"
#include "global.h"
#include "input.h"
#include "init.h"

using namespace render;

typedef FFT_T<float> FFT;

class AudioGenerativeApp : public AppBasic {
 public:
	void mouseDown(MouseEvent e) { input::mouseDown(e); }
	void mouseUp(MouseEvent e) { input::mouseUp(e); }
	void mouseMove(MouseEvent e) { input::mouseMove(e); }
	void mouseDrag(MouseEvent e) { input::mouseDrag(e); }
	void keyDown(KeyEvent e) { input::keyDown(e); }
	void keyUp(KeyEvent e) { input::keyUp(e); }
	
	/*void bench()
	{
		for(int i = 256; i <= 512; i++)
		{
			FFT::RArray r(i, i);
			FFT::CArray c(i, i/2+1);
			FFT p1 = FFT::r2c(r, c, FFTW_MEASURE);
			FFT p2 = FFT::c2r(c, r, FFTW_MEASURE);
			Stopwatch::Start();
			p1.execute();
			p2.execute();
			cout << i << "x" << i << ": " << Stopwatch::GetElapsedMilliseconds() << " ms" << endl;
		}
	}*/

	void setup()
	{
		initApp();
	}
	
	void draw()
	{
		renderApp();
		GetOpt::render();
		Pfl::nextFrame();
		framesElapsed++;
	}

	void fluid()
	{
		GETFLOAT(fluidVelocity, "step=.01", .01);
		GETFLOAT(fluidSteer, "step=.001", 0*.01);
		GETFLOAT(fluidGravity, "step=.0001", .0);
		Array2D<Vec3f> result(image.w, image.h, Vec3f::zero());
		/*GETBOOL(OLD, "", true);*/bool OLD = false;
		auto func = [&](Array2D<Vec3f>& dest, int xStart, int xEnd) { 
			Vec2i p;
			for(p.x = xStart; p.x < xEnd; p.x++) for(p.y = 0; p.y < image.h; p.y++)
			{
				Vec3f& c = image(p);
				if(c.x+c.y+c.z < 0.001f)
					continue;
				Vec3f ab = toHsv.transformVec(c);
				float hue = atan2(ab.y, ab.x);
				float saturation = ab.xy().length();
				float brightness = ab.z;
				Vec2f offset = ab.xy() * (fluidVelocity * (saturation+.5));
				//Vec2f offset = ab.xy() * (fluidVelocity * (brightness+30));
				
				if(OLD) {
					ab.rotateZ(saturation*fluidSteer);
					c=toHsvInv.transformVec(ab);
				} else rotateHue_ip(c, saturation * fluidSteer);
					//c.rotate(Vec3f::one(), saturation*fluidSteer);
				
				offset.y += fluidGravity*(pow(brightness+saturation*2,3) + 1);
				
				/*Vec2f pp=p;int times=10;c/=times;offset/=times;
				for(int i=0;i<10;i++, pp+=offset)aaPoint(dest, pp, c);*/
				c/=2;
				aaPoint(dest, Vec2f(p) + offset, c);
				aaPoint(dest, Vec2f(p) + offset/2, c);
			}
		};
		boost::thread th1(func, ref(result), 0, image.w / 2);
		func(result, image.w / 2, image.w);
		th1.join();

		//forxy(image) { result(p) = result1(p) + result2(p); }
		image = result;
	}

	void complexArrayToImage(FFT::CArray in, Image& out)
	{
		forxy(out)
		{
			FFT::Complex& val = in(p);
			out(p) = (Vec3f&)hsvToRGB(Vec3f(
				atan2(val.y, val.x)/twoPi+.5, // hue
				1.0, // sat
				val.length())); // value
		}
	}

	FFT::CArray makeKernel()
	{
		static float oldSigma_ = -1, oldBo = -1;
		GETFLOAT(sigma_, "min=0 step=.1", 10);
		GETFLOAT(bloomOpacity_, "step=.01 group=bloom label='opacity (log scale)'", -11.17);
		static FFT::CArray kernel_fs(image.w, image.h / 2 + 1); // frequency space
		if(oldSigma_ == sigma_ && oldBo==bloomOpacity_)
			return kernel_fs;
		oldSigma_ = sigma_; oldBo = bloomOpacity_;

		static Array2D<float> kernel(image.w, image.h);
		
		float sigmaMul = 1/(sigma_*sigma_);
		float bloomOpacity = pow(2.0f, bloomOpacity_) * 300; // * 300 for historical reasons
		forxy(kernel)
		{
			const int radius = 10;
			static FFT::Complex one = FFT::Complex(1.0 / (pi*radius*radius), 0.0);
			static FFT::Complex zero = FFT::Complex(0.0, 0.0);
			kernel(p) = 0;
			auto pp = p;
			auto func = [&]() {
				//if(pp.length() < radius) kernel(p).x = one.x;//pow(pp.length() / radius, 10) * one.x;
				kernel(p) += exp(-pp.lengthSquared()*sigmaMul);
			};
			func();
			pp.x = image.w - 1 - pp.x;
			func();
			pp.y = image.h - 1 - pp.y;
			func();
			pp.x = image.w - 1 - pp.x;
			func();
		}
		float sum = accumulate(kernel.begin(), kernel.end(), 0);
		float normFactor = (bloomOpacity / image.area) / sum; // image.area because fftw produces unnormalized data
		forxy(kernel) kernel(p) *= normFactor; // #checkthis

		static auto plan3 = FFT::r2c(kernel, kernel_fs, FFTW_MEASURE);
		plan3.execute();

		return kernel_fs;
	}

	template<class T> vector<T> concat(vector<T> a, vector<T> b)
	{
		vector<T> c = a;
		c.insert(c.end(), b.begin(), b.end());
		return c;
	}
	/*template<class OutIt,class Diff,class Fn0>
	void _Generate_n(_OutIt dest, _Diff _Count, _Fn0 func)
	{
	for (; 0 < _Count; --_Count, ++dest)
		*dest = func();
	}*/
	void doFFT_rgb()
	{
		// todo: fftw_malloc
		// todo: sizes that are products of small factors are transformed most efficiently
		// todo: FFTW_PATIENT
		// todo: (but look at the context of the page) http://www.fftw.org/fftw3_doc/Real_002ddata-DFTs.html
		//		FFTW is best at handling sizes of the form 2a 3b 5c 7d 11e 13f, [...]
		FFT::CArray kernel_fs = makeKernel();

		static FFT::CArray c_array(image.w, image.h / 2 + 1);
		static FFT::RArray r_array(image.w, image.h);
		static auto planF = FFT::r2c(r_array, c_array, FFTW_MEASURE);
		static auto planB = FFT::c2r(c_array, r_array, FFTW_MEASURE);
		
		static FFT::CArray out(c_array.Size());
		static FFT::RArray lum(r_array.Size());
		
		for(int channel = 0; channel < 3; channel++)
		{
			PFL(fftCopyAndMul) for(int i = 0; i < lum.area; i++) lum.data[i] = toDraw.data[i].ptr()[channel];
			PFL(planExecute) planF.execute(lum, out);
			PFL(fftCopyAndMul) for(int i = 0; i < out.area; i++) comp(out.data[i]) *= comp(kernel_fs.data[i]);
			PFL(planExecute) planB.execute(out, lum);
			PFL(fftCopyAndMul) for(int i = 0; i < lum.area; i++) bloomSurface.data[i].ptr()[channel] = lum.data[i];
		}
		GETBOOL(showFFT, "group=fft", false);
		if(showFFT)
		{
			complexArrayToImage(kernel_fs, bloomSurface);
		}
	}

	void post()
	{
		GETFLOAT(saturationFloor, "min=0.0 max=1.0 step=0.01", 1);
		GETFLOAT(saturationMul, "min=0.0 step=0.01", 0.56);
		GETFLOAT(saturationPow, "min=0.0 step=0.01", 1.51);
		auto func = [&](int from, int to) {
			for(int i = from; i < to; i++) {
				auto c = image.data[i];
				float saturation = getSaturation(c);
				c *= saturationFloor + pow(saturation * saturationMul, saturationPow);
			
				toDraw.data[i] = c;
			}
		};
		boost::thread t(func, 0, image.area/2);
		func(image.area/2, image.area);
		t.join();
		copy(toDraw.begin(), toDraw.end(), bloomSurface.begin());
	}

	void pasteExternal()
	{
		GETFLOAT(spray, "min=0.0 step=.1", 7);
		GETFLOAT(sprayAlpha, "min=0.0 step=.1", .1);
		forxy(image) {
			auto& c = image(p);
			auto pp = Vec2f(p.y, p.x) * (pic.w / (float)image.w);
			if(pic.contains(pp)) c = lerp(c, pic(pp).xyz()*spray, pic(pp).w*sprayAlpha);
		}
	}

	void update()
	{
		input::update();
		GETBOOL(pause,"key=p",false);
		if(pause)return;

		PFL(fluid) fluid();
		GETFLOAT(alpha, "min=0.01 max=1.0 step=0.01", 1);
		PFL(expTest) expTest2D(image, alpha);
		PFL(pasteExternal) pasteExternal();
		PFL(post) post();
		PFL(fft)
			doFFT_rgb();
		PFL(trim) forxy(image) if(p.x < 10 || p.y < 10 || image.w-p.x < 10 || image.h - p.y < 10) image(p) = Vec3f::zero();
	}
};

CINDER_APP_BASIC( AudioGenerativeApp, ci::app::RendererGl )
