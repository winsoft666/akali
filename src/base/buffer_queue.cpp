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


#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include "base/buffer_queue.h"
#include "base/logging.h"
#include "base/safe_release_macro.h"

namespace ppx {
    namespace base {

		class BufferQueue::BufferQueueImpl {
		public:
			BufferQueueImpl() {
				first_element_ = 0;
				last_element_ = 0;
				element_num_ = 0;
				total_data_size_ = 0;
			}

			~BufferQueueImpl() {

			}

			QUEUE_ELEMENT *first_element_;
			QUEUE_ELEMENT *last_element_;
			unsigned int element_num_;
			unsigned int total_data_size_;
            std::string queue_name_;
			std::recursive_mutex queue_mutex_;
		};

        BufferQueue::BufferQueue(const std::string &queue_name) {
			impl_ = new BufferQueueImpl();

			impl_->queue_name_ = queue_name;
        }


        BufferQueue::~BufferQueue() {
            Clear();

			SAFE_DELETE(impl_);
        }

        unsigned int BufferQueue::GetFrontDataSize() {
            std::lock_guard<std::recursive_mutex> lg(impl_->queue_mutex_);
            return impl_->first_element_->size;
        }

        unsigned int BufferQueue::GetLastDataSize() {
            std::lock_guard<std::recursive_mutex> lg(impl_->queue_mutex_);
            return impl_->last_element_->size;
        }

		int64_t BufferQueue::ToOneBuffer(char** ppBuf) const {
			std::lock_guard<std::recursive_mutex> lg(impl_->queue_mutex_);
			if (ppBuf == NULL)
				return -1;

			const unsigned int iBufSize = GetTotalDataSize();

			*ppBuf = (char*)malloc(iBufSize);

			if (*ppBuf == NULL)
				return -1;

			QUEUE_ELEMENT * p = impl_->first_element_;
			unsigned int remaind = iBufSize;
			char* pB = *ppBuf;
			while (p && remaind > 0)
			{
				memcpy(pB, p->dataReadAddress, p->size);
				remaind -= p->size;
				pB += p->size;

				p = p->next;
			}

			return iBufSize;
		}

		int64_t BufferQueue::ToOneBufferWithNullEnding(char** ppBuf) const {
			std::lock_guard<std::recursive_mutex> lg(impl_->queue_mutex_);
			if (ppBuf == NULL)
				return -1;

			const unsigned int iBufSize = GetTotalDataSize();

			*ppBuf = (char*)malloc(iBufSize + 1);

			if (*ppBuf == NULL)
				return -1;

			(*ppBuf)[iBufSize] = 0;

			QUEUE_ELEMENT * p = impl_->first_element_;
			unsigned int remaind = iBufSize;
			char* pB = *ppBuf;
			while (p && remaind > 0)
			{
				memcpy(pB, p->dataReadAddress, p->size);
				remaind -= p->size;
				pB += p->size;

				p = p->next;
			}

			return iBufSize + 1;
		}

		bool BufferQueue::AddToFront(void *pSrcData, unsigned int nSrcDataSize) {
            if (pSrcData == 0 || nSrcDataSize == 0)
                return false;

            QUEUE_ELEMENT *elem = (QUEUE_ELEMENT *)malloc(sizeof(QUEUE_ELEMENT));

            if (!elem)
                return false;


            void *data = malloc(nSrcDataSize);

            if (!data) {
                free(elem);
                return false;
            }

            {
                std::lock_guard<std::recursive_mutex> lg(impl_->queue_mutex_);
                memcpy(data, pSrcData, nSrcDataSize);

                elem->dataReadAddress = data;
                elem->dataStartAddress = data;
                elem->size = nSrcDataSize;

				impl_->total_data_size_ += nSrcDataSize;
				impl_->element_num_++;


                if (impl_->first_element_ == 0) { // Add first element in queue.
                    // Now,no element in queue.
                    elem->prev = 0;
                    elem->next = 0;
					impl_->first_element_ = elem;
					impl_->last_element_ = elem;
                } else {
                    elem->prev = 0;
                    elem->next = impl_->first_element_;
					impl_->first_element_->prev = elem;
					impl_->first_element_ = elem;
                }
            }

            TraceMsgA("Buffer Queue(%s): Add data to front element: data-size %d.\n", impl_->queue_name_.c_str(), nSrcDataSize);

            return true;
        }


        bool BufferQueue::AddToLast(void *pSrcData, unsigned int nSrcDataSize) {
            if (pSrcData == 0 || nSrcDataSize == 0)
                return false;

            QUEUE_ELEMENT *elem = (QUEUE_ELEMENT *)malloc(sizeof(QUEUE_ELEMENT));

            if (!elem)
                return false;

            void *data = malloc(nSrcDataSize);

            if (!data) {
                free(elem);
                return false;
            }

            {
                std::lock_guard<std::recursive_mutex> lg(impl_->queue_mutex_);

                memcpy(data, pSrcData, nSrcDataSize);

                elem->dataReadAddress = data;
                elem->dataStartAddress = data;
                elem->size = nSrcDataSize;

				impl_->total_data_size_ += nSrcDataSize;
				impl_->element_num_++;

                // Add last element in queue.
                if (impl_->last_element_ == 0) {
                    // Now,no element in queue.
                    elem->prev = 0;
                    elem->next = 0;
					impl_->first_element_ = elem;
					impl_->last_element_ = elem;
                } else {
                    elem->prev = impl_->last_element_;
                    elem->next = 0;
					impl_->last_element_->next = elem;
					impl_->last_element_ = elem;
                }
            }

			TraceMsgA("Buffer Queue(%s): Add data to last element: data-size %d.\n", impl_->queue_name_.c_str(), nSrcDataSize);

            return true;
        }



        unsigned int BufferQueue::PopFromFront(void *pDestData, unsigned int nSize) {
            unsigned int rvalue = 0;
            unsigned int size;

            std::lock_guard<std::recursive_mutex> lg(impl_->queue_mutex_);

            if (impl_->element_num_ != 0 && pDestData != NULL) {
                // get smaller value of size.
                size = (impl_->first_element_->size > nSize) ? nSize : impl_->first_element_->size;

                if (size > 0)
                    memcpy(pDestData, impl_->first_element_->dataReadAddress, size);

				impl_->element_num_--;
				impl_->total_data_size_ -= impl_->first_element_->size;

                QUEUE_ELEMENT *next = impl_->first_element_->next;

                if (next != 0) {
                    next->prev = 0;

                    if (impl_->first_element_->dataStartAddress)
                        free(impl_->first_element_->dataStartAddress);

                    free(impl_->first_element_);
					impl_->first_element_ = next;
                } else {
                    if (impl_->first_element_->dataStartAddress)
                        free(impl_->first_element_->dataStartAddress);

                    free(impl_->first_element_);
					impl_->first_element_ = 0;
					impl_->last_element_ = 0;
                }

                rvalue = size;
            } else if (impl_->element_num_ != 0 && pDestData == NULL) {
                rvalue = impl_->first_element_->size;
            } else {
                rvalue = 0;
            }

			TraceMsgA("Buffer Queue(%s): pop data from front element: data-size %d.\n", impl_->queue_name_.c_str(), rvalue);

            return rvalue;
        }


        unsigned int BufferQueue::PopFromLast(void *pDestData, unsigned int nSize) {
            std::lock_guard<std::recursive_mutex> lg(impl_->queue_mutex_);
            unsigned int rvalue = 0;
            unsigned int size;

            if (impl_->element_num_ != 0 && pDestData != NULL) {
                // get smaller value of size
                size = (impl_->last_element_->size > nSize) ? nSize : impl_->last_element_->size;

                if (size > 0)
                    memcpy(pDestData, impl_->last_element_->dataReadAddress, size);

				impl_->element_num_--;
				impl_->total_data_size_ -= impl_->last_element_->size;

                QUEUE_ELEMENT *prev = impl_->last_element_->prev;

                if (prev) {
                    prev->next = 0;

                    if (impl_->last_element_->dataStartAddress)
                        free(impl_->last_element_->dataStartAddress);

                    free(impl_->last_element_);
					impl_->last_element_ = prev;
                } else {
                    if (impl_->last_element_->dataStartAddress)
                        free(impl_->last_element_->dataStartAddress);

                    free(impl_->last_element_);
					impl_->first_element_ = 0;
					impl_->last_element_ = 0;
                }

                rvalue = size;
            } else if (impl_->element_num_ != 0 && pDestData == NULL) {
                rvalue = impl_->last_element_->size;
            } else {
                rvalue = 0;
            }

			TraceMsgA("Buffer Queue(%s): pop data from last element: data-size %d.\n", impl_->queue_name_.c_str(), rvalue);

            return rvalue;
        }



        unsigned int BufferQueue::PopDataCrossElement(void *pOutputBuffer, unsigned int nBytesToRead, int *pBufferIsThrown) {
            std::lock_guard<std::recursive_mutex> lg(impl_->queue_mutex_);
            unsigned int nOutBufferNum = 0;
            unsigned int rvalue = 0;
            unsigned int nBytesRead = 0; // how much bytes has been read.
            unsigned int nByteNeed = nBytesToRead;
            char *pBuffer = (char *)pOutputBuffer;

            if (impl_->element_num_ != 0 && impl_->total_data_size_ > 0 && nBytesToRead > 0) {
                while (true) {
                    if (impl_->first_element_->size >= nByteNeed) { // we have enough data.
                        memcpy(pBuffer, impl_->first_element_->dataReadAddress, nByteNeed);

                        nBytesRead += nByteNeed;
						impl_->first_element_->size -= nByteNeed;
						impl_->total_data_size_ -= nByteNeed;

                        // check if buffer is empty.
                        if (impl_->first_element_->size == 0) {
                            // remove this element from queue.
                            nOutBufferNum++;
                            PopFromFront((void *)1, 0);
                        } else {
                            // element isn't empty, but we have removed some data from element.
							impl_->first_element_->dataReadAddress = (char *)impl_->first_element_->dataReadAddress + nByteNeed;
                        }

                        nByteNeed = 0;
                    } else {
                        memcpy(pBuffer, impl_->first_element_->dataReadAddress, impl_->first_element_->size);

                        nBytesRead += impl_->first_element_->size;
                        pBuffer += impl_->first_element_->size;
                        nByteNeed -= impl_->first_element_->size;
						impl_->total_data_size_ -= impl_->first_element_->size;
						impl_->first_element_->size = 0;
                        nOutBufferNum++;

                        PopFromFront((void *)1, 0);
                    }


                    if (nByteNeed == 0 || impl_->element_num_ == 0) {
                        if (pBufferIsThrown)
                            *pBufferIsThrown = nOutBufferNum;

                        rvalue = nBytesRead;
                        break;
                    }
                }
            } else {
                rvalue = 0;
            }

			TraceMsgA("Buffer Queue(%s): pop data from %d element(s): data-size %d.\n", impl_->queue_name_.c_str(), nOutBufferNum, rvalue);

            return rvalue;
        }


        unsigned int BufferQueue::GetFromFront(void *pDestData, unsigned int nSize) {
            std::lock_guard<std::recursive_mutex> lg(impl_->queue_mutex_);
            unsigned int rvalue;
            unsigned int size;

            if (impl_->element_num_ != 0) {
                if (pDestData == 0 || nSize == 0) {
                    rvalue = impl_->first_element_->size;
                } else {
                    // get smaller value of size.
                    size = (impl_->first_element_->size > nSize) ? nSize : impl_->first_element_->size;
                    memcpy(pDestData, impl_->first_element_->dataReadAddress, size);
                    rvalue = size;
                }
            } else {
                rvalue = 0;
            }

			TraceMsgA("Buffer Queue(%s): get data from front element: data-size %d.\n", impl_->queue_name_.c_str(), rvalue);

            return rvalue;
        }


        unsigned int BufferQueue::GetFromLast(void *pDestData, unsigned int nSize) {
            std::lock_guard<std::recursive_mutex> lg(impl_->queue_mutex_);

            unsigned int rvalue;
            unsigned int size;

            if (impl_->element_num_ != 0) {
                if (pDestData == 0 || nSize == 0) {
                    rvalue = impl_->last_element_->size;
                } else {
                    // get smaller value of size
                    size = (impl_->last_element_->size > nSize) ? nSize : impl_->last_element_->size;
                    memcpy(pDestData, impl_->last_element_->dataReadAddress, size);
                    rvalue = size;
                }
            } else {
                rvalue = 0;
            }

			TraceMsgA("Buffer Queue(%s): get data from last element: data-size %d.\n", impl_->queue_name_.c_str(), rvalue);

            return rvalue;
        }


        unsigned int BufferQueue::RemoveData(unsigned int nBytesToRemove) {
            std::lock_guard<std::recursive_mutex> lg(impl_->queue_mutex_);
            unsigned int rvalue = 1;
            unsigned int nByteNeed = nBytesToRemove;

            if (impl_->element_num_ > 0 && impl_->total_data_size_ > 0 && nBytesToRemove > 0) {
                while (impl_->total_data_size_ > 0) {
                    if (impl_->first_element_->size >= nByteNeed) { // we have enough data.
                        // check if buffer is empty
                        if (impl_->first_element_->size == nByteNeed) { // remove this element from queue
                            PopFromFront((void *)1, 0);
                        } else { // element isn't empty, but we have removed some data from element
							impl_->total_data_size_ -= nByteNeed;
							impl_->first_element_->size -= nByteNeed;
							impl_->first_element_->dataReadAddress = (char *)impl_->first_element_->dataReadAddress + nByteNeed;
                        }

                        break;
                    } else {
                        nByteNeed -= impl_->first_element_->size;
                        PopFromFront((void *)1, 0);
                        rvalue++;
                    }
                }
            } else {
                rvalue = 0;
            }

			TraceMsgA("Buffer Queue(%s): remove data from %d element(s): data-size %d.\n", impl_->queue_name_.c_str(), rvalue, nBytesToRemove - nByteNeed);

            return rvalue;
        }


        unsigned int BufferQueue::GetElementCount() const {
            std::lock_guard<std::recursive_mutex> lg(impl_->queue_mutex_);
            return impl_->element_num_;
        }


        unsigned int BufferQueue::GetTotalDataSize() const {
            std::lock_guard<std::recursive_mutex> lg(impl_->queue_mutex_);
            return impl_->total_data_size_;
        }


        unsigned int BufferQueue::Clear() {
            std::lock_guard<std::recursive_mutex> lg(impl_->queue_mutex_);
            unsigned int rvalue;
            rvalue = impl_->element_num_;

            if (impl_->element_num_ > 0) { // free memory.
                QUEUE_ELEMENT *elem = impl_->first_element_;
                QUEUE_ELEMENT *next = impl_->first_element_;

                while (next) {
                    if (next->dataStartAddress)
                        free(next->dataStartAddress);

                    next = next->next;
                    free(elem);
                    elem = next;
                }
            }

			impl_->first_element_ = NULL;
			impl_->last_element_ = NULL;
			impl_->element_num_ = 0;
			impl_->total_data_size_ = 0;

			TraceMsgA("Buffer Queue(%s): clear all data\n", impl_->queue_name_.c_str());

            return rvalue;
        }
    }
}
