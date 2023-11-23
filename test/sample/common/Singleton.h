/*
 * © COPYRIGHT 2022 Corporation CAICT All rights reserved.
 *  http://www.caict.ac.cn
 *  https://bitfactory.cn
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * @author: maxufeng@caict.ac.cn
 * @date: 2023-11-22 18:21:00
 * @file: Singleton.h
 */


#ifndef __SINGLETON_H__
#define __SINGLETON_H__
#include <cassert>
namespace common {

template <class T>
class Singleton {
private:
    static T *mp_instance;

protected:
    Singleton() {}
    virtual ~Singleton() {}

public:
    static bool InitInstance() {
        if (nullptr != mp_instance)
            return false;

        mp_instance = new T;
        return true;
    }

    template <class CLS_TYPE, class ARG_TYPE>
    static bool InitInstance(ARG_TYPE nArg) {
        if (nullptr != mp_instance)
            return false;

        mp_instance = new CLS_TYPE(nArg);
        return true;
    }

    static bool ExitInstance() {
        if (nullptr == mp_instance)
            return false;

        delete mp_instance;
        mp_instance = nullptr;
        return true;
    }

    inline static T *GetInstance() { return mp_instance; }

    inline static T &Instance() {
        assert(nullptr != mp_instance);
        return *mp_instance;
    }

    template <class CLS_TYPE>
    inline static CLS_TYPE *GetSubInstance() {
        if (nullptr == mp_instance) {
            return nullptr;
        }

        return dynamic_cast<CLS_TYPE *>(mp_instance);
    }

    template <class CLS_TYPE>
    inline static CLS_TYPE &SubInstance() {
        assert(nullptr != mp_instance);
        return *dynamic_cast<CLS_TYPE *>(mp_instance);
    }
};

template <class T>
T *common::Singleton<T>::mp_instance = nullptr;

}  // namespace common

#endif  // Singleton_h__