// ======================================================================== //
// Copyright 2009-2018 Intel Corporation                                    //
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

// ospray/sg
#include "Volume.h"
// ospcommon
#include "ospcommon/range.h"

namespace ospray {
  namespace sg {

    //! AMR SG node with Chombo style structure
    struct OSPSG_INTERFACE AMRVolume : public Volume
    {
      AMRVolume();
      ~AMRVolume() override;

      std::string toString() const override;

      void preCommit(RenderContext &ctx) override;

      /*! parse a file generated by the apps/raw2amr converter, using
          the given brick size */
      void parseRaw2AmrFile(const FileName &fileName,
                            int BS,
                            int maxLevel = 1 << 30);

      // ------------------------------------------------------------------
      // this is the way we're passing over the data. for each input
      // box we create one data array (for the data values), and one
      // 'BrickInfo' descriptor that describes it. we then pass two
      // arrays - one array of all AMRBox'es, and one for all data
      // object handles. order of the brickinfos MUST correspond to
      // the order of the brickDataArrays; and the brickInfos MUST be
      // ordered from lowest level to highest level (inside each level
      // it doesn't matter)
      // ------------------------------------------------------------------
      struct BrickInfo
      {
        box3i box;
        int level;
        float dt;

        vec3i size() const
        {
          return box.size() + vec3i(1);
        }
      };

      // ID of the data component we want to render (each brick can
      // contain multiple components)
      int componentID{0};
      range1f valueRange;
      std::vector<OSPData> brickData;
      std::vector<BrickInfo> brickInfo;
      std::vector<float *> brickPtrs;
    };

  }  // ::ospray::sg
}  // ::ospray
