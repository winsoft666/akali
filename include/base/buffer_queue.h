/*******************************************************************************
* Copyright (C) 2018 - 2020, Jeffery Jiang, <china_jeffery@163.com>.
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

#include <string>
#include <mutex>
#include "base/constructormagic.h"
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
            BufferQueue(const std::wstring& queue_name = L"");
            ~BufferQueue();

            bool AddToFront(void *pSrcData, unsigned int nSrcDataSize);

            bool AddToLast(void *pSrcData, unsigned int nSrcDataSize);

            unsigned int PopFromFront(void *pDestData, unsigned int nSize);

            unsigned int PopFromLast(void *pDestData, unsigned int nSize);

            unsigned int GetFromFront(void *pDestData, unsigned int nSize);

            unsigned int GetFromLast(void *pDestData, unsigned int nSize);

            unsigned int Clear();

            unsigned int GetElementCount();

            unsigned int GetTotalDataSize();

            unsigned int PopDataCrossElement(void *pOutputBuffer, unsigned int nBytesToRead, int *pBufferIsThrown);

            unsigned int RemoveData(unsigned int nBytesToRemove);

            unsigned int GetFrontDataSize();
            unsigned int GetLastDataSize();
        private:
            QUEUE_ELEMENT *first_element_;
            QUEUE_ELEMENT *last_element_;
            unsigned int element_num_;
            unsigned int total_data_size_;
            std::wstring queue_name_;
            std::recursive_mutex queue_mutex_;

            PPX_DISALLOW_COPY_AND_ASSIGN(BufferQueue);
        };
    }
}


#endif //! PPX_BASE_BUFFER_QUEUE_H__