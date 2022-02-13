#pragma once
#include "command.h"
#include "iconfig.h"
#include "formatcfg.h"

namespace cmdlime::detail{

template<typename T, Format format>
class CommandCreator{
    using NameProvider = typename FormatCfg<format>::nameProvider;
public:
    CommandCreator(IConfig& cfg,
                   const std::string& varName,
                   std::optional<T>& commandValue,
                   typename Command<T>::Type type = Command<T>::Type::Normal)
            : cfg_(cfg)
    {
        Expects(!varName.empty());
        command_ = std::make_unique<Command<T>>(NameProvider::fullName(varName), commandValue, type);
    }

    auto& operator<<(const std::string& info)
    {
        command_->info().addDescription(info);
        return *this;
    }

    auto& operator<<(const Name& customName)
    {
        command_->info().resetName(customName.value());
        return *this;
    }

    operator std::optional<T>()
    {
        cfg_.addCommand(std::move(command_));
        return std::optional<T>{};
    }

private:
    std::unique_ptr<Command<T>> command_;
    IConfig& cfg_;
};

}