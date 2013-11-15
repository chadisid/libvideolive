
#include <vector>
#include <pthread.h>
#include "videoout.h"
#ifdef _NT
#include "mfvideoout.h"
#endif
#if _POSIX
#include "v4l2out.h"
#endif

int Video_out_manager_init(Video_out_manager *self, PyObject *args,
		PyObject *kwargs)
{
	self->threads = new std::map<std::string, class Base_Video_Out *>;
	return 0;
}

void Video_out_manager_dealloc(Video_out_manager *self)
{
	//Stop high level threads
	for(std::map<std::string, class Base_Video_Out *>::iterator it = self->threads->begin(); 
		it != self->threads->end(); it++)
	{
		it->second->Stop();
		it->second->WaitForStop();
	}

	delete self->threads;
	self->threads = NULL;
	self->ob_type->tp_free((PyObject *)self);
}

PyObject *Video_out_manager_open(Video_out_manager *self, PyObject *args)
{
	//Process arguments
	const char *devarg = NULL;
	const char *pxFmtIn = NULL;
	int widthIn = 0;
	int heightIn = 0;

	if(!PyArg_ParseTuple(args, "ssii", &devarg, &pxFmtIn, &widthIn, &heightIn))
	{
		PyErr_Format(PyExc_RuntimeError, "Incorrect arguments to function.");
		Py_RETURN_NONE;
	}

	//Create worker thread
	pthread_t thread;
	#ifdef _POSIX
	Video_out *threadArgs = new Video_out(devarg);
	#endif
	#ifdef _NT
	MfVideoOut *threadArgs = new MfVideoOut(devarg);
	#endif

	(*self->threads)[devarg] = threadArgs;
	threadArgs->SetOutputSize(widthIn, heightIn);
	threadArgs->SetOutputPxFmt(pxFmtIn);

	#ifdef _POSIX
	pthread_create(&thread, NULL, Video_out_manager_Worker_thread, threadArgs);
	#endif
	#ifdef _NT
	pthread_create(&thread, NULL, MfVideoOut_Worker_thread, threadArgs);
	#endif

	Py_RETURN_NONE;
}

PyObject *Video_out_manager_Send_frame(Video_out_manager *self, PyObject *args)
{
	//printf("Video_out_manager_Send_frame\n");
	//dev = '\\dev\\video0', img, pixel_format, width, height

	//Process arguments
	const char *devarg = NULL;
	const char *imgIn = NULL;
	const char *pxFmtIn = NULL;
	int widthIn = 0;
	int heightIn = 0;

	if(PyObject_Length(args) < 5)
	{
		PyErr_Format(PyExc_RuntimeError, "Too few arguments.");
		Py_RETURN_NONE;
	}

	PyObject *pydev = PyTuple_GetItem(args, 0);
	devarg = PyString_AsString(pydev);

	PyObject *pyimg = PyTuple_GetItem(args, 1);
	imgIn = PyString_AsString(pyimg);
	Py_ssize_t imgLen = PyObject_Length(pyimg);

	PyObject *pyPxFmt = PyTuple_GetItem(args, 2);
	pxFmtIn = PyString_AsString(pyPxFmt);

	PyObject *pyWidth = PyTuple_GetItem(args, 3);
	widthIn = PyInt_AsLong(pyWidth);

	PyObject *pyHeight = PyTuple_GetItem(args, 4);
	heightIn = PyInt_AsLong(pyHeight);

	std::map<std::string, class Base_Video_Out *>::iterator it = self->threads->find(devarg);

	if(it != self->threads->end())
	{
		it->second->SendFrame(imgIn, imgLen, pxFmtIn, widthIn, heightIn);
	}
	else
	{
		PyErr_Format(PyExc_RuntimeError, "Device not found.");
		Py_RETURN_NONE;
	}

	Py_RETURN_NONE;
}

PyObject *Video_out_manager_close(Video_out_manager *self, PyObject *args)
{
	//Process arguments
	const char *devarg = "/dev/video0";
	if(PyTuple_Size(args) >= 1)
	{
		PyObject *pydevarg = PyTuple_GetItem(args, 0);
		devarg = PyString_AsString(pydevarg);
	}

	//Stop worker thread
	std::map<std::string, class Base_Video_Out *>::iterator it = self->threads->find(devarg);

	if(it != self->threads->end())
	{
		it->second->Stop();
	}

	Py_RETURN_NONE;
}

PyObject *Video_out_manager_list_devices(Video_out_manager *self)
{
	PyObject *out = PyList_New(0);
	std::vector<std::string> devLi = List_out_devices();
	for(unsigned i=0; i<devLi.size(); i++)
	{
		PyList_Append(out, PyString_FromString(devLi[i].c_str()));
	}
	PyList_Sort(out);
	return out;
}
