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

#include "MultiIsendIrecvMessaging.h"

namespace ospray {
  namespace mpi {
    namespace async {

      MultiIsendIrecvImpl::Group::Group(const std::string &name, MPI_Comm comm, 
                                       Consumer *consumer, int32 tag)
        : async::Group(comm,consumer,tag),
          sendThread(this), recvThread(this), procThread(this)
      {
        recvThread.start();
        sendThread.start();
        procThread.start();
      }

      void MultiIsendIrecvImpl::SendThread::run()
      {
        Group *g = this->group;
        while (1) {
          Action *action = g->sendQueue.get();
          // note we're not using any window here; we just pull them
          // in order. this is OK because they key is to have mulitple
          // sends going in parallel - which we do because each
          // (i)send is already started by the time it enters the
          // queue
          MPI_Wait(&action->request,MPI_STATUS_IGNORE);
          free(action->data);
          delete action;
        }
      }

      void MultiIsendIrecvImpl::RecvThread::run()
      {
        // note this thread not only _probes_ for new receives, it
        // also immediately starts the receive operation using Irecv()
        Group *g = (Group *)this->group;
        
        while (1) {
          MPI_Status status;
          MPI_CALL(Probe(MPI_ANY_SOURCE,g->tag,g->comm,&status));
          Action *action = new Action;
          action->addr = Address(g,status.MPI_SOURCE);

          MPI_CALL(Get_count(&status,MPI_BYTE,&action->size));

          action->data = malloc(action->size);
          MPI_CALL(Irecv(action->data,action->size,MPI_BYTE,status.MPI_SOURCE,status.MPI_TAG,
                        g->comm,&action->request));
          g->recvQueue.put(action);
        }
      }

      void MultiIsendIrecvImpl::ProcThread::run()
      {
        Group *g = (Group *)this->group;
        while (1) {
          Action *action = g->recvQueue.get();
          MPI_CALL(Wait(&action->request,MPI_STATUS_IGNORE));
          g->consumer->process(action->addr,action->data,action->size);
          delete action;
        }
      }

      void MultiIsendIrecvImpl::Group::shutdown()
      {
        std::cout << "shutdown() not implemented for this messaging ..." << std::endl;
      }

      void MultiIsendIrecvImpl::init()
      {
        mpi::world.barrier();
        printf("#osp:mpi:MultiIsendIrecvMessaging started up %i/%i\n",mpi::world.rank,mpi::world.size);
        fflush(0);
        mpi::world.barrier();
      }

      void MultiIsendIrecvImpl::shutdown()
      {
        mpi::world.barrier();
        printf("#osp:mpi:MultiIsendIrecvMessaging shutting down %i/%i\n",mpi::world.rank,mpi::world.size);
        fflush(0);
        mpi::world.barrier();
        for (int i=0;i<myGroups.size();i++)
          myGroups[i]->shutdown();

        MPI_CALL(Finalize());
      }

      async::Group *MultiIsendIrecvImpl::createGroup(const std::string &name, 
                                                    MPI_Comm comm, 
                                                    Consumer *consumer, 
                                                    int32 tag)
      {
        Group *g = new Group(name,comm,consumer,tag);
        myGroups.push_back(g);
        return g;
      }

      void MultiIsendIrecvImpl::send(const Address &dest, void *msgPtr, int32 msgSize)
      {
        Action *action = new Action;
        action->addr   = dest;
        action->data   = msgPtr;
        action->size   = msgSize;

        Group *g = (Group *)dest.group;
        MPI_CALL(Isend(action->data,action->size,MPI_BYTE,
                       dest.rank,g->tag,g->comm,&action->request));
        g->sendQueue.put(action);
      }

    } // ::ospray::mpi::async
  } // ::ospray::mpi
} // ::ospray
