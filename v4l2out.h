#ifndef __V4L2OUT_H__
#define __V4L2OUT_H__

#include <Python.h>
#include <map>
#include <vector>
#include <string>

class Video_out_manager_cl{
public:
	PyObject_HEAD
	std::map<std::string, class Video_out *> *threads;
};
typedef Video_out_manager_cl Video_out_manager;

int Video_out_manager_init(Video_out_manager *self, PyObject *args,
		PyObject *kwargs);

void Video_out_manager_dealloc(Video_out_manager *self);

PyObject *Video_out_manager_open(Video_out_manager *self, PyObject *args);

PyObject *Video_out_manager_Send_frame(Video_out_manager *self, PyObject *args);

PyObject *Video_out_manager_close(Video_out_manager *self, PyObject *args);

// ******************************************************************

#endif //__V4L2OUT_H__

