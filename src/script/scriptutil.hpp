// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_SCRIPT_SCRIPTURIL_HPP
#define TESTWORLD_SCRIPT_SCRIPTURIL_HPP

#include <functional>
#include <stdexcept>
#include <type_traits>
#include <fmt/core.h>
#include <angelscript.h>
#include <scriptbuilder/scriptbuilder.h>


namespace awe::script
{
    namespace detailed
    {
        template <typename Arg>
        std::enable_if_t<std::is_integral_v<std::remove_reference_t<Arg>>, int>
        SetArg(asIScriptContext* ctx, int idx, Arg&& arg)
        {
            using namespace std;
            using T = remove_cv_t<remove_reference_t<Arg>>;
            if constexpr(sizeof(T) == sizeof(asBYTE))
                return ctx->SetArgByte(idx, (asBYTE)arg);
            else if constexpr(sizeof(T) == sizeof(asWORD))
                return ctx->SetArgWord(idx, (asWORD)arg);
            else if constexpr(sizeof(T) == sizeof(asDWORD))
                return ctx->SetArgDWord(idx, (asDWORD)arg);
            else if constexpr(sizeof(T) == sizeof(asQWORD))
                return ctx->SetArgQWord(idx, (asQWORD)arg);
        }
        template <typename Arg>
        std::enable_if_t<std::is_floating_point_v<std::remove_reference_t<Arg>>, int>
        SetArg(asIScriptContext* ctx, int idx, Arg&& arg)
        {
            using namespace std;
            using T = remove_cv_t<remove_reference_t<Arg>>;
            if constexpr(is_same_v<T, float>)
                return ctx->SetArgFloat(idx, arg);
            else if constexpr(is_same_v<T, double>)
                return ctx->SetArgDouble(idx, arg);
        }
        template <typename Arg>
        std::enable_if_t<std::is_class_v<std::remove_reference_t<Arg>>, int>
        SetArg(asIScriptContext* ctx, int idx, Arg&& arg)
        {
            return ctx->SetArgObject(idx, &arg);
        }
        template <typename T>
        int SetArg(asIScriptContext* ctx, int idx, std::reference_wrapper<T> arg)
        {
            using namespace std;
            using U = remove_cv_t<remove_reference_t<T>>;
            if constexpr(is_class_v<U>)
                return ctx->SetArgObject(idx, &arg.get());
            else
                return ctx->SetArgAddress(idx, &arg.get());
        }

        template <typename T, typename... Args>
        void ProcArg(asIScriptContext* ctx, int idx, T&& arg, Args&&... args)
        {
            int r = SetArg(ctx, idx, arg);
            if(r < 0)
            {
                throw std::invalid_argument(fmt::format(
                    "Invalid argument at arg{}"
                    "of the function {}\n"
                    "Error code: {}",
                    idx,
                    ctx->GetFunction()->GetName(),
                    r
                ));
            }
            ProcArg(ctx, ++idx, std::forward<Args>(args)...);
        }
        // Terminal
        inline void ProcArg(asIScriptContext* ctx, int idx) {}

        template <typename Ret>
        Ret GetRet(asIScriptContext* ctx)
        {
            using namespace std;
            using T = remove_cv_t<remove_reference_t<Ret>>;
            if constexpr(is_class_v<T>)
                return *(T*)ctx->GetReturnObject();
            else if constexpr(is_reference_v<Ret>)
                return *(T*)ctx->GetReturnAddress();
            else if constexpr(is_integral_v<T>)
            {
                if constexpr(sizeof(T) == sizeof(asBYTE))
                    return (Ret)ctx->GetReturnByte();
                else if constexpr(sizeof(T) == sizeof(asWORD))
                    return (Ret)ctx->GetReturnWord();
                else if constexpr(sizeof(T) == sizeof(asDWORD))
                    return (Ret)ctx->GetReturnDWord();
                else if constexpr(sizeof(T) == sizeof(asQWORD))
                    return (Ret)ctx->GetReturnQWord();
            }
            else if constexpr(is_floating_point_v<T>)
            {
                if constexpr(is_same_v<T, float>)
                    return ctx->GetReturnFloat();
                else if constexpr(is_same_v<T, double>)
                    return ctx->GetReturnDouble();
            }
        }
    }

    template <typename Ret = void, typename... Args>
    Ret Call(
        asIScriptFunction* func,
        asIScriptContext* ctx,
        Args&&... args
    ) {
        int r = 0;
        r = ctx->Prepare(func);
        if(r < 0)
            throw std::runtime_error("Failed to prepare calling script function");
        detailed::ProcArg(ctx, 0, std::forward<Args>(args)...);
        r = ctx->Execute();
        if(r != asEXECUTION_FINISHED)
        {
            if(r == asEXECUTION_EXCEPTION)
            {
                throw std::runtime_error(fmt::format(
                    "Angelscript exception: {}",
                    ctx->GetExceptionString()
                ));
            }
            throw std::runtime_error("Script not finished");
        }

        return detailed::GetRet<Ret>(ctx);
    }

    int AddSectionFromVfs(
        CScriptBuilder* builder,
        const std::string& filename
    );

    namespace detailed
    {
        template <typename T>
        class function_traits;

        template <typename R, typename... Args>
        struct function_traits<std::function<R(Args...)>>
        {
            static constexpr std::size_t nargs = sizeof...(Args);

            typedef R result_type;

            struct Caller
            {
                asIScriptFunction* func;
                asIScriptContext* ctx;

                Caller(
                    asIScriptFunction* func_,
                    asIScriptContext* ctx_
                ) noexcept : func(func_), ctx(ctx_) {}

                R operator()(
                    Args&&... args
                ) {
                    return Call<result_type>(func, ctx, std::forward<Args>(args)...);
                }
            };
        };
    }

    template <typename Func>
    std::function<Func> GenCallerByDecl(
        asIScriptModule* mod,
        const char* decl,
        asIScriptContext* ctx = asGetActiveContext()
    ) {
        auto* func = mod->GetFunctionByDecl(decl);
        if(!func)
            return std::function<Func>();

        using T = detailed::function_traits<std::function<Func>>;
        return T::Caller(func, ctx);
    }
}

#endif
