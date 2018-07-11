#pragma once

#include "dlib/any.h"
#include "dlib/pipe.h"
#include "IsSystem.h"
#include "CvvImage.h"

using namespace dlib;

class CIsVideoDetectThread : public dlib::multithreaded_object
{
public:
	CIsVideoDetectThread();
	~CIsVideoDetectThread();

	void PushFrame(TaskInfo* jobTask);
	void StartThread();
	void StopThread();

	template<typename Object, typename Param1>
	void Set_OutputHander(Object& obj, void (Object::*handler)(Param1 p1))
	{
		//dlib::auto_mutex M(mtx);
		callback_hander2 = make_mfp(obj, handler);
	}
	
private:
	//dlib::any_function<void (vector<TaskInfo*>) > callback_hander2;
	dlib::pipe<TaskInfo*>	m_jobDetectTask;
	void DetectThread();
	void DisplayVideo(TaskInfo* jobTask);
	CvvImage				m_cvImage;
};

