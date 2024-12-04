#pragma once

#include <Canavar/Engine/Util/Macros.h>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace RangeCalculator
{
    class CameraCalibration
    {
      public:
        CameraCalibration() = default;
        void Calibrate();

        DEFINE_MEMBER(cv::Mat, CameraMatrix);
        DEFINE_MEMBER(cv::Mat, DistCoeffs);
        DEFINE_MEMBER(cv::Mat, R);
        DEFINE_MEMBER(cv::Mat, T);
    };
}
