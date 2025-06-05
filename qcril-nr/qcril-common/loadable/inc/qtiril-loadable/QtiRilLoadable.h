/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once
#ifndef QTIRIL_LOADABLE_INCLUDED
#define QTIRIL_LOADABLE_INCLUDED

#include <unordered_map>
#include <any>
#include <memory>
#include <qtiril-loadable/QtiRilLFile.h>

extern void *loadLibrary(const std::string &libname);

struct LoadableHeader {
    const char *pluginName;
    std::shared_ptr<void> theModule;
};

namespace QtiRil {
namespace Loadable {

/** struct returnable
 * Used while creating a function templated on the return type.
 *  In the case where a default value needs to be returned from the function,
 *  if the function can return void, it is not possible to construct a default value.
 *  For example:
 *  template <class R>
 *  R function() {
 *      if (some_check) {
 *          return normal_action<R>();
 *      }
 *      return R{}
 *  }
 *  When R is void, the previous function will cause a compilation error, since a default
 *  value of type void cannot be constructed. A way to solve this could be by specializing
 *  the function:
 *
 *  template <>
 *  void function() {
 *      if (some_chechk) {
 *          return normal_action<void)();
 *      }
 *      return;
 *  }
 *
 *  However this solution only works when we can fully specialize the function. In cases
 *  where a partial specialization would be required, this approach doesn't work. Here, we can
 *  use returnable to address the problem. e.g.:
 *
 *  template <class R, class Args...>
 *  R function(Args... args ) {
 *      if (some_check) {
 *          return normal_action<R>(args...);
 *      }
 *      return returnable<R>::value();
 *  }
 */
template <typename R>
struct returnable {
    private:
    public:
        static R value() {
            return R{};
        }
};
template <>
struct returnable<void> {
    public:
        static void value()  {
            return;
        }
};

/** ModuleHandle: And abstraction for a shared library that has been
 * loaded into memory.
 * It provides APIs to find symbols, and a templated call function
 * to symplify calling functions from within the loaded library.
 *
 * It is recommended  that any functions expected to be accessed through this
 * mechanism, should be using the c calling convention, to avoid having
 * to deal with c++ mangling, which is ABI-specific.
 *
 * Objects of this type should be handled only as a shared_ptr, as it follows
 * the RAII idiom.
 *
 * Also, every symbol found will contain a reference to the module that created
 * it, to prevent the module from being unloaded while users to it still exist.
 * Hence, it is important to clear any symbols no longer required to prevent
 * modules from being kept longer than required in memory.
 *
 * The ModuleHandle class also keeps a map of symbol names to
 * std::weak_ptr<ModuleHandle::ModuleSym>, to avoid having to
 * call dlsym on repeated lookups for the same symbol
 */
class ModuleHandle: public std::enable_shared_from_this<ModuleHandle> {
    public:
        template <typename T>
        class ModuleSym;
        using NativeHandleType = void *;

    private:
        NativeHandleType handle;

        using map = std::unordered_map<std::string, std::any>;
        map symbols;
    public:
        /** ModuleSym:
         * Class representing every symbol loaded from a shared library
         */
        template <typename T>
        class ModuleSym {
            friend class ModuleHandle;
            private:
                std::shared_ptr<ModuleHandle> owner;
                std::string name;
                T& thing;
            public:
                ModuleSym(std::shared_ptr<ModuleHandle> owner,
                        std::string name, T& thing):
                    owner(owner), name(name), thing(thing) {}
                ~ModuleSym() {
                    owner = nullptr;
                }
                /** get: Return the object corresponding to this ModuleSym
                 */
                T& get() {
                    return thing;
                }
        };
        /**
         * getSym: Method to find a symbol in the shared library represented
         * by this ModuleHandle
         *
         * @symname: The name of the symbol to lookup
         */
        template <typename T>
        std::shared_ptr<ModuleSym<T>> getSym(const std::string symname) {
            std::shared_ptr<ModuleSym<T>> symbol = nullptr;

            if (handle) {
                auto sym = symbols.find(symname);
                if (sym != symbols.end()) {
                    symbol =
                        std::any_cast<std::weak_ptr<ModuleSym<T>>>((*sym).second).lock();
                }
                if (!symbol) {
                    void *symbolp = findSymbol(symname);
                    if (symbolp) {
                        auto fromThis = shared_from_this();
                        auto tsymbolp = reinterpret_cast<T*>(symbolp);
                        symbol = std::make_shared<ModuleSym<T>>(fromThis,
                                symname,
                                *tsymbolp);
                        std::weak_ptr<ModuleSym<T>> weakptr {symbol};
                        symbols[symname] = weakptr;
                    }
                }
            }
            return symbol;
        }

        /**
         * call: Utility function to find a symbol and invoke it as a function
         * with the parameters args...
         */
        template <typename R, typename ... Args>
        R call(std::string name, Args... args) {
            auto sym = getSym<R(Args...)>(name);
            if (sym != nullptr) {
                return sym->get()(args...);
            }
            return returnable<R>::value();
        }

        /* Platform-specific */
    private:
        void *findSymbol(std::string symname);
    public:
        ModuleHandle(std::string path);
        ~ModuleHandle();
        NativeHandleType getNativeHandle() {
            return handle;
        }
};

class LoadableModule {
    public:
        static constexpr const char *extn = ".so";
    private:
        std::string name;
        SearchPath path;
        std::string libraryName;
        std::string fullName;
        std::shared_ptr<ModuleHandle> handle;
        using initfuncsym = ModuleHandle::ModuleSym<struct LoadableHeader *()>;
        using deinitfuncsym = ModuleHandle::ModuleSym<int()>;
        std::shared_ptr<initfuncsym> loadable_init;
        std::shared_ptr<deinitfuncsym> loadable_deinit;
        LoadableHeader *hdr;
    protected:
        bool exists() {
            bool ret = false;
            return ret;
        }
        std::string defaultPath() {
            std::string ret{};
            const char *envpath = getenv("QCRIL_LIBS_PATH");
            std::string tmp;
            if (envpath) {
                tmp = envpath;
            }
            return tmp;
        }
    public:
        LoadableModule(std::string name, std::string path = ""):
            name(name),
            path(path.size() > 0 ? path : defaultPath()),
            libraryName( (name.size() > 0 ) ? name : ""),
            fullName(),
            handle{nullptr}
        {}

        void setPath(std::string path) {
            this->path = path;
        }

        void setlibraryName(std::string libraryName) {
            this->libraryName = libraryName;
        }

        bool isLoaded() {
            return handle != nullptr;
        }
        std::shared_ptr<ModuleHandle> load() {
            if (!handle) {
                std::shared_ptr<File> file = path.findFile(libraryName);
                if (file) {
                    fullName = file->getFullName();
                    handle = std::make_shared<ModuleHandle>(fullName);
                    if (handle) {
                        loadable_init = handle->getSym<struct LoadableHeader *()>("loadable_init");
                        loadable_deinit = handle->getSym<int()>("loadable_deinit");
                    }
                }
            }
            return handle;
        }
        void unload() {
            loadable_init = nullptr;
            loadable_deinit = nullptr;
            handle = nullptr;
            hdr = nullptr;
        }

        LoadableHeader *init() {
            if (loadable_init) {
                return (*loadable_init.get()->get())();
            }
            return nullptr;
        }

        int deinit() {
            if (loadable_deinit) {
                return (*loadable_deinit.get()->get())();
            }
            return -1;
        }

        template <typename R, typename ... Args>
        R call(std::string name, Args... args) {
            if (handle) {
                return handle->call<R, Args...>(name, std::forward<Args>(args)...);
            }
            return returnable<R>::value();
        }

};

}
}

#endif
