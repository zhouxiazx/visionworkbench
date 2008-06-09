// __BEGIN_LICENSE__
// 
// Copyright (C) 2006 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration
// (NASA).  All Rights Reserved.
// 
// Copyright 2006 Carnegie Mellon University. All rights reserved.
// 
// This software is distributed under the NASA Open Source Agreement
// (NOSA), version 1.3.  The NOSA has been approved by the Open Source
// Initiative.  See the file COPYING at the top of the distribution
// directory tree for the complete NOSA document.
// 
// THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY
// KIND, EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT
// LIMITED TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO
// SPECIFICATIONS, ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
// A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, ANY WARRANTY THAT
// THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT
// DOCUMENTATION, IF PROVIDED, WILL CONFORM TO THE SUBJECT SOFTWARE.
// 
// __END_LICENSE__

/// \file ControlNetwork.cc
/// 

#include <vw/Camera/ControlNetwork.h>

namespace vw {
namespace camera {

  std::ostream& operator<<( std::ostream& os, ControlMeasure const& measure) {
    os << measure.image_id() << ":" << measure.position();
    return os;
  }

  std::ostream& operator<<( std::ostream& os, ControlPoint const& point) {
    os << "[Control Point: " << point.position() << "] ";
    for (unsigned m=0; m<point.size(); ++m)
      os << point[m] << " ";
    os << "\n";
    return os;
  }

  std::ostream& operator<<( std::ostream& os, ControlNetwork const& cnet) {
    os << "Control Network: " << cnet.size() << " points.\n";
    for (unsigned p=0; p<cnet.size(); ++p)
      os << "\t" << cnet[p];
    os << "\n";
    return os;
  }



}} // namespace vw::camera
