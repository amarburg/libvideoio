
#include "libvideoio/Undistorter.h"

#include <tinyxml2.h>

#include <sstream>
#include <fstream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

namespace libvideoio
{

  using namespace tinyxml2;

  // Sample input:
  //
  // <?xml version="1.0" encoding="UTF-8"?>
  // <calibration>
  //   <projection>frame</projection>
  //   <width>1920</width>
  //   <height>1080</height>
  //   <f>1430.15016509976</f>
  //   <cx>-24.1342060175983</cx>
  //   <cy>17.5452248329602</cy>
  //   <b2>-4.25145544487403</b2>
  //   <k1>-0.118322055927498</k1>
  //   <k2>0.293632083518507</k2>
  //   <p1>0.000330080086304322</p1>
  //   <p2>0.00182154893322038</p2>
  //   <date>2017-11-14T23:30:12Z</date>
  // </calibration>

bool readDoubleFromCalibration( const XMLNode *parent, std::string name, double &value, double def = 0.0 )
{
  auto child = parent->FirstChildElement(name.c_str());
  if( child ) {
    value = atof(child->GetText());
    return true;
  }

  value = def;
  return false;
}

bool readIntFromCalibration( const XMLNode *parent, std::string name, int &value, int def = 0 )
{
  auto child = parent->FirstChildElement(name.c_str());
  if( child ) {
    value = atoi(child->GetText());
    return true;
  }

  value = def;
  return false;
}

OpenCVUndistorter *PhotoscanXMLUndistorterFactory::loadFromFile(const std::string &configFileName)

{

  cv::Mat originalK( cv::Mat(3, 3, CV_64F, cv::Scalar(0)) );
  cv::Mat distCoeffs( cv::Mat(1,4, CV_64F, cv::Scalar(0)) );
  bool valid = true;


  XMLDocument doc;
  doc.LoadFile( configFileName.c_str() );

  auto topNode = doc.FirstChildElement( "calibration" );

  if( !topNode ) {
    LOG(WARNING) << "Unable to find top-level element <\"calibration\"> calibration file " << configFileName;
    valid = false;
    return nullptr;
  }

  int width, height;
  valid = readIntFromCalibration(topNode, "width", width );
  valid = readIntFromCalibration(topNode, "height", height );


  double f=1.0, cx=0.0, cy=0.0;

  valid = readDoubleFromCalibration(topNode, "f", f);
  valid = readDoubleFromCalibration(topNode, "cx", cx);
  valid = readDoubleFromCalibration(topNode, "cy", cy);

	originalK.at<double>(0, 0) = f;
	originalK.at<double>(1, 1) = f;
	originalK.at<double>(2, 2) = 1;
	originalK.at<double>(0, 2) = cx;
	originalK.at<double>(1, 2) = cy;


  // This distortion elements are optional (no error if not found)

  // readDoubleFromCalibration( topNode, "b1", _b[0] );
  // readDoubleFromCalibration( topNode, "b2", _b[1] );

  double val;
  if( readDoubleFromCalibration( topNode, "k1", val ) ) {
  //     _distCoeffs.at<double>(0,0) = val;
  }

  readDoubleFromCalibration( topNode, "k2", val );
  //     _distCoeffs.at<double>(0,1) = atof( node->GetText() );

  readDoubleFromCalibration( topNode, "p1", val );
  //     _distCoeffs.at<double>(0,2) = atof( node->GetText() );

  readDoubleFromCalibration( topNode, "p2", val );
  //     _distCoeffs.at<double>(0,3) = atof( node->GetText() );




	if (valid) {
    return new OpenCVUndistorter( originalK, distCoeffs, cv::Size(width,height) );
	}

	// originalK = originalK.t();
	// _K = _K.t();

  return nullptr;
}




}