#!/usr/bin/python
#
# python-v4l2capture
#
# python-v4l2capture
# Python extension to capture video with video4linux2
#
# 2009, 2010, 2011 Fredrik Portstrom, released into the public domain
# 2011, Joakim Gebart
# 2013, Tim Sheerman-Chase
# See README for license

# Visual Studio 2010 trick: SET VS90COMNTOOLS=%VS100COMNTOOLS%

from distutils.core import Extension, setup
import os

if os.name == "nt":
    videolive = Extension("videolive", ["pixfmt.cpp", "libvideolive.cpp", "videoout.cpp", "videoin.cpp", "mfvideoin.cpp", "mfvideoout.cpp"],
						define_macros=[('_'+os.name.upper(), None)],
                          library_dirs=['C:\Dev\Lib\libjpeg-turbo-win\lib', "C:\Dev\Lib\pthreads\pthreads.2"],
                        include_dirs=['C:\Dev\Lib\libjpeg-turbo-win\include', "C:\Dev\Lib\pthreads\pthreads.2"],  
			libraries = ["pthreadVC2", "jpeg"])
else:
    videolive = Extension("videolive", ["v4l2capture.cpp", "v4l2out.cpp", "pixfmt.cpp", "libvideolive.cpp", "videoout.cpp", "videoin.cpp"], 
			define_macros=[('_'+os.name.upper(), None)],
			libraries = ["v4l2", "pthread", "jpeg"])
    
setup(
    name = "videolive",
    version = "1.0",
    author = "Tim Sheerman-Chase",
    author_email = "info@kinatomic",
    url = "http://www.kinatomic.com",
    description = "Capture and stream video",
    long_description = "Capture and stream video in python",
    license = "GPL v2 or later",
    classifiers = [
        "License :: GPL",
        "Programming Language :: C++"],
    ext_modules = [videolive]
    )

