// ======================================================================== //
// Copyright 2009-2016 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

// ospray
#include "ospray/fb/FrameBuffer.h"

namespace ospray {

  /*! local frame buffer - frame buffer that exists on local machine */
  struct LocalFrameBuffer : public FrameBuffer {
    void      *colorBuffer; /*!< format depends on
                               FrameBuffer::colorBufferFormat, may be
                               NULL */
    float     *depthBuffer; /*!< one float per pixel, may be NULL */
    vec4f     *accumBuffer; /*!< one RGBA per pixel, may be NULL */

    LocalFrameBuffer(const vec2i &size,
                     ColorBufferFormat colorBufferFormat,
                     bool hasDepthBuffer,
                     bool hasAccumBuffer, 
                     void *colorBufferToUse=NULL);
    virtual ~LocalFrameBuffer();
    
    //! \brief common function to help printf-debugging 
    /*! \detailed Every derived class should overrride this! */
    virtual std::string toString() const
    { return "ospray::LocalFrameBuffer"; }

    virtual void setTile(Tile &tile);

    virtual const void *mapColorBuffer();
    virtual const void *mapDepthBuffer();
    virtual void unmap(const void *mappedMem);
    virtual void clear(const uint32 fbChannelFlags);
  };

} // ::ospray
