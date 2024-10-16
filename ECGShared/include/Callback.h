#ifndef CALLBACK_H
#define CALLBACK_H

template <typename T>
using MEMBER_CB = void (T::*)();
using GLOBAL_CB = void (*)();

template <typename T, typename U>
using CTX_MEMBER_CB = void (T::*)(U *);
template <typename U>
using CTX_GLOBAL_CB = void (*)(U *);

template <class U>
struct GlobalCtxCallback
{
    CTX_GLOBAL_CB<U> callbackFn;
    U *ctx;
};

template <class T>
struct MemberCallback
{
    MEMBER_CB<T> callbackFn;
    T *callbackObject;
};

template <class T, class U>
struct MemberCtxCallback
{
    CTX_MEMBER_CB<T, U> callbackFn;
    T *callbackObject;
    U *ctx;
};

class Callback
{
private:
    GLOBAL_CB globalCallbackFn = nullptr;
    void *globalCtxCallbackFn = nullptr;
    void *memberCallback = nullptr;
    void *memberCtxCallback = nullptr;

    void (*invokeCallback)(void *);

public:
    void (*getInvokeCallback())(void *)
    {
        return this->invokeCallback;
    }

    static void invokeGlobalCallback(void *context)
    {
        Callback *_this = static_cast<Callback *>(context);
        _this->globalCallbackFn();
    }

    template <typename U>
    static void invokeGlobalCtxCallback(void *context)
    {
        Callback *_this = static_cast<Callback *>(context);
        auto *memberCallback = static_cast<GlobalCtxCallback<U> *>(_this->globalCtxCallbackFn);
        memberCallback->callbackFn(memberCallback->ctx);
    }

    template <typename T>
    static void invokeMemberCallback(void *context)
    {
        Callback *_this = static_cast<Callback *>(context);
        auto *memberCallback = static_cast<MemberCallback<T> *>(_this->memberCallback);
        auto memberObject = memberCallback->callbackObject;
        (memberObject->*(memberCallback->callbackFn))();
    }

    template <typename T, typename U>
    static void invokeMemberCtxCallback(void *context)
    {
        Callback *_this = static_cast<Callback *>(context);
        auto *memberCallback = static_cast<MemberCtxCallback<T, U> *>(_this->memberCtxCallback);
        auto memberObject = memberCallback->callbackObject;
        (memberObject->*(memberCallback->callbackFn))(memberCallback->ctx);
    }

    template <typename T>
    Callback(T *obj, MEMBER_CB<T> timerCallback)
    {
        this->memberCallback = new MemberCallback<T>{timerCallback, obj};
        this->invokeCallback = &Callback::invokeMemberCallback<T>;
    }
    template <typename T, typename U>
    Callback(T *obj, CTX_MEMBER_CB<T, U> timerCallback, U *ctx)
    {
        this->memberCtxCallback = new MemberCtxCallback<T, U>{timerCallback, obj, ctx};
        this->invokeCallback = &Callback::invokeMemberCtxCallback<T, U>;
    }
    template <typename U>
    Callback(CTX_GLOBAL_CB<U> timerCallback, U *ctx)
    {
        this->globalCtxCallbackFn = new GlobalCtxCallback<U>{timerCallback, ctx};
        this->invokeCallback = &Callback::invokeGlobalCtxCallback<U>;
    }

    Callback(GLOBAL_CB timerCallback)
    {
        this->globalCallbackFn = timerCallback;
        this->invokeCallback = [](void *context)
        {
            Callback *_this = static_cast<Callback *>(context);
            _this->globalCallbackFn();
        };
    }
    ~Callback()
    {
        if (this->globalCtxCallbackFn != nullptr)
        {
            delete globalCtxCallbackFn;
        }
        if (this->memberCtxCallback != nullptr)
        {
            delete memberCtxCallback;
        }
        if (this->memberCallback != nullptr)
        {
            delete memberCallback;
        }
    }

    void invoke()
    {
        this->invokeCallback(this);
    }
};

#endif