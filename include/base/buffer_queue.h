/*******************************************************************************
* Copyright (C) 2018 - 2020, winsoft666, <winsoft666@outlook.com>.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
*
* Expect bugs
*
* Please use and enjoy. Please let me know of any bugs/improvements
* that you have found/implemented and I will fix/incorporate them into this
* file.
*******************************************************************************/


#ifndef PPX_BASE_BUFFER_QUEUE_H__
#define PPX_BASE_BUFFER_QUEUE_H__

#include <mutex>
#include "base/constructormagic.h"
#include "base/string.h"
#include "ppx_export.h"

namespace ppx {
    namespace base {
        typedef struct QueueElem {
            void *dataStartAddress;   // start address of the data that we allocated.
            void *dataReadAddress;    // address of the data in buffer. Next time, we get data from this address.
            unsigned int size;        // the size of the data.
            struct QueueElem *prev;
            struct QueueElem *next;
        } QUEUE_ELEMENT;

        class PPX_API BufferQueue {
        public:
            explicit BufferQueue(const StringANSI& queue_name = "");
            ~BufferQueue();

            bool AddToFront(void *pSrcData, unsigned int nSrcDataSize);

            bool AddToLast(void *pSrcData, unsigned int nSrcDataSize);

            unsigned int PopFromFront(void *pDestData, unsigned int nSize);

            unsigned int PopFromLast(void *pDestData, unsigned int nSize);

            unsigned int GetFromFront(void *pDestData, unsigned int nSize);

            unsigned int GetFromLast(void *pDestData, unsigned int nSize);

            unsigned int Clear();

            unsigned int GetElementCount() const;

            unsigned int GetTotalDataSize() const;

            unsigned int PopDataCrossElement(void *pOutputBuffer, unsigned int nBytesToRead, int *pBufferIsThrown);

            unsigned int RemoveData(unsigned int nBytesToRemove);

            unsigned int GetFrontDataSize();
            unsigned int GetLastDataSize();

			int64_t ToOneBuffer(char** ppBuf) const;
			int64_t ToOneBufferWithNullEnding(char** ppBuf) const;
        private:

			class BufferQueueImpl;
			BufferQueueImpl* impl_;

            PPX_DISALLOW_COPY_AND_ASSIGN(BufferQueue);
        };
    }
}


#endif //! PPX_BASE_BUFFER_QUEUE_H__