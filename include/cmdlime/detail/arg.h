#pragma once
#include "iarg.h"
#include "configvar.h"
#include "configaccess.h"
#include "format.h"
#include "errors.h"
#include "customnames.h"
#include <sstream>
#include <functional>
#include <memory>

namespace cmdlime::detail{

template <typename T>
class Arg : public IArg, public ConfigVar{
public:
    Arg(const std::string& name,
        const std::string& type,
        std::function<T&()> argGetter)
        : ConfigVar(name, {}, type)
        , argGetter_(argGetter)
    {
    }

private:
    ConfigVar& info() override
    {
        return *this;
    }

    const ConfigVar& info() const override
    {
        return *this;
    }

    bool read(const std::string& data) override
    {
        auto stream = std::stringstream{data};
        stream >> argGetter_();
        return !stream.bad() && !stream.fail() && stream.eof();
    }

private:    
    std::function<T&()> argGetter_;
};

template <>
inline bool Arg<std::string>::read(const std::string& data)
{
    argGetter_() = data;
    return true;
}

template<typename T, typename TConfig>
class ArgCreator{
    using NameProvider = typename Format<TConfig::format>::nameProvider;
public:
    ArgCreator(TConfig& cfg,
               const std::string& varName,
               const std::string& type,
               std::function<T&()> argGetter)
        : arg_(std::make_unique<Arg<T>>(NameProvider::argName(varName),
                                        NameProvider::valueName(type), argGetter))
        , cfg_(cfg)
    {}

    ArgCreator<T, TConfig>& operator<<(const std::string& info)
    {
        arg_->addDescription(info);
        return *this;
    }

    ArgCreator<T, TConfig>& operator<<(const Name& customName)
    {
        arg_->resetName(customName.value());
        return *this;
    }

    ArgCreator<T, TConfig>& operator<<(const ValueName& valueName)
    {
        arg_->resetValueName(valueName.value());
        return *this;
    }

    operator T()
    {
        ConfigAccess<TConfig>{cfg_}.addArg(std::move(arg_));
        return T{};
    }

private:
    std::unique_ptr<Arg<T>> arg_;
    TConfig& cfg_;
};

}
