#include "kinectcapture.h"
#include <QDebug>
#include <QDateTime>

#define FRAMECOUNT 500

KinectCapture::KinectCapture()
	:_iks(nullptr)
	,_frame_reader(nullptr)
	,m_color_mat(KinectCapture::ColorImageHeight,KinectCapture::ColorImageWidth,CV_8UC4)
	,m_depth_mat(KinectCapture::DepthImageHeight,KinectCapture::DepthImageWidth,CV_8UC4)
	//,_color_image_size(KinectCapture::Width*KinectCapture::Height*KinectCapture::Depth)
	,m_tm_fmt("yyyyMMdd_HHmmss_zzz")
	,m_save_dir("E:/Video")
	,m_color_video_writer(new cv::VideoWriter)
	,m_depth_video_writer(new cv::VideoWriter)
	,m_frame_count(0)
{
	qDebug()<<"Color Image Size="<<ColorImageSize;
}

KinectCapture::~KinectCapture()
{
	qDebug()<<"release";

	if(_evt_frame_ready!=NULL)
	{
		_frame_reader->UnsubscribeMultiSourceFrameArrived(_evt_frame_ready);
		_frame_reader->Release();
	}

	//    qDebug()<<"release 2";
	//    if(_frame_reader!=nullptr)
	//    {
	//        _frame_reader->Release();
	//    }

	//    qDebug()<<"release 3";
	if(_iks!=nullptr)
	{
		_iks->Close();
		_iks->Release();
	}

	m_color_video_writer->release();


	//    if(_iks!=nullptr)
	//    {
	////        _iks->Close();
	////        _frame_reader->Release();
	//        _iks->Release();
	//    }

	//    qDebug()<<"release 4";
}

//Initialize Kinect Sensor
bool KinectCapture::Initialize()
{
	//    IKinectSensor *sensor = nullptr;
	if(FAILED(GetDefaultKinectSensor(&_iks)))
	{
		qDebug()<<"Fatal Error: Canno Find Default Kinect Sensor";
		return false;
	}

	//    _iks.reset(sensor);


	if(FAILED(_iks->Open()))
	{
		qDebug()<<"Fatal Error: Cannot Open Kinect Sensor";
		return false;
	}

	//    IColorFrameSource *pColorSource = NULL;

	//    if(FAILED(_iks->get_ColorFrameSource(&pColorSource)))
	//    {
	//        return false;
	//    }

	//    if(FAILED(pColorSource->OpenReader(&_icolor)))
	//    {
	//        return false;
	//    }

	//    IDepthFrameSource *pDepthSource = NULL;

	//    if(FAILED(_iks->get_DepthFrameSource(&pDepthSource)))
	//    {
	//        return false;
	//    }

	//    if(FAILED(pDepthSource->OpenReader(&_idepth)))
	//    {
	//        return false;
	//    }

	//    IBodyFrameSource *pBodySource = NULL;
	//    if(FAILED(_iks->get_BodyFrameSource(&pBodySource)))
	//    {
	//        return false;
	//    }

	//    if(FAILED(pBodySource->OpenReader(&_ibody)))
	//    {
	//        return false;
	//    }



	DWORD frame_type = FrameSourceTypes::FrameSourceTypes_Depth |  FrameSourceTypes::FrameSourceTypes_Color |
			FrameSourceTypes::FrameSourceTypes_Body;

	//    IMultiSourceFrameReader *reader = nullptr;

	if(FAILED(_iks->OpenMultiSourceFrameReader(frame_type,&_frame_reader)))
	{
		qDebug()<<"Fatal Error: Cannot Open Frame Reader";
		return false;
	}


	//    _frame_reader.reset(reader);

	_frame_reader->SubscribeMultiSourceFrameArrived(&_evt_frame_ready);

	return true;
}

bool KinectCapture::ProcessArrivedFrame(IMultiSourceFrameArrivedEventArgs *args)
{
	//    qDebug()<<"Process Arrived Frame Data";

	IMultiSourceFrameReference *fr = nullptr;

	if(SUCCEEDED(args->get_FrameReference(&fr)))
	{
		IMultiSourceFrame *sf = nullptr;
		if(SUCCEEDED(fr->AcquireFrame(&sf)))
		{
			CaptureColorFrame(sf);
			CaptureDepthFrame(sf);

			sf->Release();

			return true;
		}

		fr->Release();
	}

	return false;

}

bool KinectCapture::ProcessDepthFrame(UINT16 *pBuffer, USHORT nMinDepth, USHORT nMaxDepth)
{
	if (pBuffer)
	{
		//        RGBQUAD* pRGBX = m_pDepthRGBX;

		// end pixel is start + width*height - 1
		//const UINT16* pBufferEnd = pBuffer + (DepthImageWidth * DepthImageHeight);

		int idx = 0;
		//int length = DepthImageSize;

		unsigned char* mat_data = m_depth_mat.ptr<unsigned char>();

		while (idx < DepthImageSize)
		{
			USHORT depth = pBuffer[idx];

			// To convert to a byte, we're discarding the most-significant
			// rather than least-significant bits.
			// We're preserving detail, although the intensity will "wrap."
			// Values outside the reliable depth range are mapped to 0 (black).

			// Note: Using conditionals in this loop could degrade performance.
			// Consider using a lookup table instead when writing production code.
			BYTE intensity = static_cast<BYTE>((depth >= nMinDepth) && (depth <= nMaxDepth) ? (depth % 256) : 0);


			//m_depth_mat[idx] = intensity;
			//m_depth_mat[idx][0] = intensity;
			//m_depth_mat[idx][1] = intensity;
			//m_depth_mat[idx][2] = intensity;
			mat_data[idx] = intensity;
			mat_data[idx*4+1] = intensity;
			mat_data[idx*4+2] = intensity;

			++idx;

			//            pRGBX->rgbRed   = intensity;
			//            pRGBX->rgbGreen = intensity;
			//            pRGBX->rgbBlue  = intensity;

			//            ++pRGBX;
			//            ++pBuffer;
		}
	}
}

bool KinectCapture::CaptureDepthFrame(IMultiSourceFrame *sf)
{
	IDepthFrameReference *idfr = nullptr;

	if(SUCCEEDED(sf->get_DepthFrameReference(&idfr)))
	{
		IDepthFrame* idf = nullptr;

		if(SUCCEEDED(idfr->AcquireFrame(&idf)))
		{
			USHORT min_dist;
			USHORT max_dist;

			if(SUCCEEDED(idf->get_DepthMinReliableDistance(&min_dist))
				 &&SUCCEEDED(idf->get_DepthMaxReliableDistance(&max_dist)))
			{
				UINT buffer_size = 0;
				UINT16 *ptr_buffer;

				if(SUCCEEDED(idf->AccessUnderlyingBuffer(&buffer_size,&ptr_buffer)))
				{
					ProcessDepthFrame(ptr_buffer,min_dist,max_dist);
				}

			}
		}
	}
}

bool KinectCapture::CaptureColorFrame(IMultiSourceFrame *sf)
{
	IColorFrameReference *cfr = nullptr;

	if(SUCCEEDED(sf->get_ColorFrameReference(&cfr)))
	{
		IColorFrame* cf = nullptr;
		if(SUCCEEDED(cfr->AcquireFrame(&cf)))
		{
			HRESULT hr = cf->CopyConvertedFrameDataToArray(ColorImageSize,m_color_mat.data,ColorImageFormat_Bgra);
			if(SUCCEEDED(hr))
			{
				if(m_frame_count==0)
				{
					if(m_color_video_writer->isOpened())
					{
						m_color_video_writer->release();
					}

					QString path = m_save_dir.absoluteFilePath(QDateTime::currentDateTime().toString(m_tm_fmt)+".avi");
					m_color_video_writer->open(path.toStdString().c_str(),9,8.0,cv::Size(ColorImageWidth,ColorImageHeight));

					qDebug()<<"Write to "<<path;
					//_cvw->write(_color_image);
				}

				++m_frame_count;

				if(m_frame_count==FRAMECOUNT)
				{
					m_frame_count = 0;
				}

				m_color_video_writer->write(m_color_mat);

				return true;
			}
		}
	}


	return false;




	//return hr==S_OK;




}

//void KinectCapture::WriteVideo()
//{
////    if(!_cvw->isOpened())
////    {
////       bool retval =  _cvw->open(path,9,8.0,cv::Size(KinectCapture::Width,KinectCapture::Height));
////       qDebug()<<retval;
////    }

//    _cvw->write(_color_image);

//}

//bool KinectCapture::StartWriteVideo(const char *path)
//{
//    return _cvw->open(path,9,8.0,cv::Size(ColorImageWidth,ColorImageHeight));
//}

//void KinectCapture::EndWriteVideo()
//{
//    //_cvw->release();
//}

//void

//bool KinectCapture::CopyColorFrameData(IColorFrame* frame)
//{
//   HRESULT hr = frame->CopyConvertedFrameDataToArray(_color_image_size,_color_image.data,ColorImageFormat_Bgra);
//   return SUCCEEDED(hr);
//}

