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

/*! \file LoadBalancer.h Implements the abstracion layer for a (tiled) load balancer */

// ospray
#include "ospray/common/OSPCommon.h"
#include "ospray/fb/FrameBuffer.h"
#include "ospray/render/Renderer.h"

// tbb
#ifdef OSPRAY_USE_TBB
# include <tbb/task_scheduler_init.h>
#endif

namespace ospray {


  struct TileRenderer;
  struct TiledLoadBalancer
  {
    static TiledLoadBalancer *instance;
    virtual std::string toString() const = 0;
    virtual void renderFrame(Renderer *tiledRenderer,
                             FrameBuffer *fb,
                             const uint32 channelFlags) = 0;
  };

  //! tiled load balancer for local rendering on the given machine
  /*! a tiled load balancer that orchestrates (multi-threaded)
    rendering on a local machine, without any cross-node
    communication/load balancing at all (even if there are multiple
    application ranks each doing local rendering on their own)  */
  struct LocalTiledLoadBalancer : public TiledLoadBalancer
  {
    LocalTiledLoadBalancer();

    void renderFrame(Renderer *renderer,
                     FrameBuffer *fb,
                     const uint32 channelFlags) override;

    std::string toString() const override;

#ifdef OSPRAY_USE_TBB
    tbb::task_scheduler_init tbb_init;
#endif
  };

  //! tiled load balancer for local rendering on the given machine
  /*! a tiled load balancer that orchestrates (multi-threaded)
    rendering on a local machine, without any cross-node
    communication/load balancing at all (even if there are multiple
    application ranks each doing local rendering on their own)  */
  struct InterleavedTiledLoadBalancer : public TiledLoadBalancer
  {
    size_t deviceID;
    size_t numDevices;

    InterleavedTiledLoadBalancer(size_t deviceID, size_t numDevices)
      : deviceID(deviceID), numDevices(numDevices)
    {
      if (ospray::debugMode || ospray::logLevel) {
        std::cout << "=========================================" << std::endl;
        std::cout << "INTERLEAVED LOAD BALANCER" << std::endl;
        std::cout << "=========================================" << std::endl;
      }
    }

    std::string toString() const override;

    void renderFrame(Renderer *tiledRenderer,
                     FrameBuffer *fb,
                     const uint32 channelFlags) override;
  };

} // ::ospray
