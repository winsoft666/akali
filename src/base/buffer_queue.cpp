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

namespace ppx {
    namespace base {

        BufferQueue::BufferQueue(const std::wstring &queue_name) {
            queue_name_ = queue_name;
            first_element_ = 0;
            last_element_ = 0;
            element_num_ = 0;
            total_data_size_ = 0;
        }


        BufferQueue::~BufferQueue() {
            Clear();
        }

        unsigned int BufferQueue::GetFrontDataSize() {
            std::lock_guard<std::recursive_mutex> lg(queue_mutex_);
            return first_element_->size;
        }

        unsigned int BufferQueue::GetLastDataSize() {
            std::lock_guard<std::recursive_mutex> lg(queue_mutex_);
            return last_element_->size;
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
                std::lock_guard<std::recursive_mutex> lg(queue_mutex_);
                memcpy(data, pSrcData, nSrcDataSize);

                elem->dataReadAddress = data;
                elem->dataStartAddress = data;
                elem->size = nSrcDataSize;

                total_data_size_ += nSrcDataSize;
                element_num_++;


                if (first_element_ == 0) { // Add first element in queue.
                    // Now,no element in queue.
                    elem->prev = 0;
                    elem->next = 0;
                    first_element_ = elem;
                    last_element_ = elem;
                } else {
                    elem->prev = 0;
                    elem->next = first_element_;
                    first_element_->prev = elem;
                    first_element_ = elem;
                }
            }

            //printf("Buffer Queue(%s): Add data to front element: data-size %d.\n", queue_name_, nSrcDataSize);

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
                std::lock_guard<std::recursive_mutex> lg(queue_mutex_);

                memcpy(data, pSrcData, nSrcDataSize);

                elem->dataReadAddress = data;
                elem->dataStartAddress = data;
                elem->size = nSrcDataSize;

                total_data_size_ += nSrcDataSize;
                element_num_++;

                // Add last element in queue.
                if (last_element_ == 0) {
                    // Now,no element in queue.
                    elem->prev = 0;
                    elem->next = 0;
                    first_element_ = elem;
                    last_element_ = elem;
                } else {
                    elem->prev = last_element_;
                    elem->next = 0;
                    last_element_->next = elem;
                    last_element_ = elem;
                }
            }

            //printf("Buffer Queue(%s): Add data to last element: data-size %d.\n", queue_name_, nSrcDataSize);

            return true;
        }



        unsigned int BufferQueue::PopFromFront(void *pDestData, unsigned int nSize) {
            unsigned int rvalue = 0;
            unsigned int size;

            std::lock_guard<std::recursive_mutex> lg(queue_mutex_);

            if (element_num_ != 0 && pDestData != NULL) {
                // get smaller value of size.
                size = (first_element_->size > nSize) ? nSize : first_element_->size;

                if (size > 0)
                    memcpy(pDestData, first_element_->dataReadAddress, size);

                element_num_--;
                total_data_size_ -= first_element_->size;

                QUEUE_ELEMENT *next = first_element_->next;

                if (next != 0) {
                    next->prev = 0;

                    if (first_element_->dataStartAddress)
                        free(first_element_->dataStartAddress);

                    free(first_element_);
                    first_element_ = next;
                } else {
                    if (first_element_->dataStartAddress)
                        free(first_element_->dataStartAddress);

                    free(first_element_);
                    first_element_ = 0;
                    last_element_ = 0;
                }

                rvalue = size;
            } else if (element_num_ != 0 && pDestData == NULL) {
                rvalue = first_element_->size;
            } else {
                rvalue = 0;
            }

            //printf("Buffer Queue(%s): pop data from front element: data-size %d.\n", queue_name_, rvalue);

            return rvalue;
        }


        unsigned int BufferQueue::PopFromLast(void *pDestData, unsigned int nSize) {
            std::lock_guard<std::recursive_mutex> lg(queue_mutex_);
            unsigned int rvalue = 0;
            unsigned int size;

            if (element_num_ != 0 && pDestData != NULL) {
                // get smaller value of size
                size = (last_element_->size > nSize) ? nSize : last_element_->size;

                if (size > 0)
                    memcpy(pDestData, last_element_->dataReadAddress, size);

                element_num_--;
                total_data_size_ -= last_element_->size;

                QUEUE_ELEMENT *prev = last_element_->prev;

                if (prev) {
                    prev->next = 0;

                    if (last_element_->dataStartAddress)
                        free(last_element_->dataStartAddress);

                    free(last_element_);
                    last_element_ = prev;
                } else {
                    if (last_element_->dataStartAddress)
                        free(last_element_->dataStartAddress);

                    free(last_element_);
                    first_element_ = 0;
                    last_element_ = 0;
                }

                rvalue = size;
            } else if (element_num_ != 0 && pDestData == NULL) {
                rvalue = last_element_->size;
            } else {
                rvalue = 0;
            }

            //printf("Buffer Queue(%s): pop data from last element: data-size %d.\n", queue_name_, rvalue);

            return rvalue;
        }



        unsigned int BufferQueue::PopDataCrossElement(void *pOutputBuffer, unsigned int nBytesToRead, int *pBufferIsThrown) {
            std::lock_guard<std::recursive_mutex> lg(queue_mutex_);
            unsigned int nOutBufferNum = 0;
            unsigned int rvalue = 0;
            unsigned int nBytesRead = 0; // how much bytes has been read.
            unsigned int nByteNeed = nBytesToRead;
            char *pBuffer = (char *)pOutputBuffer;

            if (element_num_ != 0 && total_data_size_ > 0 && nBytesToRead > 0) {
                while (true) {
                    if (first_element_->size >= nByteNeed) { // we have enough data.
                        memcpy(pBuffer, first_element_->dataReadAddress, nByteNeed);

                        nBytesRead += nByteNeed;
                        first_element_->size -= nByteNeed;
                        total_data_size_ -= nByteNeed;

                        // check if buffer is empty.
                        if (first_element_->size == 0) {
                            // remove this element from queue.
                            nOutBufferNum++;
                            PopFromFront((void *)1, 0);
                        } else {
                            // element isn't empty, but we have removed some data from element.
                            first_element_->dataReadAddress = (char *)first_element_->dataReadAddress + nByteNeed;
                        }

                        nByteNeed = 0;
                    } else {
                        memcpy(pBuffer, first_element_->dataReadAddress, first_element_->size);

                        nBytesRead += first_element_->size;
                        pBuffer += first_element_->size;
                        nByteNeed -= first_element_->size;
                        total_data_size_ -= first_element_->size;
                        first_element_->size = 0;
                        nOutBufferNum++;

                        PopFromFront((void *)1, 0);
                    }


                    if (nByteNeed == 0 || element_num_ == 0) {
                        if (pBufferIsThrown)
                            *pBufferIsThrown = nOutBufferNum;

                        rvalue = nBytesRead;
                        break;
                    }
                }
            } else {
                rvalue = 0;
            }

            //printf("Buffer Queue(%s): pop data from %d element(s): data-size %d.\n", queue_name_, nOutBufferNum, rvalue);

            return rvalue;
        }


        unsigned int BufferQueue::GetFromFront(void *pDestData, unsigned int nSize) {
            std::lock_guard<std::recursive_mutex> lg(queue_mutex_);
            unsigned int rvalue;
            unsigned int size;

            if (element_num_ != 0) {
                if (pDestData == 0 || nSize == 0) {
                    rvalue = first_element_->size;
                } else {
                    // get smaller value of size.
                    size = (first_element_->size > nSize) ? nSize : first_element_->size;
                    memcpy(pDestData, first_element_->dataReadAddress, size);
                    rvalue = size;
                }
            } else {
                rvalue = 0;
            }

            //printf("Buffer Queue(%s): get data from front element: data-size %d.\n", queue_name_, rvalue);

            return rvalue;
        }


        unsigned int BufferQueue::GetFromLast(void *pDestData, unsigned int nSize) {
            std::lock_guard<std::recursive_mutex> lg(queue_mutex_);

            unsigned int rvalue;
            unsigned int size;

            if (element_num_ != 0) {
                if (pDestData == 0 || nSize == 0) {
                    rvalue = last_element_->size;
                } else {
                    // get smaller value of size
                    size = (last_element_->size > nSize) ? nSize : last_element_->size;
                    memcpy(pDestData, last_element_->dataReadAddress, size);
                    rvalue = size;
                }
            } else {
                rvalue = 0;
            }

            //printf("Buffer Queue(%s): get data from last element: data-size %d.\n", queue_name_, rvalue);

            return rvalue;
        }


        unsigned int BufferQueue::RemoveData(unsigned int nBytesToRemove) {
            std::lock_guard<std::recursive_mutex> lg(queue_mutex_);
            unsigned int rvalue = 1;
            unsigned int nByteNeed = nBytesToRemove;

            if (element_num_ > 0 && total_data_size_ > 0 && nBytesToRemove > 0) {
                while (total_data_size_ > 0) {
                    if (first_element_->size >= nByteNeed) { // we have enough data.
                        // check if buffer is empty
                        if (first_element_->size == nByteNeed) { // remove this element from queue
                            PopFromFront((void *)1, 0);
                        } else { // element isn't empty, but we have removed some data from element
                            total_data_size_ -= nByteNeed;
                            first_element_->size -= nByteNeed;
                            first_element_->dataReadAddress = (char *)first_element_->dataReadAddress + nByteNeed;
                        }

                        break;
                    } else {
                        nByteNeed -= first_element_->size;
                        PopFromFront((void *)1, 0);
                        rvalue++;
                    }
                }
            } else {
                rvalue = 0;
            }

            //printf("Buffer Queue(%s): remove data from %d element(s): data-size %d.\n", queue_name_, rvalue, nBytesToRemove - nByteNeed);

            return rvalue;
        }


        unsigned int BufferQueue::GetElementCount() {
            std::lock_guard<std::recursive_mutex> lg(queue_mutex_);
            return element_num_;
        }


        unsigned int BufferQueue::GetTotalDataSize() {
            std::lock_guard<std::recursive_mutex> lg(queue_mutex_);
            return total_data_size_;
        }


        unsigned int BufferQueue::Clear() {
            std::lock_guard<std::recursive_mutex> lg(queue_mutex_);
            unsigned int rvalue;
            rvalue = element_num_;

            if (element_num_ > 0) { // free memory.
                QUEUE_ELEMENT *elem = first_element_;
                QUEUE_ELEMENT *next = first_element_;

                while (next) {
                    if (next->dataStartAddress)
                        free(next->dataStartAddress);

                    next = next->next;
                    free(elem);
                    elem = next;
                }
            }

            first_element_ = NULL;
            last_element_ = NULL;
            element_num_ = 0;
            total_data_size_ = 0;

            //printf("Buffer Queue(%s): clear all data\n", queue_name_);

            return rvalue;
        }
    }
}
